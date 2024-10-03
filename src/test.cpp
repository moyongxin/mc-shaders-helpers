#include "base_app.h"

#include <SDL2/SDL.h>
#include <glad/glad.h>

namespace {
const char *vertex_shader_source = R"(
#version 330 core
layout(location = 0) in vec2 pos;
void main() {
    gl_Position = vec4(pos, 0.0, 1.0);
}

)";

const char *fragment_shader_source = R"(
#version 330 core
layout(location = 0) out vec3 color;
void main() {
    color = vec3(1.0, 0.0, 0.0);
}

)";

const float full_quad[] = {
    -1.0f, -1.0f, 3.0f, -1.0f, -1.0f, 3.0f,
};
} // namespace

struct TestApplication final : BaseApplication {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint shader_program = 0;
    GLuint vertex_shader = 0;
    GLuint fragment_shader = 0;
    bool running = true;

    bool should_exit() override { return !running; }

    const char *requested_window_title() override { return "Test Application"; }

    virtual void on_render_loop_begin() override {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(full_quad), full_quad,
                     GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                              nullptr);
        glEnableVertexAttribArray(0);

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
        glCompileShader(vertex_shader);

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);

        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);
    }

    virtual void on_key_pressed(SDL_Event &event) override {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        }
    }

    virtual void render() override {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_CULL_FACE);

        glUseProgram(shader_program);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};

BaseApplication *CreateApplication(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    return new TestApplication();
}
