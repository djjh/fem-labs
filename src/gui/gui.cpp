#include "gui.h"

Gui::Gui()
{}

Gui::~Gui()
{}

bool Gui::initialize()
{
    _window.print_info();

    // add event hooks
    _window.event_manager.addEventHook(this, &Gui::keyInputEventHook);
    _window.event_manager.addEventHook(this, &Gui::framebufferSizeEventHook);
    _window.event_manager.addEventHook(this, &Gui::windowRefreshEventHook);

    // trigger callbacks to initialize controls properly
    // you could argue that it would be better to just
    // construct them with the correct value from the
    // beginning, but this works for now
    // TODO: what I just said.
    int fb_width, fb_height, win_width, win_height;
    double xpos, ypos;
    _window.get_framebuffer_size(&fb_width, &fb_height); 
    _window.get_window_size(&win_width, &win_height);
    _window.get_cursor_position(&xpos, &ypos);
    _window.event_manager.triggerEvent(new FramebufferSizeEvent(fb_width, fb_height));
    _window.event_manager.triggerEvent(new WindowSizeEvent(win_width, win_height));
    _window.event_manager.triggerEvent(new MouseCursorEvent(xpos, ypos));

    return true;
}

bool Gui::add_element(Element * element)
{
    if (!element)
        return false;

    if (!_elements.insert(element).second)
        return false;

    element->setEventManager(&_window.event_manager);

    return true;
}

bool Gui::remove_element(Element * element)
{
    if (!element)
        return false;

    if (_elements.erase(element) == 0)
        return false;

    _window.event_manager.triggerEvent(new RemoveElementEventHooksEvent(element));

    return true;
}

bool Gui::step(double dt)
{
    // trigger render event...
    render();

    // poll events...
    static double last_time = get_time();
    do
    {
        _window.poll_events();
    } while ((get_time() - last_time) < dt);
    last_time = get_time();

    return true;
}

// change to an event (render event)
bool Gui::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _window.event_manager.triggerEvent(new WindowRenderEvent());
    _window.swap_buffers();

    return true;
}

void Gui::keyInputEventHook(KeyInputEvent const * event)
{
    assert(event);

    if (event->key == GLFW_KEY_ESCAPE && event->action == GLFW_PRESS)
    {
       _window.set_should_close();
    }
}

void Gui::framebufferSizeEventHook(FramebufferSizeEvent const * event)
{
    assert(event);
    glViewport(0, 0, event->width, event->height);
}

void Gui::windowRefreshEventHook(WindowRefreshEvent const * event)
{
    assert(event);
    render();
}


double Gui::get_time()
{
    return _window.get_time();
}

bool  Gui::should_close()
{
    return _window.should_close();
}
