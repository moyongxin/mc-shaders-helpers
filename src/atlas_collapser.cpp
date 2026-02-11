/**
 * @file atlas_collapser.cpp
 * @author moyongxin
 * @date 2026-02-11
 * @copyright Copyright (c) 2026 moyongxin
 */

/*
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
 * Limitations:
 *  - Only supports up to 8-bit per channel images (e.g., PNG, JPEG).
 *  - Assumes the atlas is perfectly tiled with no padding or margins.
 *  - Images with less than 8 bits per channel (e.g., 4-bit indexed PNG) not
 *      well tested.
 *  - Output is raw binary data without headers or metadata.
 *  - Output is 8-bit per channel; lower bit-depth images will be upscaled(by
 *      stb_image).
 */

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#define STBI_NO_HDR
#include <stb_image.h>

#include <fmt/format.h>

#include <climits>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

namespace {
struct {
    const char *input_file = nullptr;
    const char *output_file = nullptr;

    bool is_horizontal = false;
    bool flip_vertical = false;
    int aux_dim = 0;
} opts;
} // namespace

void parse_cmd(int argc, char *argv[]) {
    const char *help_msg =
        "Usage: <path-to-atlas_collapser> "
        "<input_file> [-o <output_file>] [--help] "
        "[-h] [-v] [-i] [-a <aux_dimension>]\n"
        "Treats `input_file` as a texture atlas, "
        "collapses it into a 3D texture bin, and writes to `output_file`.\n"
        "\t--help\tShow this help message and exit.\n"
        "\t-o\tSpecify the output file path. If not provided, defaults to "
        "{input_file}.bin\n"
        "\t-h\tAssume the atlas is laid out horizontally.\n"
        "\t-v\tAssume the atlas is laid out vertically.(default)\n"
        "\t-i\tFlip the texture vertically.\n"
        "\t-a\tSpecify the auxiliary dimension of a single tile.\n";

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (strcmp(arg, "--help") == 0) {
            fmt::print(stderr, "{}", help_msg);
            exit(0);
        } else if (strcmp(arg, "-h") == 0) {
            opts.is_horizontal = true;
        } else if (strcmp(arg, "-v") == 0) {
            opts.is_horizontal = false;
        } else if (strcmp(arg, "-i") == 0) {
            opts.flip_vertical = true;
        } else if (strcmp(arg, "-o") == 0) {
            if (i + 1 < argc) {
                opts.output_file = argv[++i];
            } else {
                fmt::println(stderr, "Error: -o option requires an output file "
                                     "argument.");
                exit(1);
            }
        } else if (strcmp(arg, "-a") == 0) {
            if (i + 1 < argc) {
                const char *value_str = argv[++i];
                char *end = nullptr;
                long value = std::strtol(value_str, &end, 10);
                if (!end || *end != '\0' || value <= 0 || value > INT_MAX) {
                    fmt::println(
                        stderr,
                        "Error: -a requires a positive integer argument.");
                    exit(1);
                }
                opts.aux_dim = static_cast<int>(value);
            } else {
                fmt::println(stderr,
                             "Error: -a option requires an integer argument.");
                exit(1);
            }
        } else if (!opts.input_file) {
            opts.input_file = arg;
        } else {
            fmt::println(stderr, "Error: Unrecognized argument '{}'.", arg);
            fmt::print(stderr, "{}", help_msg);
            exit(1);
        }
    }

    if (!opts.input_file) {
        fmt::println(stderr, "Error: Input file is required.");
        fmt::print(stderr, "{}", help_msg);
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    parse_cmd(argc, argv);

    int x, y, comp;
    auto image_data = stbi_load(opts.input_file, &x, &y, &comp, 0);
    if (!image_data) {
        fmt::println(stderr, "Error: Failed to load image '{}': {}",
                     opts.input_file, stbi_failure_reason());
        return 1;
    }

    int auto_dim = opts.is_horizontal ? y : x;
    int tile_width = opts.is_horizontal
                         ? (opts.aux_dim > 0 ? opts.aux_dim : auto_dim)
                         : auto_dim;
    int tile_height = opts.is_horizontal
                          ? auto_dim
                          : (opts.aux_dim > 0 ? opts.aux_dim : auto_dim);
    if (tile_width <= 0 || tile_height <= 0) {
        fmt::println(stderr, "Error: Invalid tile dimensions.");
        stbi_image_free(image_data);
        return 1;
    }

    int main_dim = opts.is_horizontal ? x : y;
    int tile_main_dim = opts.is_horizontal ? tile_width : tile_height;
    if (main_dim % tile_main_dim != 0) {
        fmt::println(stderr,
                     "Warning: Atlas dimension ({}) is not divisible by tile "
                     "size ({}).",
                     main_dim, tile_main_dim);
    }

    int tile_count = main_dim / tile_main_dim;

    std::string output_path;
    if (!opts.output_file) {
        output_path = fmt::format("{}.bin", opts.input_file);
        opts.output_file = output_path.c_str();
    }

    std::ofstream out(opts.output_file, std::ios::binary);
    if (!out) {
        fmt::println(stderr, "Error: Failed to open output file '{}'.",
                     opts.output_file);
        stbi_image_free(image_data);
        return 1;
    }

    size_t row_bytes = static_cast<size_t>(tile_width) * comp;
    for (int tile = 0; tile < tile_count; ++tile) {
        for (int row = 0; row < tile_height; ++row) {
            int src_row = opts.flip_vertical ? (tile_height - 1 - row) : row;
            int src_x = opts.is_horizontal ? tile * tile_width : 0;
            int src_y =
                opts.is_horizontal ? src_row : tile * tile_height + src_row;
            const stbi_uc *src =
                image_data + (static_cast<size_t>(src_y) * x + src_x) * comp;
            out.write(reinterpret_cast<const char *>(src),
                      static_cast<std::streamsize>(row_bytes));
            if (!out) {
                fmt::println(stderr, "Error: Failed writing output file '{}'.",
                             opts.output_file);
                stbi_image_free(image_data);
                return 1;
            }
        }
    }

    fmt::println("Wrote {} layers ({}x{}, {} channels) to '{}'", tile_count,
                 tile_width, tile_height, comp, opts.output_file);
    stbi_image_free(image_data);
    return 0;
}
