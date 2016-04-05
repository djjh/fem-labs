#ifndef __ELEMENT_H
#define __ELEMENT_H

#include "events.h"
#include "shader.h"

class RenderElementsEvent : public Event
{
public:
    public:
    RenderElementsEvent(VertexColorShaderProgram * p = NULL) : program(p) {};
    VertexColorShaderProgram * program;
};

class Element
{
public:
    virtual ~Element() {};
    virtual void addEventHooks(EventManager * event_manager) = 0;
    virtual void removeEventHooks(EventManager * event_manager) = 0;
};

#endif  // __ELEMENT_H