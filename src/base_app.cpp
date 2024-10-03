#include "base_app.h"

#include <SDL.h>
#include <fmt/format.h>
#include <glad/glad.h>

int main(int argc, char *argv[]) {

#ifdef _WIN32
    // Set up DPI awareness, Disable DPI scaling
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING, "0");
#endif

    SDL_Init(SDL_INIT_EVERYTHING);

    auto app = CreateApplication(argc, argv);

    const char *title = app->requested_window_title();
    int x, y, w, h;
    app->requested_window_position(x, y);
    app->requested_window_size(w, h);
    uint32_t flags = 0;
    app->requested_window_flags(flags);

    auto window =
        SDL_CreateWindow(title, x, y, w, h,
                         flags | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);

    app->on_window_created(window);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    auto gl_context = SDL_GL_CreateContext(window);

    SDL_GL_MakeCurrent(window, gl_context);
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        fmt::println("Failed to initialize GLAD");
        return 1;
    }

    app->on_render_loop_begin();

    SDL_Event event{};

    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                app->on_key_pressed(event);
            }

            if (event.type == SDL_KEYUP) {
                app->on_key_released(event);
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                app->on_mouse_button_down(event);
            }

            if (event.type == SDL_MOUSEBUTTONUP) {
                app->on_mouse_button_up(event);
            }

            if (event.type == SDL_MOUSEMOTION) {
                app->on_mouse_moved(event);
            }

            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    app->on_window_resized(event.window.data1,
                                           event.window.data2);
                }
            }

            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    running = false;
                }
            }

            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    app->on_window_resized(event.window.data1,
                                           event.window.data2);
                }
            }
        }

        if (app->should_exit() || !running) {
            break;
        }

        app->render();

        app->before_swap();
        SDL_GL_SwapWindow(window);
        app->after_swap();
    }
    delete app;
    SDL_Quit();
    return 0;
}
