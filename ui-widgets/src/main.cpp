#include <QApplication>

#include "main_window.h"
#include "main_controller.h"

#include "menu_model.h"

using grapher::controllers::MainController;
using grapher::models::MenuModel;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainController main_controller;

    MenuModel menu_model;
    main_controller.menuController()->setModel(menu_model);


    MainWindow window(main_controller);
    window.show();

    return QApplication::exec();
}
