//
// Created by Roberts Kalnins on 30/04/2021.
//

#include "main_controller.h"

using namespace grapher::models;

namespace grapher::controllers {

    MainController::MainController(QObject *parent) : QObject{parent} {
        connect(&menu_controller_, &MenuController::saveWorkspaceAsClicked, this,
                &MainController::handleSaveWorkspaceAsClicked);
        connect(&menu_controller_, &MenuController::saveWorkspaceClicked, this,
                &MainController::handleSaveWorkspaceClicked);
        connect(&menu_controller_, &MenuController::newWorkspaceClicked, this,
                &MainController::handleNewWorkspaceClicked);
        connect(&menu_controller_, &MenuController::openWorkspaceClicked, this,
                &MainController::handleOpenWorkspaceClicked);
    }

    MenuController *MainController::getMenuController() {
        return &menu_controller_;
    }

    void MainController::handleNewWorkspaceClicked() {
        qDebug() << "New workspace clicked";
    }

    void MainController::handleSaveWorkspaceClicked() {
        qDebug() << "Save workspace clicked";
    }

    void MainController::handleSaveWorkspaceAsClicked(const QUrl &url) {
        qDebug() << "Save workspace as clicked" << url.toString();
    }

    void MainController::handleOpenWorkspaceClicked(const QUrl &url) {
        qDebug() << "Open workspace clicked " << url.toString();
    }

}