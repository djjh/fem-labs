// #ifndef __HEAT_H
// #define __HEAT_H

#include <cassert>
#include <iomanip>

#include "fem.h"
#include "element.h"
#include "adhoc_events.h"
#include "draw.h"

static unsigned int const NODES = 100;
static unsigned int const START = 2;
static unsigned int const END = 8;

template<typename precision>
class Heat : public Element
{
public:
    Heat()
    : ga_curve(NULL)
    , ga_heights(NULL)
    {
        generate_mesh();
        solve();
    }
    virtual ~Heat() {}
    virtual void setEventManager(EventManager * event_manager)
    {
        Element::setEventManager(event_manager);

        _event_manager->addEventHook(this, &Heat::shaderRenderEvent);
        _event_manager->addEventHook(this, &Heat::removeElementEventHooksEventHook);
    }
    void removeElementEventHooksEventHook(RemoveElementEventHooksEvent const * event)
    {
        if (event->element == this)
        {
            _event_manager->removeEventHook(this, &Heat::shaderRenderEvent);
            _event_manager->removeEventHook(this, &Heat::removeElementEventHooksEventHook);
        }
    }
    void shaderRenderEvent(ShaderRenderEvent const * event)
    {
        assert(event);
        assert(event->program);

        if (!ga_curve || !ga_heights)
        {
            precision max = _u(0);
            precision min = _u(0);
            for (unsigned i = 0; i < NODES; ++i)
            {
                if (max < _u(i))
                    max = _u(i);
                if (min > _u(i))
                    min = _u(i);
            }

            std::vector<Vertex> curve_vertices;
            curve_vertices.reserve(NODES);
            for (unsigned i = 0; i < NODES; ++i)
            {
                precision ui = (_u(i) - min) / (max - min);
                curve_vertices.push_back(Vertex(_m.nodes(i), _u(i), 0.f, 1.f, ui, 0.f, 1.f - ui, 1.f));
            }

            std::vector<Vertex> height_vertices;
            height_vertices.reserve(2*NODES);
            for (unsigned i = 0; i < NODES; ++i)
            {
                precision ui = (_u(i) - min) / (max - min);
                height_vertices.push_back(Vertex(_m.nodes(i), 0.f, 0.f, 1.f, ui, 0.f, 1.f - ui, 1.f));
                height_vertices.push_back(Vertex(_m.nodes(i), _u(i), 0.f, 1.f, ui, 0.f, 1.f - ui, 1.f));
            }

            ga_curve = event->program->gpu_create_asset(curve_vertices);
            ga_heights = event->program->gpu_create_asset(height_vertices);
        }

        event->program->gpu_draw_line_strip(ga_curve);
        event->program->gpu_draw_lines(ga_heights);
    }

private:
    void generate_mesh()
    {
        _m.nodes.resize(1, NODES);
        _m.elements.resize(2, NODES - 1);
        _m.boundary.resize(1, 2);

        // setup the problem
        _m.boundary(0) = 0;
        _m.boundary(1) = NODES - 1;

        precision const spacing = (precision) (END - START) / (_m.elements.cols());

        for (int i = 0; i < _m.nodes.cols(); ++i)
        {
            _m.nodes(i) = START + i * spacing;
        }

        for (int i = 0; i < _m.elements.cols(); ++i)
        {
            _m.elements(0, i) = i;
            _m.elements(1, i) = i + 1;
        }
    }
    void solve()
    {
        int const dim = _m.nodes.rows();
        int const nodes_per_element = _m.elements.rows();
        int const number_of_nodes = _m.nodes.cols();
        int const number_of_elements = _m.elements.cols();
        int const udim = 1;

        _A.resize(number_of_nodes, number_of_nodes);
        _b.resize(number_of_nodes, udim);
        _R.resize(number_of_nodes, number_of_nodes);
        _r.resize(number_of_nodes, udim);

        // assert(p.is_valid());
        assemble_stiffness_matrix(_A, _m, _a);
        assemble_load_vector(_b, _m, _f);
        assemble_robin_mass_matrix(_R, _m, _kappa);
        assemble_robin_load_vector(_r, _m, _kappa, _gD, _gN);

        SimplicialLDLT< SparseMatrix<precision> > solver;
        solver.compute(_A + _R);
        assert(solver.info() == Eigen::Success);
        _u = solver.solve(_b + _r);

        assert(solver.info() == Eigen::Success);
    }
    
    class A : public Fem::Function<Matrix<precision, Dynamic, 1>, precision>
    {
    public:
        virtual void operator()(precision & out, Matrix<precision, Dynamic, 1> const & in) const
        {
            assert(in.rows() == 1);
            out = (precision) (0.1 * (5. - 0.6 * in(0)));
        }
    };


    class F : public Fem::Function<Matrix<precision, Dynamic, 1>, Matrix<precision, Dynamic, 1> >
    {
    public:
        virtual void operator()(Matrix<precision, Dynamic, 1> & out, Matrix<precision, Dynamic, 1> const & in) const
        {
            assert(in.rows() == 1);
            assert(in.rows() == 1);

            out(0)  = (precision) (0.03 * pow(in(0) - 6, 4));
        }
    };

    class Kappa : public Fem::Function<Matrix<precision, Dynamic, 1>, precision>
    {
    public:
        virtual void operator()(precision & out, Matrix<precision, Dynamic, 1> const & in) const
        {
            assert(in.rows() == 1);
            out = (in(0) == START) ? 1.e+6f : 0.f;
        }
    };

    class GD : public Fem::Function<Matrix<precision, Dynamic, 1>, Matrix<precision, Dynamic, 1> >
    {
    public:
        virtual void operator()(Matrix<precision, Dynamic, 1> & out, Matrix<precision, Dynamic, 1> const & in) const
        {
            assert(in.rows() == 1);
            assert(out.rows() == 1);

            out(0) = (in(0) == START) ? -1.f : 0.f;
        }
    };

    class GN : public Fem::Function<Matrix<precision, Dynamic, 1>, Matrix<precision, Dynamic, 1> >
    {
    public:
        virtual void operator()(Matrix<precision, Dynamic, 1> & out, Matrix<precision, Dynamic, 1> const & in) const
        {
            assert(in.rows() == 1);
            assert(out.rows() == 1);

            out(0) = 0;
        }
    };

    // fem
    Mesh<precision> _m;
    Matrix<precision, Dynamic, Dynamic> _u;   // solution matrix                    (nodes x solution dim)
    A _a;                                     // constitutive relation              (dim x 1) -> (1 x 1)
    SparseMatrix<precision> _A;               // stiffness matrix                   (nodes x nodes)
    F _f;                                     // forcing function                   (dim x 1) -> (1 x 1)
    Matrix<precision, Dynamic, Dynamic> _b;   // load vector                        (dim x solution dim)
    Kappa _kappa;                             // robin dirichlet parameter          (dim x 1) -> (1 x 1)
    GD _gD;                                   // robin dirichlet parameter          (dim x 1) -> (solution dim x 1)
    GN _gN;                                   // robin neumann parameter            (dim x 1) -> (solution dim x 1)
    SparseMatrix<precision> _R;               // robin mass matrix                  (nodes x nodes)
    Matrix<precision, Dynamic, Dynamic> _r;   // robin load vector                  (nodes x solution dim)


    // graphics
    GpuAsset * ga_curve;
    GpuAsset * ga_heights;

};

// #endif  // __HEAT_H