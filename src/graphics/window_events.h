#ifndef __WINDOW_EVENTS_H
#define __WINDOW_EVENTS_H

#include "events.h"

class KeyInputEvent : public Event
{
public:
    KeyInputEvent(int k = 0, int s = 0, int a = 0, int m = 0)
    : key(k)
    , scancode(s)
    , action(a)
    , mods(m)
    {};

    int key;
    int scancode;
    int action;
    int mods;
};

class MouseButtonEvent : public Event
{
public:
    MouseButtonEvent(int b = 0, int a = 0, int m = 0)
    : button(b)
    , action(a)
    , mods(m)
    {};

    int button;
    int action;
    int mods;
};

class MouseCursorEvent : public Event
{
public:
    MouseCursorEvent(double x = 0.0, double y = 0.0)
    : xpos(x)
    , ypos(y)
    {};

    double xpos;
    double ypos;
};

class WindowSizeEvent : public Event
{
public:
    WindowSizeEvent(int w = 0, int h = 0)
    : width(w)
    , height(h)
    {};

    int width;
    int height;
};

class FramebufferSizeEvent : public Event
{
public:
    FramebufferSizeEvent(int w = 0, int h = 0)
    : width(w)
    , height(h)
    {};

    int width;
    int height;
};

class WindowRefreshEvent : public Event
{
public:
    WindowRefreshEvent() {}
};

class WindowRenderEvent : public Event
{
public:
    WindowRenderEvent() {};
};

#endif  // __WINDOW_EVENTS_H