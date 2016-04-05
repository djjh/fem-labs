#ifndef __DRAW_H
#define __DRAW_H

#include <Eigen/Dense>

#include "shader.h"

using Eigen::Matrix;

typedef VertexColorShaderProgram::Vertex Vertex;
typedef VertexColorShaderProgram::GpuAsset GpuAsset;

static inline void draw_triangle(VertexColorShaderProgram * program)
{
    assert(program);

    static GpuAsset * ga = NULL;
    if (!ga)
    {
        std::vector<Vertex> vertices;
        if (vertices.size() == 0)
        {
            vertices.push_back(Vertex( -0.6f, -0.4f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f ));
            vertices.push_back(Vertex(  0.6f, -0.4f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f ));
            vertices.push_back(Vertex(  0.0f,  0.6f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f ));
        }

        ga = program->gpu_create_asset(vertices);
    }
    assert(ga);

    program->gpu_draw_triangles(ga);
}

static inline void draw_grid(VertexColorShaderProgram * program)
{
    assert(program);

    static GpuAsset * ga = NULL;
    if (!ga)
    {
        std::vector<Vertex> vertices;
        for (int i = -100; i <= 100; ++i)
        {
            vertices.push_back(Vertex( -100.f, 1.f*i, 0, 1.f, 0.5f, 0.5f, 0.5f, 1.f));
            vertices.push_back(Vertex( 100.f, 1.f*i,  0, 1.f, 0.5f, 0.5f, 0.5f, 1.f));
            vertices.push_back(Vertex( 1.f*i, -100.f, 0, 1.f, 0.5f, 0.5f, 0.5f, 1.f));
            vertices.push_back(Vertex( 1.f*i, 100.f,  0, 1.f, 0.5f, 0.5f, 0.5f, 1.f));
        }
        vertices.push_back(Vertex( -100.f, 0.f, 0, 1.f, 1.f, 1.f, 1.f, 1.f));
        vertices.push_back(Vertex( 100.f, 0.f,  0, 1.f, 1.f, 1.f, 1.f, 1.f));
        vertices.push_back(Vertex( 0.f, -100.f, 0, 1.f, 1.f, 1.f, 1.f, 1.f));
        vertices.push_back(Vertex( 0.f, 100.f,  0, 1.f, 1.f, 1.f, 1.f, 1.f));

        ga = program->gpu_create_asset(vertices);
    }
    assert(ga);

    program->gpu_draw_lines(ga);
}

template <typename precision, int rows>
void draw_mesh_1D(Matrix<precision, rows, 1> const & x,
                  Matrix<precision, rows, 1> const & u,
                  VertexColorShaderProgram * program)
{
    assert(program);

    precision max = u(0);
    precision min = u(0);
    for (unsigned i = 0; i < rows; ++i)
    {
        if (max < u(i))
            max = u(i);
        if (min > u(i))
            min = u(i);
    }

    std::vector<Vertex> curve_vertices;
    curve_vertices.reserve(rows);
    for (unsigned i = 0; i < rows; ++i)
    {
        precision ui = (u(i) - min) / (max - min);
        curve_vertices.push_back(Vertex(x(i), u(i), 0.f, 1.f, ui, 0.f, 1.f - ui, 1.f));
    }

    std::vector<Vertex> height_vertices;
    height_vertices.reserve(2*rows);
    for (unsigned i = 0; i < rows; ++i)
    {
        precision ui = (u(i) - min) / (max - min);
        height_vertices.push_back(Vertex(x(i), 0.f, 0.f, 1.f, ui, 0.f, 1.f - ui, 1.f));
        height_vertices.push_back(Vertex(x(i), u(i), 0.f, 1.f, ui, 0.f, 1.f - ui, 1.f));
    }

    static GpuAsset * ga_curve = NULL;
    static GpuAsset * ga_heights = NULL;
    if (!ga_curve || !ga_heights)
    {
        ga_curve = program->gpu_create_asset(curve_vertices);
        ga_heights = program->gpu_create_asset(height_vertices);
    }
    else
    {
        program->gpu_update_asset(ga_curve, curve_vertices);
        program->gpu_update_asset(ga_heights, height_vertices);
    }

    program->gpu_draw_line_strip(ga_curve);
    program->gpu_draw_lines(ga_heights);
}

#endif  // __DRAW_H