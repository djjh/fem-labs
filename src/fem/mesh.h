#ifndef __MESH_H
#define __MESH_H

#include <fstream>
#include <sstream>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/Sparse>

using Eigen::Matrix;
using Eigen::SparseMatrix;

using Eigen::Dynamic;

template <typename precision>
class Mesh
{
public:
    Matrix<precision, Dynamic, Dynamic> nodes; // point matrix        (node dimension  x number of nodes)
    Matrix<int, Dynamic, Dynamic> elements;    // connectivity matrix (nodes per element x number of elements)
    Matrix<int, Dynamic, Dynamic> boundary;    // boundary matrix     (nodes per boundary segment x number of boundary segments)
                                               // intuitively, I think (nodes per boundary segment) == (node per element) - 1
};

std::istream & mygetline(std::istream & is, std::string & str)
{
    char first = '#';
    str.clear();  
    do
    {
        getline(is, str);
        std::stringstream ss(str);
        ss >> first;
    }
    while (str.length() == 0 || first == '#');

    return is;
}

template <typename precision>
void read_in_mesh(std::vector<precision> & nodes,
                  std::vector<int> & boundary,
                  std::vector<int> & elements,
                  std::string const & basename)
{
    std::string line;
    nodes.clear();
    boundary.clear();
    elements.clear();

    std::fstream poly_file(basename + ".poly", std::fstream::in);
    if (!poly_file.is_open())
        poly_file.open(basename + ".smesh", std::fstream::in);

    std::fstream node_file(basename + ".node", std::fstream::in);
    std::fstream ele_file(basename + ".ele", std::fstream::in);

    assert(node_file.is_open());
    assert(poly_file.is_open());
    assert(ele_file.is_open());

    // nodes, from .node
    {
        int num_nodes = 0;
        int dim_nodes = 0; // 
        int num_attrs = 0;
        int num_bmark = 0; // 0 or 1 for 2d at least
        {
            mygetline(node_file, line);
            std::stringstream line_ss(line);
            line_ss >> num_nodes;
            line_ss >> dim_nodes;
            line_ss >> num_attrs;
            line_ss >> num_bmark;
        }
        printf("[%s] %d %d %d %d\n", line.c_str(), num_nodes, dim_nodes, num_attrs, num_bmark);

        nodes.resize(num_nodes * dim_nodes);
        for (int i = 0; i < num_nodes; ++i)
        {
            mygetline(node_file, line);
            std::stringstream line_ss(line);
            printf("[%s] ", line.c_str());

            int vertex_num = 0;
            line_ss >> vertex_num;
            printf("%2d ", vertex_num);

            for (int j = 0; j < dim_nodes; ++j)
            {
                line_ss >> nodes[i*dim_nodes + j];
                printf("% .3f ", nodes[i*dim_nodes + j]);
            }
            for (int j = 0; j < num_attrs; ++j)
            {
                precision tmp;
                line_ss >> tmp;
                printf("% .3f ", tmp);
            }

            for (int j = 0; j < num_bmark; ++j)
            {
                int tmp;
                line_ss >> tmp;
                printf("%d    ", tmp);
            }
            printf("\n");
        }
    }

    // boundary, from .poly
    {
        int num_nodes = 0;
        int dim_nodes = 0; // 
        int num_attrs = 0;
        int num_bmark = 0; // 0 or 1 for 2d at least
        {
            mygetline(poly_file, line);
            std::stringstream line_ss(line);
            line_ss >> num_nodes;
            line_ss >> dim_nodes;
            line_ss >> num_attrs;
            line_ss >> num_bmark;
        }
        printf("[%s] %d %d %d %d\n", line.c_str(), num_nodes, dim_nodes, num_attrs, num_bmark);
        assert(num_nodes == 0); // cause we are using node file too...

        int num_segs = 0;
        int poly_num_bmark = 0;
        {
            mygetline(poly_file, line);
            std::stringstream line_ss(line);
            line_ss >> num_segs;
            line_ss >> poly_num_bmark;
        }
        printf("[%s] %d %d\n", line.c_str(), num_segs, num_bmark);
        assert(poly_num_bmark == num_bmark);

        boundary.resize(num_segs * dim_nodes);
        for (int i = 0; i < num_segs; ++i)
        {
            mygetline(poly_file, line);
            std::stringstream line_ss(line);
            printf("[%s] ", line.c_str());

            int vertex_num = 0;
            line_ss >> vertex_num;
            printf("%2d ", vertex_num);

            for (int j = 0; j < dim_nodes; ++j)
            {
                line_ss >> boundary[i*dim_nodes + j];
                --boundary[i*dim_nodes + j];
                printf("%d ", boundary[i*dim_nodes + j]);
            }

            for (int j = 0; j < num_bmark; ++j)
            {
                int tmp;
                line_ss >> tmp;
                printf("%d    ", tmp);
            }
            printf("\n");
        }
    }

    // elements, from .ele
    {
        int num_elements = 0;
        int num_nodes_per_element = 0; // 
        int num_attrs = 0;
        {
            mygetline(ele_file, line);
            std::stringstream line_ss(line);
            line_ss >> num_elements;
            line_ss >> num_nodes_per_element;
            line_ss >> num_attrs;
        }
        printf("[%s] %d %d %d\n", line.c_str(), num_elements, num_nodes_per_element, num_attrs);

        elements.resize(num_elements * num_nodes_per_element);
        for (int i = 0; i < num_elements; ++i)
        {
            mygetline(ele_file, line);
            std::stringstream line_ss(line);
            printf("[%s] ", line.c_str());

            int element_num = 0;
            line_ss >> element_num;
            printf("%2d ", element_num);

            for (int j = 0; j < num_nodes_per_element; ++j)
            {
                line_ss >> elements[i*num_nodes_per_element + j];
                --elements[i*num_nodes_per_element + j];
                printf("%d ", elements[i*num_nodes_per_element + j]);
            }

            for (int j = 0; j < num_attrs; ++j)
            {
                precision tmp;
                line_ss >> tmp;
                printf("%f    ", tmp);
            }
            printf("\n");
        }
    }

    node_file.close();
    poly_file.close();
    ele_file.close();
}

template <typename precision>
void read_in_mesh(Mesh<precision> & mesh,
                  std::string const & basename)
{
    std::string line;

    mesh.nodes.resize(0, 0);
    mesh.elements.resize(0, 0);
    mesh.boundary.resize(0, 0);

    std::fstream poly_file(basename + ".poly", std::fstream::in);
    if (!poly_file.is_open())
        poly_file.open(basename + ".smesh", std::fstream::in);

    std::fstream node_file(basename + ".node", std::fstream::in);
    std::fstream ele_file(basename + ".ele", std::fstream::in);

    assert(node_file.is_open());
    assert(poly_file.is_open());
    assert(ele_file.is_open());

    // nodes, from .node
    {
        int num_nodes = 0;
        int dim_nodes = 0; // 
        int num_attrs = 0;
        int num_bmark = 0; // 0 or 1 for 2d at least
        {
            mygetline(node_file, line);
            std::stringstream line_ss(line);
            line_ss >> num_nodes;
            line_ss >> dim_nodes;
            line_ss >> num_attrs;
            line_ss >> num_bmark;
        }
        // printf("[%s] %d %d %d %d\n", line.c_str(), num_nodes, dim_nodes, num_attrs, num_bmark);

        mesh.nodes.resize(dim_nodes, num_nodes);
        for (int i = 0; i < num_nodes; ++i)
        {
            mygetline(node_file, line);
            std::stringstream line_ss(line);
            // printf("[%s] ", line.c_str());

            int vertex_num = 0;
            line_ss >> vertex_num;
            // printf("%2d ", vertex_num);

            for (int j = 0; j < dim_nodes; ++j)
            {
                line_ss >> mesh.nodes(j, i);
                // printf("% .3f ", mesh.nodes(j, i));
            }
            for (int j = 0; j < num_attrs; ++j)
            {
                precision tmp;
                line_ss >> tmp;
                // printf("% .3f ", tmp);
            }

            for (int j = 0; j < num_bmark; ++j)
            {
                int tmp;
                line_ss >> tmp;
                // printf("%d    ", tmp);
            }
            // printf("\n");
        }
    }

    // boundary, from .poly
    {
        int num_nodes = 0;
        int nodes_per_boundary_segment = 0; // 
        int num_attrs = 0;
        int num_bmark = 0; // 0 or 1 for 2d at least
        {
            mygetline(poly_file, line);
            std::stringstream line_ss(line);
            line_ss >> num_nodes;
            line_ss >> nodes_per_boundary_segment;
            line_ss >> num_attrs;
            line_ss >> num_bmark;
        }
        // printf("[%s] %d %d %d %d\n", line.c_str(), num_nodes, nodes_per_boundary_segment, num_attrs, num_bmark);
        // assert(num_nodes == 0); // cause we are using node file too...
        for (int i = 0; i < num_nodes; ++i)
        {
            mygetline(poly_file, line);
        }

        int num_segs = 0;
        int poly_num_bmark = 0;
        {
            mygetline(poly_file, line);
            std::stringstream line_ss(line);
            line_ss >> num_segs;
            line_ss >> poly_num_bmark;
        }
        // printf("[%s] %d %d\n", line.c_str(), num_segs, num_bmark);
        assert(poly_num_bmark == num_bmark);

        mesh.boundary.resize(nodes_per_boundary_segment, num_segs);
        for (int i = 0; i < num_segs; ++i)
        {
            mygetline(poly_file, line);
            std::stringstream line_ss(line);
            // printf("[%s] ", line.c_str());

            int vertex_num = 0;
            line_ss >> vertex_num;
            // printf("%2d ", vertex_num);

            for (int j = 0; j < nodes_per_boundary_segment; ++j)
            {
                line_ss >> mesh.boundary(j, i);
                --mesh.boundary(j, i);
                // printf("%d ", mesh.boundary(j, i));
            }

            for (int j = 0; j < num_bmark; ++j)
            {
                int tmp;
                line_ss >> tmp;
                // printf("%d    ", tmp);
            }
            // printf("\n");
        }
    }

    // elements, from .ele
    {
        int num_elements = 0;
        int num_nodes_per_element = 0; // 
        int num_attrs = 0;
        {
            mygetline(ele_file, line);
            std::stringstream line_ss(line);
            line_ss >> num_elements;
            line_ss >> num_nodes_per_element;
            line_ss >> num_attrs;
        }
        // printf("[%s] %d %d %d\n", line.c_str(), num_elements, num_nodes_per_element, num_attrs);

        mesh.elements.resize(num_nodes_per_element, num_elements);
        for (int i = 0; i < num_elements; ++i)
        {
            mygetline(ele_file, line);
            std::stringstream line_ss(line);
            // printf("[%s] ", line.c_str());

            int element_num = 0;
            line_ss >> element_num;
            // printf("%2d ", element_num);

            for (int j = 0; j < num_nodes_per_element; ++j)
            {
                line_ss >> mesh.elements(j, i);
                --mesh.elements(j, i);
                // printf("%d ", mesh.elements(j, i));
            }

            for (int j = 0; j < num_attrs; ++j)
            {
                precision tmp;
                line_ss >> tmp;
                // printf("%f    ", tmp);
            }
            // printf("\n");
        }

        // // double check dims....
        // std::cout << "mesh.nodes : (" << mesh.nodes.rows() << ", " << mesh.nodes.cols() << ")" << std::endl;
        // std::cout << "mesh.elements : (" << mesh.elements.rows() << ", " << mesh.elements.cols() << ")" << std::endl;
        // std::cout << "mesh.boundary : (" << mesh.boundary.rows() << ", " << mesh.boundary.cols() << ")" << std::endl;
    }

    node_file.close();
    poly_file.close();
    ele_file.close();
}

#endif  // __MESH_H