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

    MenuController *MainController::menuController() {
        return &menu_controller_;
    }

    void MainController::handleNewWorkspaceClicked() {
        qDebug() << "New workspace clicked";
    }

    void MainController::handleSaveWorkspaceClicked() {
        qDebug() << "Save workspace clicked";
    }

    void MainController::handleSaveWorkspaceAsClicked() {
        qDebug() << "Save workspace as clicked";
    }

    void MainController::handleOpenWorkspaceClicked() {
        qDebug() << "Open workspace clicked";
    }

}