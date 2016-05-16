#ifndef __SHADER_H
#define __SHADER_H

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <linmath.h>

#include "events.h"
#include "gui.h"
#include "element.h"
#include "camera2D.h"
#include "camera3D.h"

class VertexColorShaderProgram;

class ShaderRenderEvent : public Event
{
public:
    ShaderRenderEvent(VertexColorShaderProgram * p = NULL) : program(p) {}
    VertexColorShaderProgram * program;
};

class VertexColorShaderProgram : public Element
{
public:
    VertexColorShaderProgram() {}
    virtual ~VertexColorShaderProgram() {}
    virtual void setEventManager(EventManager * event_manager)
    {
        Element::setEventManager(event_manager);
        _event_manager->addEventHook(this, &VertexColorShaderProgram::removeElementEventHooksEventHook);
        _event_manager->addEventHook(this, &VertexColorShaderProgram::windowRenderEventHook);
        _event_manager->addEventHook(this, &VertexColorShaderProgram::camera2DEventHook);
        _event_manager->addEventHook(this, &VertexColorShaderProgram::camera3DEventHook);
    }
    void removeElementEventHooksEventHook(RemoveElementEventHooksEvent const * event)
    {
        assert(event);
        if (event->element == this)
        {
            _event_manager->removeEventHook(this, &VertexColorShaderProgram::removeElementEventHooksEventHook);
            _event_manager->removeEventHook(this, &VertexColorShaderProgram::windowRenderEventHook);
            _event_manager->removeEventHook(this, &VertexColorShaderProgram::camera2DEventHook);
            _event_manager->removeEventHook(this, &VertexColorShaderProgram::camera3DEventHook);
        }
    }
    void windowRenderEventHook(WindowRenderEvent const * event)
    {
        assert(event);
        _event_manager->triggerEvent(new ShaderRenderEvent(this));
    }
    void camera2DEventHook(Camera2DEvent const * event)
    {
        assert(event);
        mat4x4_dup(_v, *event->view);
        mat4x4_dup(_p, *event->perspective);
        update_mvp();
        _event_manager->triggerEvent(new ShaderRenderEvent(this));
    }
    void camera3DEventHook(Camera3DEvent const * event)
    {
        assert(event);
        mat4x4_dup(_v, *event->view);
        mat4x4_dup(_p, *event->perspective);
        update_mvp();
        _event_manager->triggerEvent(new ShaderRenderEvent(this));
    }
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
        GLuint vao, vbo, vbi, program;
        int size, count;
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
    GpuAsset * gpu_create_asset(std::vector<Vertex> const & vv, std::vector<unsigned> const & vi);
    void gpu_update_asset(GpuAsset * ga, std::vector<Vertex> const & vv, std::vector<unsigned> const & vi);
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