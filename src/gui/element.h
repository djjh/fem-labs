#ifndef __ELEMENT_H
#define __ELEMENT_H

#include "events.h"

class Element;
class VertexColorShaderProgram;

class SelectElementEvent : public Event
{
public:
    // TODO: this may use color selection....
    SelectElementEvent(Element * e = NULL) : element(e) {}
    Element * element;
};

class RenderElementsEvent : public Event
{
public:
    RenderElementsEvent(VertexColorShaderProgram * p = NULL) : program(p) {};
    VertexColorShaderProgram * program;
};

class RemoveElementEventHooksEvent : public Event
{
public:
    RemoveElementEventHooksEvent(Element * e = NULL) : element(e) {};
    Element * element;
};

class Element
{
public:
    virtual ~Element() {};
    virtual void setEventManager(EventManager * event_manager) // change to add*** for multiple
    {                                                          // event managers, useful e.g. multi window apps
        assert(event_manager);
        _event_manager = event_manager;
    }
protected:
    EventManager * _event_manager;
};

#endif  // __ELEMENT_H