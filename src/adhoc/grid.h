#ifndef __GRID_H
#define __GRID_H

#include <cassert>

#include "element.h"
#include "window_events.h"
#include "draw.h"

class Grid : public Element
{
public:
    Grid() {}
    virtual ~Grid() {}
    void addEventHooks(EventManager * event_manager)
    {
        assert(event_manager);
        event_manager->addEventHook(this, &Grid::renderElementsEventHook);
    }
    void removeEventHooks(EventManager * event_manager)
    {
        assert(event_manager);
        event_manager->removeEventHook(this, &Grid::renderElementsEventHook);
    }
    void renderElementsEventHook(RenderElementsEvent const * event)
    {
        assert(event);
        assert(event->program);
        draw_grid(event->program);
    }
};

#endif  // __GRID_H
