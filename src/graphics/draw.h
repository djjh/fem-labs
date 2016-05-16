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
        vertices.push_back(Vertex( -0.6f, -0.4f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f ));
        vertices.push_back(Vertex(  0.6f, -0.4f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f ));
        vertices.push_back(Vertex(  0.0f,  0.6f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f ));

        ga = program->gpu_create_asset(vertices);
    }
    assert(ga);

    program->gpu_draw_triangles(ga);
}

static inline void draw_sphere(float radius, vec3 const & center, VertexColorShaderProgram * program)
{
    assert(program);

    std::vector<Vertex> vertices;

    int nlayers = 100;
    int nslices = 100;
    for (int i = 0; i < nlayers; ++i)
    {
        float zl = radius - 2.f * radius * i / (nlayers - 1);
        float z_radius = radius * cos(asin(zl / radius));
        for (int j = 0; j < nslices; ++j)
        {
            // map [0 nslices] to [0 2pi]
            float theta = (float) (2*M_PI) * j / (nslices - 1);
            float x = z_radius * (float) cos(theta) + center[0];
            float y = z_radius * (float) sin(theta) + center[1];
            float z = zl + center[2];  
            vertices.push_back(Vertex(x, y, z,  1.f,
                                      0.5f * (float) cos(2*M_PI* i / nlayers) + 0.5f,
                                      0.5f * (float) sin(2*M_PI* (nlayers-i) / nlayers) + 0.5f,
                                      0.5f * (float) sin(2*M_PI* (i) / nlayers) * (float) cos(2*M_PI*j/nslices) + 0.5f, 1.f ));
        }   
    }

    std::vector<unsigned> indices;
    for (int i = 0; i < nlayers - 1; ++i)
    {
        for (int j = 0; j < nslices - 1; ++j)
        {
            int idx[4] = {
                i * nslices + j,
                i * nslices + j + 1,
                (i + 1) * nslices + j,
                (i + 1) * nslices + j + 1
            };
            indices.push_back(idx[1]);
            indices.push_back(idx[0]);
            indices.push_back(idx[2]);

            indices.push_back(idx[1]);
            indices.push_back(idx[2]);
            indices.push_back(idx[3]);
        }
    }
    // last slice
    for (int i = 0; i < nlayers - 1; ++i)
    {
        int idx[4] = {
            i * nslices + (nslices - 1),
            i * nslices + 0,
            (i + 1) * nslices + (nslices - 1),
            (i + 1) * nslices + 0
        };
        indices.push_back(idx[1]);
        indices.push_back(idx[0]);
        indices.push_back(idx[2]);

        indices.push_back(idx[1]);
        indices.push_back(idx[2]);
        indices.push_back(idx[3]);
    }
        
    static GpuAsset * ga = NULL;
    if (!ga)
    {
        ga = program->gpu_create_asset(vertices, indices);
    }
    else
    {
        program->gpu_update_asset(ga, vertices, indices);
    }
    assert(ga);

    program->gpu_draw_triangles(ga);
}

static inline void draw_cube(VertexColorShaderProgram * program)
{
    assert(program);

    static GpuAsset * ga = NULL;
    if (!ga)
    {
        std::vector<Vertex> vertices;
        vertices.push_back(Vertex(  0.5f,  0.5f,  0.5f, 1.f, 1.f, 0.f, 0.f, 1.f ));
        vertices.push_back(Vertex(  0.5f, -0.5f,  0.5f, 1.f, 0.f, 1.f, 0.f, 1.f ));
        vertices.push_back(Vertex(  0.5f,  0.5f, -0.5f, 1.f, 0.f, 0.f, 1.f, 1.f ));
        vertices.push_back(Vertex(  0.5f, -0.5f, -0.5f, 1.f, 1.f, 0.f, 0.f, 1.f ));
        vertices.push_back(Vertex( -0.5f,  0.5f,  0.5f, 1.f, 0.f, 1.f, 0.f, 1.f ));
        vertices.push_back(Vertex( -0.5f, -0.5f,  0.5f, 1.f, 0.f, 0.f, 1.f, 1.f ));
        vertices.push_back(Vertex( -0.5f,  0.5f, -0.5f, 1.f, 0.f, 1.f, 0.f, 1.f ));
        vertices.push_back(Vertex( -0.5f, -0.5f, -0.5f, 1.f, 0.f, 0.f, 1.f, 1.f ));

        std::vector<unsigned> indices;
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(3);
        indices.push_back(0);
        indices.push_back(3);
        indices.push_back(2);

        indices.push_back(0);
        indices.push_back(4);
        indices.push_back(5);
        indices.push_back(0);
        indices.push_back(5);
        indices.push_back(1);

        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(6);
        indices.push_back(0);
        indices.push_back(6);
        indices.push_back(4);

        indices.push_back(7);
        indices.push_back(5);
        indices.push_back(4);
        indices.push_back(7);
        indices.push_back(4);
        indices.push_back(6);

        indices.push_back(7);
        indices.push_back(3);
        indices.push_back(1);
        indices.push_back(7);
        indices.push_back(1);
        indices.push_back(5);

        indices.push_back(7);
        indices.push_back(3);
        indices.push_back(2);
        indices.push_back(7);
        indices.push_back(2);
        indices.push_back(6);

        ga = program->gpu_create_asset(vertices, indices);
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
        int size = 25;
        float fsize = (float) size;
        std::vector<Vertex> vertices;
        for (int i = -size; i <= size; ++i)
        {
            vertices.push_back(Vertex( -fsize, 1.f*i, 0, 1.f, 0.5f, 0.5f, 0.5f, 1.f));
            vertices.push_back(Vertex( fsize, 1.f*i,  0, 1.f, 0.5f, 0.5f, 0.5f, 1.f));
            vertices.push_back(Vertex( 1.f*i, -fsize, 0, 1.f, 0.5f, 0.5f, 0.5f, 1.f));
            vertices.push_back(Vertex( 1.f*i, fsize,  0, 1.f, 0.5f, 0.5f, 0.5f, 1.f));
        }

        ga = program->gpu_create_asset(vertices);
    }
    assert(ga);

    program->gpu_draw_lines(ga);
}

#endif  // __DRAW_H