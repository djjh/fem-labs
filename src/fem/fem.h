#ifndef __FEM_H
#define __FEM_H

#include <cassert>
#include <iostream>

#include <Eigen/Dense>


using Eigen::Matrix;

namespace Fem
{

template <typename precision>
class RealFunction
{
public:
    virtual precision operator()(precision) = 0;
};

/* 
 * Table of interpretations of the state vector and conjugate vector by
 * application problem:
 *
 * Application                      State (DOF) vector u    Conjugate vector f
 * Problem                          interpretation          interpretation
 * -----------                      --------------------    ------------------
 * Structures and solid mechanics   Displacement             Mechanical force
 * Heat conduction                  Temperature              Heat flux
 * Acoustic fluid                   Displacement potential   Particle velocity
 * Potential flows                  Pressure                 Particle velocity
 * General flows                    Velocity                 Fluxes
 * Electrostatics                   Electric potential       Charge density
 * Magnetostatics                   Magnetic potential       Magnetic intensity
 */
template <typename precision, int nodes>
class Problem
{
public:
    Problem()
    : fun_a(NULL)
    , fun_f(NULL)
    {
        u.fill(0.0);
        x.fill(0.0);
        A.fill(0.0);
        b.fill(0.0);
        k[0] = 0;
        k[1] = 0;
        g[0] = 0;
        g[1] = 0;
    }
    inline bool is_valid()
    {
        return (fun_a != NULL) && (fun_f != NULL);
    }
    inline precision a(precision x)
    {
        assert(is_valid());
        return (*fun_a)(x);
    }
    inline precision f(precision x)
    {
        assert(is_valid());
        return (*fun_f)(x);
    }

    Matrix<precision, nodes, 1> u;      // state vector
    Matrix<precision, nodes, 1> x;      // node coordinates
    Matrix<precision, nodes, nodes> A;  // stiffness matrix
    Matrix<precision, nodes, 1> b;      // load vector
    RealFunction<precision> * fun_a;    // constitutive relation
    RealFunction<precision> * fun_f;    // forcing function
    precision k[2];                     // robin bc: pseudo dirichlet to neumann ratio 
    precision g[2];                     // robin bc: pseudo dirichlet or neumann bc

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW  // required for fixed sized Eigen member,
                                     // see Eigen docs for details
};


template <typename precision, int nodes>
void assemble_stiffness_matrix(Problem<precision, nodes> & p)
{
    assert(p.is_valid());

    p.A.fill(0.0);

    /* Your code here */

}

template <typename precision, int nodes>
void assemble_load_vector(Problem<precision, nodes> & p)
{
    assert(p.is_valid());

    p.b.fill(0.0);

    /* Your code here */

}

template <typename precision, int nodes>
void solve(Problem<precision, nodes> & p)
{
    assert(p.is_valid());

    /*
     * Given a problem p as defined in this file,
     * this function should:
     * - assemble the stiffness matrix, p.A
     * - assemble the load vector, p.b
     * - solve for the state vector, p.u
     *
     * Tips:
     * - remember we are given the number of nodes
     *   rather than number of elements, so watch
     *   out for off-by-one errors
     * - we are using Eigen for linear algebra; don't
     *   forget that Eigen has multiple solvers and
     *   that their usage depends on the problem
     * - the background grid is made of unit squares,
     *   so you can use that to get a sense of the
     *   scale of the resulting state vector.
     */

    /* Your code here */

}

}  // namespace fem

#endif  // __FEM_H