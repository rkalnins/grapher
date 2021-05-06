//
// Created by Roberts Kalnins on 30/04/2021.
//

#include "main_controller.h"

#include "workspace_handler.h"

#include "workspace_model.h"
#include "menu_model.h"
#include "data_model.h"

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

        connect(&workspace_controller_, &WorkspaceController::settingUpdated, this,
                &MainController::handleSettingChange);
    }

    void MainController::setWorkspaceModel(models::WorkspaceModel &model) {
        disconnect(workspace_created_connection_);
        workspace_model_ = &model;
        workspace_created_connection_ = connect(workspace_model_, &WorkspaceModel::workspaceCreated, this,
                                                &MainController::openWorkspace);
        qDebug() << "connected new model";
    }

    void MainController::setMenuModel(models::MenuModel &model) {
        if (menu_model_) {
            disconnect(menu_model_, &MenuModel::titleChanged, this, &MainController::handleTitleChanged);
        }
        menu_model_ = &model;
        menu_controller_.setModel(model);
        connect(menu_model_, &MenuModel::titleChanged, this, &MainController::handleTitleChanged);
        qDebug() << "reconnected menu model";
    }

    void MainController::setDataModel(models::DataModel &model) {
        data_model_ = &model;
        data_provider_.setModel(data_model_);
        qDebug() << "set data model";
    }

    void MainController::openWorkspace() {
        if (!workspace_model_) {
            return;
        }

        disconnect(workspace_model_, &WorkspaceModel::workspaceUpdated, this,
                   &MainController::pushSettingChange);

        menu_controller_.setWorkspace(*workspace_model_->getWorkspace());

        connect(workspace_model_, &WorkspaceModel::workspaceUpdated, this,
                &MainController::pushSettingChange);
    }

    MenuController *MainController::getMenuController() {
        return &menu_controller_;
    }

    WorkspaceController *MainController::getWorkspaceController() {
        return &workspace_controller_;
    }

    DataController *MainController::getDataController() {
        return &data_controller_;
    }

    DataProvider *MainController::getDataProvider() {
        return &data_provider_;
    }

    void MainController::handleNewWorkspaceClicked() {
        qDebug() << "New workspace clicked";

        if (!workspace_model_) {
            return;
        }

        workspace_model_->newWorkspace();
    }

    void MainController::handleSaveWorkspaceClicked() {
        qDebug() << "Save workspace clicked";

        if (!workspace_model_) {
            return;
        }

        workspace_model_->save();
    }

    void MainController::handleSaveWorkspaceAsClicked(const QUrl &url) {
        qDebug() << "Save workspace as clicked" << url.toString();

        if (!workspace_model_) {
            return;
        }

        workspace_model_->saveAs(url);

        qDebug() << "Done saving to workspace";
    }

    void MainController::handleOpenWorkspaceClicked(const QUrl &url) {
        qDebug() << "Open workspace clicked " << url.toString();

        if (!workspace_model_) {
            return;
        }

        workspace_model_->openWorkspace(url);
    }

    void MainController::handleSettingChange(const QJsonObject &data) {
        if (!workspace_model_) {
            return;
        }
        qDebug() << "handling setting change";
        workspace_model_->setData(data);
    }

    void MainController::pushSettingChange(const QJsonObject &data) {
        data_model_->setDataFromJson(data);
        qDebug() << "pushing setting change to mainWindow";
        emit dataChanged(data);
        qDebug() << "pushed setting change to mainWindow";
    }

    void MainController::handleTitleChanged() {
        emit titleChanged(menu_model_->getTitle());
        qDebug() << "title changing to " << menu_model_->getTitle();
    }

}