#ifndef __BALL_H
#define __BALL_H

#include "element.h"
#include "window_events.h"
#include "draw.h"
#include "linmath.h"

class Ball : public Element
{
public:
    Ball()
    : press(false)
    , drag(false)
    , release(false)
    , radius(1.f) // should probably be related to viewport width.....TODO!
    {
        quat_identity(_q_now);
        quat_identity(_q_drag);

        _lookat[0] = 0.f;
        _lookat[1] = 0.f;
        _lookat[2] = 0.f;

        // rotation origin relative to camera
        // (look at location)
        _center[0] = 0.0f;
        _center[1] = 0.0f;
        _center[2] = 50.f; // 

        // this is just input window, it has no effect on the center of rotation...
        _window_center[0] = 0.f;
        _window_center[1] = 0.f;
        _window_center[2] = 0.f;
    }
    virtual ~Ball() {}
    virtual void setEventManager(EventManager * event_manager)
    {
        Element::setEventManager(event_manager);
        _event_manager->addEventHook(this, &Ball::removeElementEventHooksEventHook);
        _event_manager->addEventHook(this, &Ball::windowSizeEventHook);
        _event_manager->addEventHook(this, &Ball::keyInputEventHook);
        _event_manager->addEventHook(this, &Ball::removeElementEventHooksEventHook);
        _event_manager->addEventHook(this, &Ball::dragEventHook);
    }
    void removeElementEventHooksEventHook(RemoveElementEventHooksEvent const * event)
    {
        if (event->element == this)
        {
            _event_manager->removeEventHook(this, &Ball::removeElementEventHooksEventHook);
            _event_manager->removeEventHook(this, &Ball::windowSizeEventHook);
            _event_manager->removeEventHook(this, &Ball::keyInputEventHook);
            _event_manager->removeEventHook(this, &Ball::removeElementEventHooksEventHook);
            _event_manager->removeEventHook(this, &Ball::dragEventHook);
        }
    }
    void windowSizeEventHook(WindowSizeEvent const * event)
    {
        assert(event);
        wx = (float) event->width;
        wy = (float) event->height;
        _event_manager->triggerEvent(new BallEvent(&_q_now, &_center, &_lookat));
    }
    void keyInputEventHook(KeyInputEvent const * event)
    {
        assert(event);
        if (event->key == GLFW_KEY_E)
        {
            if (event->action == GLFW_PRESS)
            {
                press = true;
                _event_manager->addEventHook(this, &Ball::mouseCursorEventHook);
                _event_manager->addEventHook(this, &Ball::shaderRenderEvent);
            }
            else if (event->action == GLFW_RELEASE)
            {
                release = true;
                drag = false;
                release = false;

                quat q_tmp;
                quat_identity(q_tmp);
                quat_mul(q_tmp, _q_drag, _q_now);
                for (int i = 0; i < 4; ++i)
                    _q_now[i] = q_tmp[i];
                
                _event_manager->triggerEvent(new BallEvent(&_q_now, &_center, &_lookat));
                _event_manager->removeEventHook(this, &Ball::mouseCursorEventHook);
                _event_manager->removeEventHook(this, &Ball::shaderRenderEvent);
            }
        }

        if (event->key == GLFW_KEY_W)
        {
            if (event->action == GLFW_PRESS)
            {
                _event_manager->addEventHook(this, &Ball::shaderRenderEvent);
            }
            else if (event->action == GLFW_RELEASE)
            {
                _event_manager->removeEventHook(this, &Ball::shaderRenderEvent);
            }
        }
            
    }
    void mouseCursorEventHook(MouseCursorEvent const * event)
    {
        assert(event);
        if (press)
        {
            press = false;
            drag = true;
            vec2 m;
            m[0] =  (float) event->xpos / wx;
            m[1] = 1.f -  (float) event->ypos / wy;
            cursor2ball(_v_down, m, _window_center, radius);
        }
        if (drag)
        {
            vec2 m;
            m[0] =  (float) event->xpos / wx;
            m[1] = 1.f -  (float) event->ypos / wy;
            cursor2ball(_v_now, m, _window_center, radius);
            arc2quat(_q_drag, _v_down, _v_now);

            quat q_tmp;
            quat_identity(q_tmp);
            quat_mul(q_tmp, _q_drag, _q_now);

            _event_manager->triggerEvent(new BallEvent(&q_tmp, &_center, &_lookat));
        }       
    }
    void dragEventHook(DragEvent const * event)
    {
        assert(event);
        if (event->key != GLFW_KEY_W)
            return;

        _lookat[0] = -10.f*event->x_drag;
        _lookat[1] = 10.f*event->y_drag;
        _event_manager->triggerEvent(new BallEvent(&_q_now, &_center, &_lookat));
    }
    void cursor2ball(vec3 & v, vec2 const & mouse, vec3 const & center, float radius) const
    {
        assert(radius);
        float mag;
        v[0] = (mouse[0] - center[0]) / radius;
        v[1] = (mouse[1] - center[1]) / radius;
        mag = v[0] * v[0] + v[1] * v[1];
        if (mag > 1.f)
        {
            float scale = 1.f / sqrt(mag);
            v[0] *= scale;
            v[1] *= scale;
            v[2] = 0;
        }
        else
        {
            v[2] = sqrt(1.f - mag);
        }
    }
    void arc2quat(quat & q, vec3 const & from, vec3 const & to)
    {
        q[0] = from[1] * to[2] - from[2] * to[1];
        q[1] = from[2] * to[0] - from[0] * to[2];
        q[2] = from[0] * to[1] - from[1] * to[0];
        q[3] = from[0] * to[0] + from[1] * to[1] + from[2] * to[2];
    }
    static inline void print(char const * s, quat const & q)
    {
        assert(s);
        printf("%s, (% .3f", s, q[0]);
        for (int i = 1; i < 4; ++i)
            printf(", % .3f", q[i]);
        printf(")\n");
    }
    static inline void print(char const * s, vec3 const & v)
    {
        assert(s);
        printf("%s, (% .3f", s, v[0]);
        for (int i = 1; i < 3; ++i)
            printf(", % .3f", v[i]);
        printf(")\n");
    }
    static inline void print(char const * s, vec2 const & v)
    {
        assert(s);
        printf("%s, (% .3f", s, v[0]);
        for (int i = 1; i < 2; ++i)
            printf(", % .3f", v[i]);
        printf(")\n");
    }
    void shaderRenderEvent(ShaderRenderEvent const * event)
    {
        assert(event);
        assert(event->program);
        // draw_sphere(radius, _lookat, event->program);
    }


private:
    bool press, drag, release;
    float xi, yi, xf, yf;
    float wx, wy;
    vec3 _center;
    vec3 _lookat;
    vec3 _window_center;
    float radius;
    quat _q_now, _q_drag;
    vec3 _v_now, _v_down;
};

#endif  // __BALL_H