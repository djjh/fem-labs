#ifndef __HOSE_H
#define __HOSE_H

#include <cassert>
#include <ctime>

#include "element.h"
#include "window_events.h"
#include "adhoc_events.h"
#include "draw.h"
#include "mesh.h"
#include "fem.h"

using Eigen::Matrix;
using Eigen::SparseMatrix;
using Eigen::Dynamic;

// sparse solvers at our disposal
using Eigen::SimplicialLLT;      // semi-positive definite
using Eigen::SimplicialLDLT;     // semi-positive definite
using Eigen::ConjugateGradient;  // semi-positive definite
using Eigen::BiCGSTAB;           // square
using Eigen::SparseLU;           // square
using Eigen::SparseQR;           // any

typedef VertexColorShaderProgram::Vertex Vertex;
typedef VertexColorShaderProgram::GpuAsset GpuAsset;

template <typename precision>
class Hose : public Element
{
public:
    Hose()
    : ga(NULL)
    {
        read_in_mesh(_m, "dat/hose/hose.1");
        solve();
    }
    virtual ~Hose() {}
    virtual void setEventManager(EventManager * event_manager)
    {
        Element::setEventManager(event_manager);
        _event_manager->addEventHook(this, &Hose::shaderRenderEvent);
        _event_manager->addEventHook(this, &Hose::removeElementEventHooksEventHook);
    }
    void removeElementEventHooksEventHook(RemoveElementEventHooksEvent const * event)
    {
        if (event->element == this)
        {
            _event_manager->removeEventHook(this, &Hose::shaderRenderEvent);
            _event_manager->removeEventHook(this, &Hose::removeElementEventHooksEventHook);
        }
    }
    void shaderRenderEvent(ShaderRenderEvent const * event)
    {
        assert(event);
        assert(event->program);

        if (!ga)
        {
            float max = _u(0);
            float min = max;
            for (int i = 1; i < _m.nodes.cols(); ++i)
            {
                float value = _u(i);
                if (max < value)
                    max = value;
                if (min > value)
                    min = value;
            }

            for (int i = 0; i < _m.nodes.cols(); ++i)
            {
                float r, b;

                r = (_u(i) - min) / (max - min);
                b = 1.f - r;

                vertices.push_back(Vertex(0.2*_m.nodes(0, i),
                                          0.2*_m.nodes(1, i),
                                          0.2*_m.nodes(2, i),
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
    void solve()
    {
        int const dim = _m.nodes.rows();
        int const nodes_per_element = _m.elements.rows();
        int const number_of_nodes = _m.nodes.cols();
        int const number_of_elements = _m.elements.cols();
        int const udim = 1;

        _A.resize(number_of_nodes, number_of_nodes);
        _R.resize(number_of_nodes, number_of_nodes);
        _r.resize(number_of_nodes, udim);

        assemble_stiffness_matrix(_A, _m, _a);
        assemble_robin_mass_matrix(_R, _m, _kappa);
        assemble_robin_load_vector(_r, _m, _kappa, _gD, _gN);
        SimplicialLDLT< SparseMatrix<precision> > solver;
        solver.compute(_A + _R);
        assert(solver.info() == Eigen::Success);
        _u = solver.solve(_r);
        assert(solver.info() == Eigen::Success);
    }

    class GN : public Fem::Function<Matrix<precision, Dynamic, 1>, Matrix<precision, Dynamic, 1> >
    {
    public:
        virtual void operator()(Matrix<precision, Dynamic, 1> & out, Matrix<precision, Dynamic, 1> const & in) const
        {
            assert(in.rows() == 3);
            assert(out.rows() == 1);
            out(0) = (sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]) > 30) ? -1.f : 0.f;
            // out(0) = (sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]) < 50.0 || in[2] < 40.0) ? -1.f : 0.f;
            // out(0) = (sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]) > 30 && sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]) < 100) ? -1.f : 0.f;
        }
    };

    class A : public Fem::Function<Matrix<precision, Dynamic, 1>, precision>
    {
    public:
        virtual void operator()(precision & out, Matrix<precision, Dynamic, 1> const & in) const
        {
            assert(in.rows() == 3);
            out = 1.f;
        }
    };

    class Kappa : public Fem::Function<Matrix<precision, Dynamic, 1>, precision>
    {
    public:
        virtual void operator()(precision & out, Matrix<precision, Dynamic, 1> const & in) const
        {
            assert(in.rows() == 3);
            out = (sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]) < 30) ? 1.e+6f : 0.f;
            // out = (sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]) > 50.0 && in[2] > 40.0) ? 1.e+6f : 0.f;
            // out = (sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]) < 30 || sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]) > 100) ? 1.e+6f : 0.f;
        }
    };

    class GD : public Fem::Function<Matrix<precision, Dynamic, 1>, Matrix<precision, Dynamic, 1> >
    {
    public:
        virtual void operator()(Matrix<precision, Dynamic, 1> & out, Matrix<precision, Dynamic, 1> const & in) const
        {
            assert(in.rows() == 3);
            assert(out.rows() == 1);
            out(0) = 0;
        }
    };

    // fem
    Mesh<precision> _m;
    Matrix<precision, Dynamic, Dynamic> _u;   // solution matrix                    (nodes x solution dim)
    A _a;                                     // constitutive relation              (dim x 1) -> (1 x 1)
    SparseMatrix<precision> _A;               // stiffness matrix                   (nodes x nodes)
    Kappa _kappa;                             // robin dirichlet parameter          (dim x 1) -> (1 x 1)
    GD _gD;                                   // robin dirichlet parameter          (dim x 1) -> (solution dim x 1)
    GN _gN;                                   // robin neumann parameter            (dim x 1) -> (solution dim x 1)
    SparseMatrix<precision> _R;               // robin mass matrix                  (nodes x nodes)
    Matrix<precision, Dynamic, Dynamic> _r;   // robin load vector                  (nodes x solution dim)


    // graphics
    GpuAsset * ga;
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
};

#endif  // __HOSE_H