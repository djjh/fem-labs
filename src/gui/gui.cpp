#include "gui.h"

Gui::Gui()
: _pan(NULL)
, _shaderprogram(NULL)
, _zoom(NULL)
{}

Gui::~Gui()
{
    if (_shaderprogram)
    {
        delete _shaderprogram;
        _shaderprogram = NULL;
    }
    if (_pan)
    {
        delete _pan;
        _pan = NULL;
    }
    if (_zoom)
    {
        delete _zoom;
        _zoom = NULL;
    }
}

bool Gui::initialize()
{
    _window.print_info();

    // initialize graphics
    _shaderprogram = new VertexColorShaderProgram;
    if (!_shaderprogram)
    {
        return false;
    }
    _shaderprogram->initialize();  // TODO: should return false on unsuccessful or something

    // initialize controls
    _pan = new Pan;
    if (!_pan)
    {
        delete _shaderprogram;
        _shaderprogram = NULL;
        return false;
    }
    _zoom = new Zoom(100.f);
    if (!_zoom)
    {
        delete _shaderprogram;
        _shaderprogram = NULL;
        delete _pan;
        _pan = NULL;
        return false;
    }

    // add event hooks
    _window.event_manager.addEventHook(this, &Gui::keyInputEventHook);
    _window.event_manager.addEventHook(this, &Gui::mouseButtonEventHook);
    _window.event_manager.addEventHook(this, &Gui::mouseCursorEventHook);
    _window.event_manager.addEventHook(this, &Gui::windowSizeEventHook);
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

    element->addEventHooks(&_window.event_manager);

    return true;
}

bool Gui::remove_element(Element * element)
{
    if (!element)
        return false;

    if (_elements.erase(element) == 0)
        return false;

    element->removeEventHooks(&_window.event_manager);

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
    glClear(GL_COLOR_BUFFER_BIT);
    _window.event_manager.triggerEvent(new RenderElementsEvent(_shaderprogram));
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

    _pan->set_dragging(event->key == GLFW_KEY_W && (event->action == GLFW_PRESS || event->action == GLFW_REPEAT));
    _zoom->set_dragging(event->key == GLFW_KEY_R && (event->action == GLFW_PRESS || event->action == GLFW_REPEAT));
}

void Gui::mouseButtonEventHook(MouseButtonEvent const * event)
{
    assert(event);
}

void Gui::mouseCursorEventHook(MouseCursorEvent const * event)
{
    assert(event);
    assert(_zoom);
    assert(_pan);
    assert(_shaderprogram);

    // update controls
    _zoom->set_position(event->xpos, event->ypos);
    _pan->set_position(event->xpos, event->ypos);
    _pan->set_scale(1.f / _zoom->get_pixels_per_unit_x(), 1.f / _zoom->get_pixels_per_unit_y());

    // update view
    mat4x4 v;
    mat4x4_identity(v);
    _zoom->apply(v);
    _pan->apply(v);
    // scoot back from xy plane
    // TODO: more general solution using pan (z) and pan constructor
    mat4x4_translate_in_place(v, 0.f, 0.f, -8.f);
    _shaderprogram->set_v(v);
}

void Gui::windowSizeEventHook(WindowSizeEvent const * event)
{
    assert(event);
    assert(_pan);
    assert(_zoom);
    assert(_shaderprogram);

    // update controls
    _zoom->set_in_range(0, event->width, event->height, 0);
    _pan->set_in_range(0, event->width, event->height, 0);
    _pan->set_out_range(-event->width, event->width, -event->height, event->height);

    // update perspective
    mat4x4 p;
    mat4x4_ortho(p, -event->width, event->width, -event->height, event->height, 0.1f, 10.f);
    _shaderprogram->set_p(p);
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
