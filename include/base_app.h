#pragma once

#include <SDL2/SDL_video.h>
#include <SDL_events.h>
#include <stdint.h>

struct BaseApplication {
    virtual ~BaseApplication() = default;

    virtual bool should_exit() { return false; }

    virtual const char *requested_window_title() { return "Base Application"; }

    virtual void requested_window_position(int &x, int &y) {
        x = SDL_WINDOWPOS_CENTERED;
        y = SDL_WINDOWPOS_CENTERED;
    }

    virtual void requested_window_size(int &width, int &height) {
        width = 800;
        height = 600;
    }

    virtual void requested_window_flags(uint32_t &flags) { flags = 0; }

    virtual void on_window_resized(int width, int height) {
        (void)width;
        (void)height;
    }

    virtual void on_window_created(SDL_Window *window) { (void)window; }
    virtual void on_render_loop_begin() {}

    virtual void on_key_pressed(SDL_Event &event) { (void)event; }
    virtual void on_key_released(SDL_Event &event) { (void)event; }

    virtual void on_mouse_button_down(SDL_Event &event) { (void)event; }
    virtual void on_mouse_button_up(SDL_Event &event) { (void)event; }
    virtual void on_mouse_moved(SDL_Event &event) { (void)event; }

    virtual void render() {}

    virtual void before_swap() {}
    virtual void after_swap() {}
};

extern BaseApplication *CreateApplication(int argc, char *argv[]);
