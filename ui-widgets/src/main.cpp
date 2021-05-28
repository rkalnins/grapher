#include <QApplication>

#include "main_window.h"
#include "main_controller.h"
#include "workspace_model.h"
#include "data_providers_model.h"

#include "menu_model.h"
#include "data_model.h"


using grapher::controllers::MainController;
using grapher::models::DataProvidersModel;
using grapher::models::MenuModel;
using grapher::models::DataModel;
using grapher::models::WorkspaceModel;

int main ( int argc, char *argv[] ) {
    QApplication a(argc, argv);

    MainController main_controller;

    MenuModel menu_model;
    main_controller.setMenuModel(menu_model);

    DataModel data_model;
    main_controller.setDataModel(data_model);

    WorkspaceModel workspace_model;
    main_controller.setWorkspaceModel(workspace_model);


    DataProvidersModel providers;
    main_controller.setProvidersModel(providers);

    MainWindow window(main_controller);
    main_controller.getMenuController()->newWorkspaceClicked();

    window.show();

    return QApplication::exec();
}
