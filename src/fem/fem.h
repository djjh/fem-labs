#ifndef __FEM_H
#define __FEM_H

#include <cassert>
#include <iostream>

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "mesh.h"


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

namespace Fem
{

///////////////////////////////////////////////////////////////////////////////
// REUSABLE MATH
///////////////////////////////////////////////////////////////////////////////

template <typename domain, typename codomain>
class Function
{
public:
    virtual void operator()(codomain & out, domain const & in) const = 0;
};

int factorial(int n)
{
    assert(n >= 0);

    // memoization
    static std::map<int, int> cache;
    if (cache.find(n) == cache.end())
        cache[n] = (n == 0) ? 1 : factorial(n-1) * n;

    return cache[n];
}

// an n-dimensional "ploygon" is actually called a polytope
// a k-simplex is a k-dimensional polytope with k+1 vertices
//
// simplex given may be embedded in a higher level space....
// interpretted based on # rows / cols
template <typename precision>
precision simplex_measure(Matrix<precision, Dynamic, Dynamic> & x)
{
    // x: (number of vertices x dim)
    int const nvertices = x.rows();
    int const dim = x.cols();
    assert(nvertices - 1 <= dim);

    // compute det(x1 - x0, x2 - x0, ..., xn - x0)
    Matrix<precision, Dynamic, Dynamic> m(nvertices - 1, dim);
    for (int i = 0; i < nvertices - 1; ++i)
    {
        m.block(i, 0, 1, dim) = x.block(i + 1, 0, 1, dim) - x.block(0, 0, 1, dim);
    }

    if (nvertices - 1 == dim)
    {
        return (precision) fabs(m.determinant()) / factorial(dim);
    }

    return (precision) sqrt((m * m.transpose()).determinant()) / factorial(nvertices - 1);
}

template <typename precision>
void mean(Matrix<precision, Dynamic, 1> & mv,
          Matrix<precision, Dynamic, Dynamic> const & x)
{
    mv.resize(x.cols());
    for (int i = 0; i < x.cols(); ++i)
    {
        mv(i) = 0.0;
        for (int j = 0; j < x.rows(); ++j)
        {
            mv(i) += x(j, i);
        }
        mv(i) /= x.rows();
    }
}

///////////////////////////////////////////////////////////////////////////////
// FEM -- SIMPLEX ELEMENTS
///////////////////////////////////////////////////////////////////////////////


template <typename precision>
void hat_gradients(Matrix<precision, Dynamic, Dynamic> & a,
                   Matrix<precision, Dynamic, Dynamic> & x)
{
    x.conservativeResize(x.rows(), x.cols() + 1);
    x.block(0, x.cols() - 1, x.rows(), 1).fill(1.0);

    a = x.inverse().transpose();

    a.conservativeResize(a.rows(), a.cols() - 1);
    x.conservativeResize(x.rows(), x.cols() - 1);
}


// to be combined (multipled) with the simplex area to get part of an element of the mass matrix...
template <typename precision>
precision mass_matrix_factor(int nodes_per_boundary_segment, int dim)
{
    assert(dim >= 0);
    return (precision) factorial(factorial(dim)) / factorial(nodes_per_boundary_segment + factorial(dim));
}

template <typename precision>
void assemble_stiffness_matrix(SparseMatrix<precision> & A,
                               Mesh<precision> const & m,
                               Function<Matrix<precision, Dynamic, 1>, precision> const & afun)
{
    // size hints
    int const dim = m.nodes.rows();
    int const nodes_per_element = m.elements.rows();
    int const number_of_nodes = m.nodes.cols();
    int const number_of_elements = m.elements.cols();

    // sparse matrices still need to be resized for dimension info
    A.resize(number_of_nodes, number_of_nodes);

    for (int i = 0; i < number_of_elements; ++i)
    {
        // Store a local to global node index mapping for this element.
        // hints:
        //   - you want to be able to get a node's global index given
        //     it's local index
        //   - element indices are stored in m.elements

        /* Your code here. */


        // 
        // Store the geometry for this element in a matrix.
        // hints:
        //   - this is needed as input to hat_gradients, mean, and
        //     simplex_measure, so you can look in there for more
        //     clues if needed
        //   - the matrix should be size (nodes_per_element x dim)
        // 

        /* Your code here. */


        // Compute the gradients of this nodes' shape functions.
        // hints:
        //   - use the hat_gradients function

        /* Your code here. */


        // Compute the center of gravity of this element.
        // hints:
        //   - use this element's geometry, and the mean function

        /* Your code here. */


        // Compute the value of the afun for this element.
        // hints:
        //   - think about where it should be evaluated

        /* Your code here */


        // Compute the measure of this element, for intuition, this
        // corresponds to length, area, and volume for 1D, 2D, and
        // 3D elements, respectively.
        // hints:
        //   - use the simplex_measure function

        /* Your code here. */


        // Assemble the local stiffness matrix.
        // hint:
        //   - this can be done in one line.
        //   - use the value of afun, gradients of the shape functions,
        //     and the measure

        /* Your code here. */


        // Update the global stiffness matrix.
        // hint:
        //   - use the local to global mapping, global stiffness matrix,
        //     and local stiffness matrix
        //   - elements of sparse matrices are accessed using the coeffRef
        //     member function

        /* Your code here. */

    }
}

template <typename precision>
void assemble_load_vector(Matrix<precision, Dynamic, Dynamic> & b,
                          Mesh<precision> const & m,
                          Function< Matrix<precision, Dynamic, 1>, Matrix<precision, Dynamic, 1> > const & ffun)
{
    // size hints
    int const dim = m.nodes.rows();
    int const nodes_per_element = m.elements.rows();
    int const number_of_nodes = m.nodes.cols();
    int const number_of_elements = m.elements.cols();
    int const udim = b.cols();

    assert(b.cols() > 0);
    assert(b.rows() == number_of_nodes);
    b.fill(0.0);

    for (int i = 0; i < number_of_elements; ++i)
    {
        // Store a local to global node index mapping for this element.
        // hints:
        //   - you want to be able to get a node's global index given
        //     it's local index
        //   - element indices are stored in m.elements

        /* Your code here. */


        // 
        // Store the geometry for this element in a matrix.
        // hints:
        //   - this is needed as input to hat_gradients, mean, and
        //     simplex_measure, so you can look in there for more
        //     clues if needed
        //   - the matrix should be size (nodes_per_element x dim)
        // 

        /* Your code here. */


        // Compute the measure of this element, for intuition, this
        // corresponds to length, area, and volume for 1D, 2D, and
        // 3D elements, respectively.
        // hints:
        //   - use the simplex_measure function

        /* Your code here. */


        // Update the global load vector.
        // hint:
        //   - use the local to global mapping, global load vector,
        //     and local stiffness matrix

        /* Your code here. */

    }
}

///////////////////////////////////////////////////////////////////////////////
// FEM -- SIMPLEX ELEMENTS -- ROBIN BOUNDARY CONDITIONS
///////////////////////////////////////////////////////////////////////////////

template <typename precision>
void assemble_robin_mass_matrix(SparseMatrix<precision> & R,
                                Mesh<precision> const & m,
                                Function<Matrix<precision, Dynamic, 1>, precision> const & kappafun)
{    
    // size hints
    int const dim = m.nodes.rows();
    int const nodes_per_boundary_segment = m.boundary.rows();
    int const number_of_nodes = m.nodes.cols();
    int const boundary_segments = m.boundary.cols();

    // sparse matrices still need to be resized for dimension info
    R.resize(number_of_nodes, number_of_nodes);

    for (int i = 0; i < boundary_segments; ++i)
    {
        // Store a local to global node index mapping for this boundary segment.
        // hints:
        //   - you want to be able to get a node's global index given
        //     it's local index
        //   - boundary segment indices are stored in m.boundary

        /* Your code here. */


        // 
        // Store the geometry for this boundary segement in a matrix.
        // hints:
        //   - this is needed as input to mean, and simplex_measure,
        //     so you can look in there for more clues if needed
        //   - the matrix should be size (nodes_per_boundary_segment x dim)
        // 

        /* Your code here. */


        // Compute the center of gravity of this boundary segment.
        // hints:
        //   - use this boundary segment's geometry, and the mean function

        /* Your code here. */


        // Compute the value of the kappafun for this boundary segment.
        // hints:
        //   - think about where it should be evaluated

        /* Your code here */


        // Compute the measure of this boundary segment, for intuition,
        // this corresponds to length, area, and volume for 1D, 2D, and
        // 3D elements, respectively.
        // hints:
        //   - use the simplex_measure function

        /* Your code here. */


        // Compute the mass matrix factor for this boundary segment.
        // hints:
        //   - It's the same for all of the boundary segments.

        /* Your code here. */


        // Assemble the local mass matrix.
        // hints:
        //   - use the mass matrix factor, the value of kappafun, and measure
        //   - since you have to extrapolate from the book a bit,
        //     here's a bigger hint: Mi = something * (I + Ones)

        /* Your code here. */


        // Update the global mass matrix.
        // hint:
        //   - use the local to global mapping, global mass matrix,
        //     and local mass matrix
        //   - elements of sparse matrices are accessed using the coeffRef
        //     member function

        /* Your code here. */

    }
}

template <typename precision>
void assemble_robin_load_vector(Matrix<precision, Dynamic, Dynamic> & r,
                                Mesh<precision> const & m,
                                Function<Matrix<precision, Dynamic, 1>, precision> const & kappafun,
                                Function<Matrix<precision, Dynamic, 1>, Matrix<precision, Dynamic, 1> > const & gDfun,
                                Function<Matrix<precision, Dynamic, 1>, Matrix<precision, Dynamic, 1> > const & gNfun)
{    
    // size hints
    int const dim = m.nodes.rows();
    int const nodes_per_boundary_segment = m.boundary.rows();
    int const number_of_nodes = m.nodes.cols();
    int const boundary_segments = m.boundary.cols();
    int const udim = r.cols();

    assert(r.cols() > 0);
    assert(r.rows() == number_of_nodes);
    r.fill(0.0);

    for (int i = 0; i < boundary_segments; ++i)
    {
        // Store a local to global node index mapping for this boundary segment.
        // hints:
        //   - you want to be able to get a node's global index given
        //     it's local index
        //   - boundary segment indices are stored in m.boundary

        /* Your code here. */


        // 
        // Store the geometry for this boundary segement in a matrix.
        // hints:
        //   - this is needed as input to mean, and simplex_measure,
        //     so you can look in there for more clues if needed
        //   - the matrix should be size (nodes_per_boundary_segment x dim)
        // 

        /* Your code here. */


        // Compute the center of gravity of this boundary segment.
        // hints:
        //   - use this boundary segment's geometry, and the mean function

        /* Your code here. */


        // Compute the values of kappafun, gDfun, and gNfun for this boundary
        // segment.
        // hints:
        //   - think about where it should be evaluated
        //   - think about what they should output

        /* Your code here */


        // Compute the measure of this boundary segment, for intuition,
        // this corresponds to length, area, and volume for 1D, 2D, and
        // 3D elements, respectively.
        // hints:
        //   - use the simplex_measure function

        /* Your code here. */


        // Assemble the local robin load vector.
        // hints:
        //   - use measure, nodes_per_boundary_segment, and the values of kappafun, gDfun,
        //     and gNfun
        //   - the size should be (dim x udim)

        // Update the global robin load vector.
        // hint:
        //   - use the local to global mapping, global robin load vector,
        //     and local robin load vector

        /* Your code here. */

    }
}

}  // namespace fem

#endif  // __FEM_H