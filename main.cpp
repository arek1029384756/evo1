#include <QApplication>
#include <iostream>
#include "window.h"

int main(int argc, char *argv[])
{
    try {
        QApplication app(argc, argv);
        gui::Window world;
        world.show();
        return app.exec();
    } catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch(...) {
        std::cout << "Unknown exception! Something really bad happened"
            << std::endl;
        return EXIT_FAILURE;
    }
}
