#ifndef __GUI_H
#define __GUI_H

#include <iostream>
#include <set>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include <linmath.h>

#include <Eigen/Dense>

#include "element.h"
#include "events.h"
#include "draw.h"
#include "window.h"
#include "window_events.h"
#include "shader.h"
#include "pan.h"
#include "zoom.h"

/*
 * Can only be used from the main thread.
 */
class Gui
{
public:
    Gui();
    ~Gui();
    bool initialize();
    bool add_element(Element * element);
    bool remove_element(Element * element);

    bool step(double dt = 1.0/60.0);
    
    double get_time();
    bool should_close();

private:
    bool render();

    // event hooks
    void keyInputEventHook(KeyInputEvent const * event);
    void mouseButtonEventHook(MouseButtonEvent const * event);
    void mouseCursorEventHook(MouseCursorEvent const * event);
    void windowSizeEventHook(WindowSizeEvent const * event);
    void framebufferSizeEventHook(FramebufferSizeEvent const * event);
    void windowRefreshEventHook(WindowRefreshEvent const * event);

    Window _window;
    Pan * _pan;
    VertexColorShaderProgram * _shaderprogram;
    Zoom * _zoom;
    std::set<Element *> _elements;
};


#endif  // __GUI_H
