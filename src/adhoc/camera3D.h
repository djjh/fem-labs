#ifndef __CAMERA_3D_H
#define __CAMERA_3D_H

#include "element.h"
#include "window_events.h"
#include "adhoc_events.h"

#define _USE_MATH_DEFINES
#include <math.h>

class Camera3D : public Element
{
public:
    Camera3D()
    {
        fov_default = 7.9f;
        fov = fov_default;
        mat4x4_identity(T1);
        mat4x4_identity(T2);
        mat4x4_identity(R);
        update();
    }
    virtual ~Camera3D() {}
    virtual void setEventManager(EventManager * event_manager)
    {
        Element::setEventManager(event_manager);
        _event_manager->addEventHook(this, &Camera3D::removeElementEventHooksEventHook);
        _event_manager->addEventHook(this, &Camera3D::windowSizeEventHook);
        _event_manager->addEventHook(this, &Camera3D::ballEventHook);
        _event_manager->addEventHook(this, &Camera3D::dragEventHook);
    }
    void removeElementEventHooksEventHook(RemoveElementEventHooksEvent const * event)
    {
        if (event->element == this)
        {
            _event_manager->removeEventHook(this, &Camera3D::removeElementEventHooksEventHook);
            _event_manager->removeEventHook(this, &Camera3D::windowSizeEventHook);
            _event_manager->removeEventHook(this, &Camera3D::ballEventHook);
            _event_manager->removeEventHook(this, &Camera3D::dragEventHook);
        }
    }
    void windowSizeEventHook(WindowSizeEvent const * event)
    {
        assert(event);
        update();
        wx = (float) event->width;
        wy = (float) event->height;
        update();
        _event_manager->triggerEvent(new Camera3DEvent(&_p, &_v));
    }
    void ballEventHook(BallEvent const * event)
    {
        assert(event && event->quaternion && event->translation1 && event->translation2);
        quat & q =  *(event->quaternion);
        vec3 & t1 = *(event->translation1);
        vec3 & t2 = *(event->translation2);
        mat4x4_from_quat(R, q);
        mat4x4_translate(T1, -t1[0], -t1[1], -t1[2]);
        mat4x4_translate(T2, -t2[0], -t2[1], -t2[2]);
        update();
        _event_manager->triggerEvent(new Camera3DEvent(&_p, &_v));
    }
    void dragEventHook(DragEvent const * event)
    {
        assert(event);
        if (event->key != GLFW_KEY_F)
            return;

        fov = fov_default*event->y_drag + fov_default;// set "zero..."
        update();
        _event_manager->triggerEvent(new Camera3DEvent(&_p, &_v));
    }
    void update()
    {
        mat4x4 S;
        mat4x4_identity(S);
        mat4x4_scale_aniso(S, S, 0.1f, 0.1f, 1.f);

        // update v
        mat4x4_identity(_v);
        mat4x4_mul(_v, T2, _v);
        mat4x4_mul(_v, R, _v);
        mat4x4_mul(_v, T1, _v);
        mat4x4_mul(_v, S, _v);

        // update p
        mat4x4_identity(_p);
        // mat4x4_ortho(_p, -5.0f , 5.0f, -5.0f, 5.0f, 0.1f, 10000.f);
        mat4x4_perspective(_p, (float) fov / 180.f * (float) M_PI, (float) wx / wy , 0.1f, 1000.f);
    }

private:
    float fov, fov_default;
    float wx, wy;
    mat4x4 R;
    mat4x4 T1, T2;
    mat4x4 _p;
    mat4x4 _v;
};

#endif  // __CAMERA_3D_H