#ifndef __FEM_2D_H
#define __FEM_2D_H

#include <cassert>
#include <iostream>

#include <Eigen/Dense>


using Eigen::Matrix;
using Eigen::SparseMatrix;

// sparse solvers at our disposal
using Eigen::SimplicialLLT;      // semi-positive definite
using Eigen::SimplicialLDLT;     // semi-positive definite
using Eigen::ConjugateGradient;  // semi-positive definite
using Eigen::BiCGSTAB;           // square
using Eigen::SparseLU;           // square
using Eigen::SparseQR;           // any

namespace Fem
{

template <typename domain, typename codomain>
class Function
{
public:
    virtual void operator()(codomain & out, domain const & in) = 0;
};

template <typename precision>
class Problem2D
{
public:
    Problem2D()
    : fun_a(NULL)
    , fun_kappa(NULL)
    , fun_gD(NULL)
    , fun_gN(NULL)
    {
    }
    bool is_valid()
    {
        return (m != NULL) && 
               (fun_a != NULL) &&
               (fun_kappa != NULL) &&
               (fun_gD != NULL) &&
               (fun_gN != NULL);
    }
    precision a(Matrix<precision, 2, 1> const & x)
    {
        assert(is_valid());
        precision out;
        (*fun_a)(out, x);
        return out;
    }
    precision kappa(Matrix<precision, 2, 1> const & x)
    {
        assert(is_valid());
        precision out;
        (*fun_kappa)(out, x);
        return out;
    }
    precision gD(Matrix<precision, 2, 1> const & x)
    {
        assert(is_valid());
        precision out;
        (*fun_gD)(out, x);
        return out;
    }
    precision gN(Matrix<precision, 2, 1> const & x)
    {
        assert(is_valid());
        precision out;
        (*fun_gN)(out, x);
        return out;
    }

    // Problem Input (fill in by user...) :
    Mesh<precision> * m;                                         // mesh:
                                                                 // - point matrix (P in ch 4)        (node dims x nodes)
                                                                 // - connectivity matrix (T in ch 4) (element dim x elements)
    Function<Matrix<precision, 2, 1>, precision> * fun_a;        // constitutive relation
    //Function<Matrix<precision, 2, 1>, precision> * fun_f;      // forcing function is missing simply because it is not used
                                                                 //    in the airfoil problem (in other words f(x, y) = 0)
    Function<Matrix<precision, 2, 1>, precision> * fun_kappa;    // robin bc: pseudo dirichlet to neumann ratio (kappa in ch 4)
    Function<Matrix<precision, 2, 1>, precision> * fun_gD;       // robin bc: pseudo dirichlet bc
    Function<Matrix<precision, 2, 1>, precision> * fun_gN;       // robin bc: pseudo neumann bc

    // Problem output (filled in by solver...) :
    Matrix<precision, Dynamic, 1> u;    // state vector (phi in ch 4)        (nodes x 1)
    SparseMatrix<precision> A;          // stiffness matrix                  (nodes x nodes)
    SparseMatrix<precision> R;          // robin mass matrix                 (nodes x nodes)
    Matrix<precision, Dynamic, 1> r;    // robin load vector                 (nodes x 1)
};

template <typename precision>
precision polyarea(Matrix<precision, Dynamic, 1> const & x,
                   Matrix<precision, Dynamic, 1> const & y)
{
    assert(x.rows() == y.rows());

    precision area = 0;
    for (int i = 0, j = x.rows() - 1; i < x.rows(); ++i)
    {
        area += (x(j) + x(i)) * (y(j) - y(i));
        j = i;
    }

    return (precision) 0.5 * fabs(area);
}

template <typename precision>
void hat_gradients(Matrix<precision, Dynamic, 1> & b,
                   Matrix<precision, Dynamic, 1> & c,
                   Matrix<precision, Dynamic, 1> const & x,
                   Matrix<precision, Dynamic, 1> const & y,
                   precision const & area)
{
    // Only will work for triangluar elements in the plane,
    // we are using dynamically sized vectors for convenience only.
    assert(x.rows() == 3);
    assert(x.rows() == y.rows());

    b.resize(y.rows());
    b(0) = y(1) - y(2);
    b(1) = y(2) - y(0);
    b(2) = y(0) - y(1);
    b *= (precision) (1.0 / 2.0 / area);

    c.resize(x.rows());
    c(0) = x(2) - x(1);
    c(1) = x(0) - x(2);
    c(2) = x(1) - x(0);
    c *= (precision) (1.0 / 2.0 / area);
}

template <typename precision>
void mean(Matrix<precision, 2, 1> & mv,
          Matrix<precision, Dynamic, 1> const & x,
          Matrix<precision, Dynamic, 1> const & y)
{
    assert(x.rows() == y.rows());

    precision sum = (precision) 0.0;
    for (int i = 0; i < x.rows(); ++i)
    {
        sum += x(i);
    }
    mv(0) = sum / (precision) 2.0;

    sum = 0.0;
    for (int i = 0; i < y.rows(); ++i)
    {
        sum += y(i);
    }
    mv(1) = sum / (precision) 2.0;
}

template <typename precision>
void assemble_stiffness_matrix(Problem2D<precision> & p)
{
    // size hints
    int const dim = 2;
    int const nodes_per_element = 3;
    int const number_of_nodes = p.m->nodes.cols();
    int const number_of_elements = p.m->elements.cols();
    assert(p.m->nodes.rows() == dim);
    assert(p.m->elements.rows() == nodes_per_element);

    // sparse matrices still need to be resized for dimension info
    p.A.resize(number_of_nodes, number_of_nodes);

    /*
     * TODO:
     *
     * for each element:
     *     create a local to global index mapping
     *     compute the area (hint: use polyarea)
     *     compute the center of gravity (hint: use mean)
     *     compute the gradients of the three hat functions (hint: use hats_gradients)
     *     compute the values of the local values of the stiffness matrix (hint: use p.a,
     *         computed gradients, computed area, and computed center of gravity)
     *     update the global values of the stiffness matrix (hint: SparseMatrix::coeffRef)
     */

    /* Your code here */

}

template <typename precision>
void assemble_robin_mass_matrix(Problem2D<precision> & p)
{    

    // size hints
    int const dim = 2;
    int const nodes_per_boundary_segment = 2;
    int const number_of_nodes = p.m->nodes.cols();
    int const boundary_segments = p.m->boundary.cols();
    assert(p.m->nodes.rows() == dim);
    assert(p.m->boundary.rows() == nodes_per_boundary_segment);

    // sparse matrices still need to be resized for dimension info
    p.R.resize(number_of_nodes, number_of_nodes);

    /*
     * TODO:
     * 
     * for each boundary segment:
     *     create a local to global index mapping
     *     compute the length (hint: it's just distance)
     *     compute the center of gravity (hint: compute the mean)
     *     compute the values of the local values of the robin mass matrix vector (hint: use p.kappa,
     *         computed length, and computed center of gravity)
     *     update the global values of the robin mass matrix vector
     */

    /* Your code here */

}

template <typename precision>
void assemble_robin_load_vector(Problem2D<precision> & p)
{    
    // size hints
    int const dim = 2;
    int const nodes_per_boundary_segment = 2;
    int const number_of_nodes = p.m->nodes.cols();
    int const boundary_segments = p.m->boundary.cols();
    assert(p.m->nodes.rows() == dim);
    assert(p.m->boundary.rows() == nodes_per_boundary_segment);

    p.r.resize(p.m->nodes.cols(), 1);

    /*
     * TODO:
     * 
     * for each boundary segment:
     *     create a local to global index mapping
     *     compute the length (hint: it's just distance)
     *     compute the center of gravity (hint: compute the mean)
     *     compute the values of the local values of the robin load vector (hint: use p.kappa,
     *         p.gD, p.gN, computed length, and computed center of gravity)
     *     update the global values of the robin load vector
     */

     /* Your code here */

}

template <typename precision>
void solve(Problem2D<precision> & p)
{
    assert(p.is_valid());
    assemble_stiffness_matrix(p);
    assemble_robin_mass_matrix(p);
    assemble_robin_load_vector(p);

    /*
     * TODO: solve for p.u! (hint: use a sparse solver as well as p.A, p.R, p.u, and p.r)
     */

    /* Your code here */

}

}  // namespace fem

#endif  // __FEM_2D_H