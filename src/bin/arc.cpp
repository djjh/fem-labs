// logic master flex
#include "gui.h"
#include "draw.h"

// graphics master flex
#include "shader.h"

// flex master control elements
#include "drag.h"

// 2D camera control elements
#include "camera2D.h"
#include "pan2D.h"
#include "zoom2D.h"

// 3D camera control elements
#include "camera3D.h"
#include "ball.h"

// graphical elements of style
#include "grid.h"
#include "heat.h"
#include "tmp.h"
#include "airfoil.h"
#include "hose.h"

int main(int argc, char ** argv)
{
    Gui gui;

    VertexColorShaderProgram program;
    gui.add_element(&program);


    
    int num = 0;
    if (argc > 1)
    {
        std::stringstream ss(argv[1]);
        ss >> num;
    }

    switch (num)
    {
        case 1:
        {
            gui.add_element(new Pan2D);
            gui.add_element(new Zoom2D);
            gui.add_element(new Camera2D);
            gui.add_element(new Grid);
            gui.add_element(new Heat<float>);
            break;
        }
        case 2:
        {
            gui.add_element(new Camera3D);
            gui.add_element(new Ball);
            gui.add_element(new Drag(GLFW_KEY_F));
            gui.add_element(new Drag(GLFW_KEY_W));
            // gui.add_element(new Tmp);
            gui.add_element(new Airfoil<float>);

            break;
        }
        default:
        {
            gui.add_element(new Camera3D);
            gui.add_element(new Ball);
            gui.add_element(new Drag(GLFW_KEY_F));
            gui.add_element(new Drag(GLFW_KEY_W));
            gui.add_element(new Hose<float>);
            break;
        }
    }

    program.initialize();
    gui.initialize();

    while (!gui.should_close())
    {
        gui.step();
    }


    // TODO: remove element for a proper clean up.
    // or better yet, let the event manager handle
    // that.

    return EXIT_SUCCESS;
}
