#include "SearchFile.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SearchFile window;
    window.show();
    return app.exec();
}
