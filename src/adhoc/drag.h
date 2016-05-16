#ifndef __DRAG_H
#define __DRAG_H

#include <cassert>

#include "element.h"
#include "window_events.h"
#include "adhoc_events.h"

// accumulates drag.... normalized and such
class Drag : public Element
{
public:
    Drag(int key)
    : _key(key)
    , press(false)
    , drag(false)
    , release(false)
    , dx(0.0f)
    , dy(0.0f)
    , wx(0)
    , wy(0)
    {}
    virtual ~Drag() {}
    virtual void setEventManager(EventManager * event_manager)
    {
        Element::setEventManager(event_manager);
        _event_manager->addEventHook(this, &Drag::removeElementEventHooksEventHook);
        _event_manager->addEventHook(this, &Drag::windowSizeEventHook);
        _event_manager->addEventHook(this, &Drag::keyInputEventHook);
    }
    void removeElementEventHooksEventHook(RemoveElementEventHooksEvent const * event)
    {
        if (event->element == this)
        {
            _event_manager->removeEventHook(this, &Drag::removeElementEventHooksEventHook);
            _event_manager->removeEventHook(this, &Drag::windowSizeEventHook);
            _event_manager->removeEventHook(this, &Drag::keyInputEventHook);
            _event_manager->removeEventHook(this, &Drag::mouseCursorEventHook);
        }
    }
    void windowSizeEventHook(WindowSizeEvent const * event)
    {
        assert(event);
        wx = (float) event->width;
        wy = (float) event->height;
    }
    void keyInputEventHook(KeyInputEvent const * event)
    {
        assert(event);
        if (event->key == _key)
        {
            if (event->action == GLFW_PRESS)
            {
                press = true;
                
                _event_manager->addEventHook(this, &Drag::mouseCursorEventHook);
            }
            else if (event->action == GLFW_RELEASE)
            {
                release = true;
                drag = false;
                release = false;
                dx += xf - xi;
                dy += yf - yi;
                _event_manager->triggerEvent(new DragEvent(_key, dx, dy));
                _event_manager->removeEventHook(this, &Drag::mouseCursorEventHook);
            }
        }
            
    }
    void mouseCursorEventHook(MouseCursorEvent const * event)
    {
        assert(event);
        if (press)
        {
            xi = (float) event->xpos / wx;
            yi = (float) event->ypos / wy;
            press = false;
            drag = true;
        }
        if (drag)
        {
            xf = (float) event->xpos / wx;
            yf = (float) event->ypos / wy;
            float tmp_dx = dx + xf - xi;
            float tmp_dy = dy + yf - yi;
            _event_manager->triggerEvent(new DragEvent(_key, tmp_dx, tmp_dy));
        }       
    }
private:
    int _key;
    bool press, drag, release;
    float xi, yi, xf, yf;
    float wx, wy;
    float dx, dy;  // drag accumulation
};

#endif  // __DRAG_H