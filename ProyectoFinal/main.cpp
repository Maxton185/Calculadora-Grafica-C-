#include <QApplication>
#include "WindowGraficadora.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    WindowGraficadora ventanaPrincipal;
    ventanaPrincipal.show();
    return app.exec();
}
