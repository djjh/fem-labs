#ifndef __WINDOW_H
#define __WINDOW_H

#include <cassert>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "events.h"
#include "window_events.h"

/*
 * Many members should only be accessed from main thread,
 * as they use glfw functions with such restrictions. This
 * is really only an issue with osx and sometimes linux.
 *
 * TODO: make the class wide initialization thread safe
 */
class Window
{
public:
    Window(char const * title = "");
    ~Window();
    bool should_close();
    void set_should_close();
    void swap_buffers();
    void poll_events();
    void print_info();
    void get_window_size(int * x, int * y);
    void get_framebuffer_size(int * x, int * y);
    void get_cursor_position(double * x, double * y);
    static double get_time();
    EventManager event_manager;

private:
    static int _instance_count;
    static void system_initialize_window();
    static void system_terminate_window();
    GLFWwindow * _window;
};

#endif  // __WINDOW_H