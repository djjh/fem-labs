#ifndef __AIRFOIL_H
#define __AIRFOIL_H

#include <cassert>

#include "element.h"
#include "window_events.h"
#include "adhoc_events.h"
#include "draw.h"
#include "mesh.h"
#include "fem2D.h"

typedef VertexColorShaderProgram::Vertex Vertex;
typedef VertexColorShaderProgram::GpuAsset GpuAsset;

template <typename precision>
class AFunction : public Fem::Function<Matrix<precision, 2, 1>, precision>
{
public:
    virtual void operator()(precision & out, Matrix<precision, 2, 1> const & in)
    {
        out = 1.f;
    }
};

template <typename precision>
class Kappa1Function : public Fem::Function<Matrix<precision, 2, 1>, precision>
{
public:
    virtual void operator()(precision & out, Matrix<precision, 2, 1> const & in)
    {
        out = (in[0] > 29.99f) ? 1.e+6f : 0.f;
    }
};

template <typename precision>
class GD1Function : public Fem::Function<Matrix<precision, 2, 1>, precision>
{
public:
    virtual void operator()(precision & out, Matrix<precision, 2, 1> const & in)
    {
        out = 0;
    }
};

template <typename precision>
class GN1Function : public Fem::Function<Matrix<precision, 2, 1>, precision>
{
public:
    virtual void operator()(precision & out, Matrix<precision, 2, 1> const & in)
    {
        out = (in[0] < -29.99f) ? 1.f : 0.f;
    }
};

template <typename precision>
class Airfoil : public Element
{
public:
    Airfoil()
    {
        ga = NULL;

        read_in_mesh(_m, "dat/airfoil/airfoil.1"); // intented to be run from project root

        // // setup the problem
        _problem.m = &_m;
        _problem.fun_a = &_a;
        _problem.fun_kappa = &_kappa1;
        _problem.fun_gD = &_gD1;
        _problem.fun_gN = &_gN1;

        // since it's a static (non-time-varying) problem,
        // we can just solve the problem here
        Fem::solve(_problem);
    }
    virtual ~Airfoil() {}
    virtual void setEventManager(EventManager * event_manager)
    {
        Element::setEventManager(event_manager);
        _event_manager->addEventHook(this, &Airfoil::shaderRenderEvent);
        _event_manager->addEventHook(this, &Airfoil::removeElementEventHooksEventHook);
    }
    void removeElementEventHooksEventHook(RemoveElementEventHooksEvent const * event)
    {
        if (event->element == this)
        {
            _event_manager->removeEventHook(this, &Airfoil::shaderRenderEvent);
            _event_manager->removeEventHook(this, &Airfoil::removeElementEventHooksEventHook);
        }
    }
    void shaderRenderEvent(ShaderRenderEvent const * event)
    {
        assert(event);
        assert(event->program);

        if (_problem.u.rows() == 0)
            return;

        if (!ga)
        {
            float max = _problem.u(0);
            float min = max;
            for (int i = 1; i < _m.nodes.cols(); ++i)
            {
                float value = _problem.u(i);
                if (max < value)
                    max = value;
                if (min > value)
                    min = value;
            }

            float nregions = 5.f;

            for (int i = 0; i < _m.nodes.cols(); ++i)
            {
                float r, b;

                // // alternate between two colors
                // if (((int)(nregions*(_problem.u(i) - min) / (max - min))) % 2 == 0)
                // {
                //     r = 1.f;
                //     b = 0.f;
                // }
                // else
                // {
                //     r = 0.f;
                //     b = 1.f;
                // }

                // discritized gradient between two colors.
                r = ((int)(nregions*(_problem.u(i) - min) / (max - min)))/nregions;
                b = ((int)(nregions*(1.f - (_problem.u(i) - min) / (max - min))))/nregions;

                vertices.push_back(Vertex(_m.nodes(0, i),
                                          _m.nodes(1, i),
                                          0.f,
                                          1.f,
                                          r,
                                          0.f,
                                          b,
                                          1.f));
            }

            // for drawing wireframe elements
            // assumes full connectivity between nodes of an element
            // so it works for tris and tets
            for (int i = 0; i < _m.elements.cols(); ++i)
            {
                for (int j = 0;  j < _m.elements.rows() - 1; ++j)
                {
                    for (int k = j + 1; k < _m.elements.rows(); ++k)
                    {
                        indices.push_back(_m.elements(j, i));
                        indices.push_back(_m.elements(k, i));
                    }
                }
            }

            // // draw the boundary region
            // for (unsigned i = 0; i < _m.boundary.cols(); ++i)
            // {
            //     indices.push_back(_m.boundary(0, i));
            //     indices.push_back(_m.boundary(1, i));
            // }

            ga = event->program->gpu_create_asset(vertices, indices);
        }
        event->program->gpu_draw_lines(ga);
    }
private:
    // fem
    Mesh<precision> _m;
    AFunction<precision> _a;
    Kappa1Function<precision> _kappa1;
    GD1Function<precision> _gD1;
    GN1Function<precision> _gN1;
    Fem::Problem2D<precision> _problem;

    // graphics
    GpuAsset * ga;
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
};

#endif  // __AIRFOIL_H