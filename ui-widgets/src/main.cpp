#include <QApplication>

#include "main_window.h"
#include "main_controller.h"
#include "workspace_model.h"

#include "menu_model.h"

using grapher::controllers::MainController;
using grapher::models::MenuModel;
using grapher::models::WorkspaceModel;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainController main_controller;

    MenuModel menu_model;
    main_controller.setMenuModel(menu_model);

    WorkspaceModel workspace_model;
    main_controller.setWorkspaceModel(workspace_model);

    MainWindow window(main_controller);
    main_controller.getMenuController()->newWorkspaceClicked();
    window.show();

    return QApplication::exec();
}
