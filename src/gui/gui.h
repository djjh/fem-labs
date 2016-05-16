#ifndef __GUI_H
#define __GUI_H

#include <iostream>
#include <set>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include <linmath.h>

#include <Eigen/Dense>

#include "events.h"
#include "element.h"
#include "window.h"

/*
 * Can only be used from the main thread.
 */
class Gui
{
public:
    Gui();
    ~Gui();
    bool initialize();
    bool should_close();
    bool add_element(Element * element);
    bool remove_element(Element * element);
    bool step(double dt = 1.0/60.0);

private:
    double get_time();
    bool render();

    // event hooks
    void keyInputEventHook(KeyInputEvent const * event);
    void framebufferSizeEventHook(FramebufferSizeEvent const * event);
    void windowRefreshEventHook(WindowRefreshEvent const * event);

    Window _window;
    std::set<Element *> _elements;
};


#endif  // __GUI_H
