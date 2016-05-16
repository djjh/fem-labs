#ifndef __ZOOM_H
#define __ZOOM_H

#include <cassert>

#include "element.h"
#include "window_events.h"
#include "adhoc_events.h"

class Zoom2D : public Element
{
public:
    Zoom2D()
    : press(false)
    , drag(false)
    , release(false)
    , sx(0.1f)
    , sy(0.1f)
    , wx(0)
    , wy(0)
    {}
    virtual ~Zoom2D() {}
    virtual void setEventManager(EventManager * event_manager)
    {
        Element::setEventManager(event_manager);
        _event_manager->addEventHook(this, &Zoom2D::removeElementEventHooksEventHook);
        _event_manager->addEventHook(this, &Zoom2D::keyInputEventHook);
        _event_manager->addEventHook(this, &Zoom2D::windowSizeEventHook);
    }
    void removeElementEventHooksEventHook(RemoveElementEventHooksEvent const * event)
    {
        if (event->element == this)
        {
            _event_manager->removeEventHook(this, &Zoom2D::removeElementEventHooksEventHook);
            _event_manager->removeEventHook(this, &Zoom2D::keyInputEventHook);
            _event_manager->removeEventHook(this, &Zoom2D::mouseCursorEventHook);
            _event_manager->removeEventHook(this, &Zoom2D::windowSizeEventHook);
        }
    }
    void keyInputEventHook(KeyInputEvent const * event)
    {
        assert(event);
        if (event->key == GLFW_KEY_R)
        {
            if (event->action == GLFW_PRESS)
            {
                press = true;
                
                _event_manager->addEventHook(this, &Zoom2D::mouseCursorEventHook);
            }
            else if (event->action == GLFW_RELEASE)
            {
                release = true;
                drag = false;
                release = false;
                sx = pow(sx, 1 - (xf - xi) / wx);
                sy = pow(sy, 1 + (yf - yi) / wy);
                _event_manager->triggerEvent(new Zoom2DEvent(sx, sy));
                _event_manager->removeEventHook(this, &Zoom2D::mouseCursorEventHook);
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
            float tsx = (float) pow(sx, 1 - (xf - xi) / wx);
            float tsy = (float) pow(sy, 1 + (yf - yi) / wy);
            _event_manager->triggerEvent(new Zoom2DEvent(tsx, tsy));
        }       
    }
    void windowSizeEventHook(WindowSizeEvent const * event)
    {
        assert(event);
        if (wx == 0 || wy == 0)
        {
            wx = (float) event->width;
            wy = (float) event->height;
        }
        
        sx *= (wx / (float) event->width);
        sy *= (wy / (float) event->height);
        _event_manager->triggerEvent(new Zoom2DEvent(sx, sy));

        wx = (float) event->width;
        wy = (float) event->height;
    }
private:
    bool press, drag, release;
    float xi, yi, xf, yf;
    float wx, wy;
    float sx, sy;  // scale percent
};

#endif  // __ZOOM_H
