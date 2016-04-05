#ifndef __TMP_H
#define __TMP_H

#include <cassert>

#include "element.h"
#include "window_events.h"
#include "adhoc_events.h"
#include "draw.h"
#include "mesh.h"

typedef VertexColorShaderProgram::Vertex Vertex;
typedef VertexColorShaderProgram::GpuAsset GpuAsset;

class Tmp : public Element
{
public:
    Tmp()
    {
        ga = NULL;

        Mesh<float> m;
        read_in_mesh(m, "dat/a/A.1"); // intented to be run from project root

        float scale_up = 5.f;
        for (int i = 0; i < m.nodes.cols(); ++i)
            vertices.push_back(Vertex(scale_up * m.nodes(0, i),
                                      scale_up * m.nodes(1, i),
                                      0.f, 1.f,
                                      0.5f, 0.5f, 0.5f, 1.f));

        // for drawing wireframe elements
        for (int i = 0; i < m.elements.cols(); ++i)
        {
            indices.push_back(m.elements(0, i));
            indices.push_back(m.elements(1, i));

            indices.push_back(m.elements(0, i));
            indices.push_back(m.elements(2, i));

            indices.push_back(m.elements(1, i));
            indices.push_back(m.elements(2, i));
        }

        // for drawing boundary
        // for (int i = 0; i < m.boundary.cols(); ++i)
        // {
        //     indices.push_back(m.boundary(0, i));
        //     indices.push_back(m.boundary(1, i));
        // }

        // // double check dims....
        // std::cout << "m.nodes : (" << m.nodes.rows() << ", " << m.nodes.cols() << ")" << std::endl;
        // std::cout << "m.elements : (" << m.elements.rows() << ", " << m.elements.cols() << ")" << std::endl;
        // std::cout << "m.boundary : (" << m.boundary.rows() << ", " << m.boundary.cols() << ")" << std::endl;
    }
    virtual ~Tmp() {}
    virtual void setEventManager(EventManager * event_manager)
    {
        Element::setEventManager(event_manager);
        _event_manager->addEventHook(this, &Tmp::shaderRenderEvent);
        _event_manager->addEventHook(this, &Tmp::removeElementEventHooksEventHook);
    }
    void removeElementEventHooksEventHook(RemoveElementEventHooksEvent const * event)
    {
        if (event->element == this)
        {
            _event_manager->removeEventHook(this, &Tmp::shaderRenderEvent);
            _event_manager->removeEventHook(this, &Tmp::removeElementEventHooksEventHook);
        }
    }
    void shaderRenderEvent(ShaderRenderEvent const * event)
    {
        assert(event);
        assert(event->program);

        // triangles
        if (!ga)
            ga = event->program->gpu_create_asset(vertices, indices);
        event->program->gpu_draw_lines(ga);
    }
private:
    GpuAsset * ga;
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
};

#endif  // __TMP_H