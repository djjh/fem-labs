#ifndef __GRID_H
#define __GRID_H

#include <cassert>

#include "element.h"
#include "window_events.h"
#include "adhoc_events.h"
#include "draw.h"

class Grid : public Element
{
public:
    Grid() {}
    virtual ~Grid() {}
    virtual void setEventManager(EventManager * event_manager)
    {
        Element::setEventManager(event_manager);
        _event_manager->addEventHook(this, &Grid::shaderRenderEvent);
        _event_manager->addEventHook(this, &Grid::removeElementEventHooksEventHook);
    }
    void removeElementEventHooksEventHook(RemoveElementEventHooksEvent const * event)
    {
        if (event->element == this)
        {
            _event_manager->removeEventHook(this, &Grid::shaderRenderEvent);
            _event_manager->removeEventHook(this, &Grid::removeElementEventHooksEventHook);
        }
    }
    void shaderRenderEvent(ShaderRenderEvent const * event)
    {
        assert(event);
        assert(event->program);
        draw_grid(event->program);
    }
};

#endif  // __GRID_H
