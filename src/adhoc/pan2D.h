#ifndef __PAN_H
#define __PAN_H

#include <cassert>

#include "element.h"
#include "window_events.h"
#include "adhoc_events.h"

#include <linmath.h>

class Pan2D : public Element
{
public:
    Pan2D()
    : press(false)
    , drag(false)
    , release(false)
    , sx(0.f)
    , sy(0.f)
    , tx(0.f)
    , ty(0.f)
    {}
    virtual ~Pan2D() {}
    virtual void setEventManager(EventManager * event_manager)
    {
        Element::setEventManager(event_manager);
        _event_manager->addEventHook(this, &Pan2D::removeElementEventHooksEventHook);
        _event_manager->addEventHook(this, &Pan2D::keyInputEventHook);
        _event_manager->addEventHook(this, &Pan2D::mouseCursorEventHook);
        _event_manager->addEventHook(this, &Pan2D::zoom2DEventHook);
        _event_manager->addEventHook(this, &Pan2D::windowSizeEventHook);
    }
    void removeElementEventHooksEventHook(RemoveElementEventHooksEvent const * event)
    {
        if (event->element == this)
        {
            _event_manager->removeEventHook(this, &Pan2D::removeElementEventHooksEventHook);
            _event_manager->removeEventHook(this, &Pan2D::keyInputEventHook);
            _event_manager->removeEventHook(this, &Pan2D::mouseCursorEventHook);
            _event_manager->removeEventHook(this, &Pan2D::zoom2DEventHook);
            _event_manager->removeEventHook(this, &Pan2D::windowSizeEventHook);
        }
    }
    void keyInputEventHook(KeyInputEvent const * event)
    {
        assert(event);
        if (event->key == GLFW_KEY_W)
        {
            if (event->action == GLFW_PRESS)
            {
                press = true;
                
                _event_manager->addEventHook(this, &Pan2D::mouseCursorEventHook);
            }
            else if (event->action == GLFW_RELEASE)
            {
                release = true;
                drag = false;
                release = false;
                tx = tx + (xf - xi)/wx/sx;
                ty = ty - (yf - yi)/wy/sy;
                _event_manager->triggerEvent(new Pan2DEvent(tx, ty));
                _event_manager->removeEventHook(this, &Pan2D::mouseCursorEventHook);
            }
        }
            
    }
    void mouseCursorEventHook(MouseCursorEvent const * event)
    {
        assert(event);
        if (press)
        {
            xi = (float) event->xpos;
            yi = (float) event->ypos;
            press = false;
            drag = true;
        }
        if (drag)
        {
            xf = (float) event->xpos;
            yf = (float) event->ypos;
            float ttx = tx + (xf - xi)/wx/sx;
            float tty = ty - (yf - yi)/wy/sy;
            _event_manager->triggerEvent(new Pan2DEvent(ttx, tty));
        }       
    }
    void zoom2DEventHook(Zoom2DEvent const * event)
    {
        assert(event);
        sx = (float) event->x_scale;
        sy = (float) event->y_scale;
    }
    void windowSizeEventHook(WindowSizeEvent const * event)
    {
        assert(event);
        wx = (float) event->width;
        wy = (float) event->height;
    }
private:
    bool press, drag, release;
    float xi, yi, xf, yf;
    float tx, ty;
    float sx, sy;  // pixels per unit
    float wx, wy;
};

#endif  // __PAN_H