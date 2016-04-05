#ifndef __SHADER_H
#define __SHADER_H

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <linmath.h>

class VertexColorShaderProgram
{
public:
    struct Vertex
    {
        Vertex(float vx, float vy, float vz, float vw, float vr, float vg, float vb, float va)
        : x(vx), y(vy), z(vz), w(vw), r(vr), g(vg), b(vb), a(va) 
        {}
        float x, y, z, w;
        float r, g, b, a;
    };
    struct GpuAsset
    {  
        GpuAsset() : vbo(0), vao(0), program(0) {}
        GpuAsset(GLuint vbo_, GLuint vao_, GLuint program_) : vbo(vbo_), vao(vao_), program(program_) {}
        GLuint vbo, vao, program;
        int size;
    };
    void initialize();
    void set_m(mat4x4 m);
    void set_v(mat4x4 v);
    void set_p(mat4x4 p);
    void draw_triangles(std::vector<Vertex> const & vv);
    void draw_lines(std::vector<Vertex> const & vv);
    void draw_line_strip(std::vector<Vertex> const & vv);
    GpuAsset * gpu_create_asset(std::vector<Vertex> const & vv);
    void gpu_update_asset(GpuAsset * ga, std::vector<Vertex> const & vv);
    void gpu_draw_triangles(GpuAsset const * ga);
    void gpu_draw_lines(GpuAsset const * ga);
    void gpu_draw_line_strip(GpuAsset const * ga);
    void gpu_draw_points(GpuAsset const * ga);
private:
    void compile();
    void link();
    void initialize_buffers();
    void initialize_mvp();
    void update_mvp();
    void draw_vertices(std::vector<Vertex> const & vv, GLint mode);
    void gpu_draw_vertices(GpuAsset const * ga, GLint mode);
    GLuint vbo, vao, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;
    mat4x4 _m, _v, _p;
};

#endif  // __SHADER_H