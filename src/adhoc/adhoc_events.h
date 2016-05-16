#ifndef __ADHOC_EVENTS_H
#define __ADHOC_EVENTS_H

#include "window_events.h"
#include "linmath.h"

class BallEvent : public Event
{
public:
    BallEvent(quat * q = NULL, vec3 * t1 = NULL, vec3 * t2 = NULL)
    : quaternion(q), translation1(t1), translation2(t2) {}
    virtual ~BallEvent() {}
    quat * quaternion;
    vec3 * translation1;
    vec3 * translation2;
};

class Camera2DEvent : public Event
{
public:
    Camera2DEvent(mat4x4 * p = NULL, mat4x4 * v = NULL)
    : perspective(p)
    , view(v)
    {}
    mat4x4 * perspective;
    mat4x4 * view;
};

class Camera3DEvent : public Event
{
public:
    Camera3DEvent(mat4x4 * p = NULL, mat4x4 * v = NULL)
    : perspective(p)
    , view(v)
    {}
    mat4x4 * perspective;
    mat4x4 * view;
};

class DragEvent : public Event
{
public:
    DragEvent(int k = 0, float x = 0.0, float y = 0.0) : key(k), x_drag(x), y_drag(y) {}
    int key;
    float x_drag, y_drag;
};

class Pan2DEvent : public Event
{
public:
    Pan2DEvent(float xt = 0.0, float yt = 0.0) : x_translate(xt), y_translate(yt) {}
    float x_translate, y_translate;
};

class Zoom2DEvent : public Event
{
public:
    Zoom2DEvent(float xs = 0.0, float ys = 0.0) : x_scale(xs), y_scale(ys) {}
    float x_scale, y_scale;
};


#endif  // __ADHOC_EVENTS_H