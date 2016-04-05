#include "gui.h"
#include "grid.h"
#include "heat.h"

int main(void)
{
    Gui gui;
    gui.initialize();

    Grid grid;
    HeatProblem<float, 100> problem;

    gui.add_element(&grid);
    gui.add_element(&problem);

    while (!gui.should_close())
    {
        gui.step();
    }

    gui.remove_element(&problem);
    gui.remove_element(&grid);

    return EXIT_SUCCESS;
}
