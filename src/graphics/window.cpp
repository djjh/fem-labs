#include "window.h"

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void glfw_key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    EventManager * event_manager = (EventManager *) glfwGetWindowUserPointer(window);
    assert(event_manager);
    event_manager->triggerEvent(new KeyInputEvent(key, scancode, action, mods));
}

void glfw_mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
    EventManager * event_manager = (EventManager *) glfwGetWindowUserPointer(window);
    assert(event_manager);
    event_manager->triggerEvent(new MouseButtonEvent(button, action, mods));
}

void glfw_cursor_pos_callback(GLFWwindow * window, double xpos, double ypos)
{
    EventManager * event_manager = (EventManager *) glfwGetWindowUserPointer(window);
    assert(event_manager);
    event_manager->triggerEvent(new MouseCursorEvent(xpos, ypos));
}

void glfw_window_size_callback(GLFWwindow * window, int width, int height)
{ 
    EventManager * event_manager = (EventManager *) glfwGetWindowUserPointer(window);
    assert(event_manager);
    event_manager->triggerEvent(new WindowSizeEvent(width, height));
}

void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    EventManager * event_manager = (EventManager *) glfwGetWindowUserPointer(window);
    assert(event_manager);
    event_manager->triggerEvent(new FramebufferSizeEvent(width, height));
}

void glfw_window_refresh_callback(GLFWwindow* window)
{
    EventManager * event_manager = (EventManager *) glfwGetWindowUserPointer(window);
    assert(event_manager);
    event_manager->triggerEvent(new WindowRefreshEvent);
}

Window::Window(char const * title)
{
    // create a window
    system_initialize_window();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    _window = glfwCreateWindow(640, 480, title, NULL, NULL);
    if (!_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // setup opengl context
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);

    // load opengl extensions (required)
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    //
    glfwSetWindowUserPointer(_window, &event_manager);

    // set window callbacks
    glfwSetKeyCallback(_window, glfw_key_callback);
    glfwSetMouseButtonCallback(_window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(_window, glfw_cursor_pos_callback);
    glfwSetWindowSizeCallback(_window, glfw_window_size_callback);
    glfwSetFramebufferSizeCallback(_window, glfw_framebuffer_size_callback);
    glfwSetWindowRefreshCallback(_window, glfw_window_refresh_callback);
}
Window::~Window()
{
    // clean up window
    glfwDestroyWindow(_window);
    system_terminate_window();
}
bool Window::should_close()
{
    return glfwWindowShouldClose(_window);
}
void Window::set_should_close()
{
    glfwSetWindowShouldClose(_window, GLFW_TRUE);
}
void Window::swap_buffers()
{
    glfwSwapBuffers(_window);
}
void Window::poll_events()
{
    glfwPollEvents();
}
void Window::print_info()
{
    // print opengl info
    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    fprintf(stderr, "OpenGL version: %d.%d.%d\n", major, minor, rev);
    fprintf(stderr, "  GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void Window::get_window_size(int * x, int * y)
{
    glfwGetWindowSize(_window, x, y);
}

void Window::get_framebuffer_size(int * x, int * y)
{
    glfwGetFramebufferSize(_window, x, y);
}

void Window::get_cursor_position(double * x, double * y)
{
    glfwGetCursorPos(_window, x, y);
}

double Window::get_time()
{
    if (_instance_count == 0)
    {
        glfwSetTime(0.0);
        return 0.0;
    }
    else
    {
        return glfwGetTime();
    }
}

int Window::_instance_count = 0;

void Window::system_initialize_window()
{
    if (_instance_count++ == 0)
    {
        // initialize window system
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            exit(EXIT_FAILURE);
    }
}

void Window::system_terminate_window()
{
    if (--_instance_count == 0)
    {
        // terminate window system
        glfwTerminate();
    }
}
