#ifndef __HEAT_H
#define __HEAT_H

#include <cassert>

#include "fem.h"
#include "element.h"
#include "shader.h"
#include "draw.h"

template <typename precision>
class ConductivityFunction : public Fem::RealFunction<precision>
{
public:
    virtual precision operator()(precision x)
    {
        return 0.1 * (5. - 0.6 * x);
    }
};

template <typename precision>
class SourceFunction : public Fem::RealFunction<precision>
{
public:
    virtual precision operator()(precision x)
    {
        return 0.03 * pow(x - 6, 4);
    }
};

template<typename precision, int nodes>
class HeatProblem : public Element
{
public:
    HeatProblem()
    {
        // setup the problem
        int const start = 2;
        int const end = 8;
        precision const spacing = (precision) (end - start) / (nodes - 1);
        for (int i = 0; i < nodes; ++i)
            _problem.x(i) = start + i * spacing;
        _problem.fun_a = &_conductivity;
        _problem.fun_f = &_source;
        _problem.k[0] = 1.0e+6f;
        _problem.k[1] = 0.f;
        _problem.g[0] = -1.f;
        _problem.g[1] = 0.f;

        // since it's a static (non-time-varying) problem,
        // we can just solve the problem here
        Fem::solve(_problem);
    }
    virtual ~HeatProblem() {}
    void addEventHooks(EventManager * event_manager)
    {
        assert(event_manager);
        event_manager->addEventHook(this, &HeatProblem::renderElementsEventHook);
    }
    void removeEventHooks(EventManager * event_manager)
    {
        assert(event_manager);
        event_manager->removeEventHook(this, &HeatProblem::renderElementsEventHook);
    }
    void renderElementsEventHook(RenderElementsEvent const * event)
    {
        assert(event);
        assert(event->program);
        draw_mesh_1D<precision, nodes>(_problem.x, _problem.u, event->program);
    }

private:
    ConductivityFunction<precision> _conductivity;
    SourceFunction<precision> _source;
    Fem::Problem<precision, nodes> _problem;
};

#endif  // __HEAT_H