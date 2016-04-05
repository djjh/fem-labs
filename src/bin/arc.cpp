// logic master flex
#include "gui.h"

// graphics master flex
#include "shader.h"

// flex master controls
#include "drag.h"
#include "camera3D.h"
#include "ball.h"

// graphical elements of style
#include "tmp.h"
#include "airfoil.h"

int main(void)
{
    Gui gui;

    VertexColorShaderProgram program;
    gui.add_element(&program);

    Camera3D camera;
    gui.add_element(&camera);

    Ball ball;
    gui.add_element(&ball);

    Drag fov_drag(GLFW_KEY_F);
    gui.add_element(&fov_drag);

    Drag pos_drag(GLFW_KEY_W);
    gui.add_element(&pos_drag);

    // Tmp tmp;
    // gui.add_element(&tmp);

    Airfoil<float> airfoil;
    gui.add_element(&airfoil);

    program.initialize();
    gui.initialize();

    while (!gui.should_close())
    {
        gui.step();
    }

    return EXIT_SUCCESS;
}
