#ifndef __CAMERA_H
#define __CAMERA_H

#include <cassert>

#include "element.h"
#include "window_events.h"
#include "adhoc_events.h"

#include <linmath.h>

class Camera2D : public Element
{
public:
    Camera2D() : _sx(1.f), _sy(1.f), _tx(0.f), _ty(0.f) {}
    virtual ~Camera2D() {}
    virtual void setEventManager(EventManager * event_manager)
    {
        Element::setEventManager(event_manager);

        _event_manager->addEventHook(this, &Camera2D::removeElementEventHooksEventHook);
        _event_manager->addEventHook(this, &Camera2D::windowSizeEventHook);
        _event_manager->addEventHook(this, &Camera2D::pan2DEventHook);
        _event_manager->addEventHook(this, &Camera2D::zoom2DEventHook);
    }
    void removeElementEventHooksEventHook(RemoveElementEventHooksEvent const * event)
    {
        if (event->element == this)
        {
            _event_manager->removeEventHook(this, &Camera2D::removeElementEventHooksEventHook);
            _event_manager->removeEventHook(this, &Camera2D::windowSizeEventHook);
            _event_manager->removeEventHook(this, &Camera2D::pan2DEventHook);
            _event_manager->removeEventHook(this, &Camera2D::zoom2DEventHook);
        }
    }
    void windowSizeEventHook(WindowSizeEvent const * event)
    {
        assert(event);
        update();
        _event_manager->triggerEvent(new Camera2DEvent(&_p, &_v));
    }
    void zoom2DEventHook(Zoom2DEvent const * event)
    {
        assert(event);
        _sx = event->x_scale;
        _sy = event->y_scale;
        update();
    }
    void pan2DEventHook(Pan2DEvent const * event)
    {
        assert(event);
        _tx = event->x_translate;
        _ty = event->y_translate;
        update();
    }
    void update()
    {
        mat4x4 S;
        mat4x4_identity(S);
        mat4x4_scale_aniso(S, S, _sx, _sy, 1.f);

        mat4x4 T;
        mat4x4_translate(T, _tx, _ty, 0.f);

        // update v
        mat4x4_identity(_v);
        mat4x4_mul(_v, T, _v);
        mat4x4_mul(_v, S, _v);
        mat4x4_translate_in_place(_v, 0.f, 0.f, -5.f);

        // update p
        mat4x4_identity(_p);
        mat4x4_ortho(_p, -0.5f , 0.5f, -0.5f, 0.5f, 0.1f, 10.f);

        _event_manager->triggerEvent(new Camera2DEvent(&_p, &_v));
    }

private:
    float _sx, _sy, _tx, _ty;
    mat4x4 _p;
    mat4x4 _v;
};

#endif  // __CAMERA_H
