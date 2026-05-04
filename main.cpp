#include <QApplication>
#include "GUI.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    GUI window;
    window.show();
    return app.exec();
}