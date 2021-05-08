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
        connect(&menu_controller_, &MenuController::exportDataClicked, this, &MainController::handleCsvExport);

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

    DataProvidersModel *MainController::getProviders() {
        return providers_;
    }


    void MainController::configureProviders() {
        QJsonObject o = workspace_model_->getWorkspace()->getWorkspaceData();
        QJsonArray providers = o["providers"].toArray();
        providers_->populateProviders(providers);

        qDebug() << "connecting handlers and channels " << data_model_->getHandlerCount();

        for (int i = 0; i < data_model_->getHandlerCount(); ++i) {
            DataHandler *handler = data_model_->getDataHandler(i);
            const QString &id = handler->getId();
            const QString &p = handler->getProvider();

            qDebug() << "id: " << id << "  prov: " << p;

            AbstractDataProviderModel *provider = providers_->getProviderByName(p);
            if (provider) {
                handler->setChannel(provider->getChannelById(id));
                qDebug() << "connected handler to channel " << handler->getName() << " -> "
                         << provider->getChannelById(id)->getName();
            }
        }
    }


    void MainController::setProvidersModel(models::DataProvidersModel &model) {
        providers_ = &model;
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
        data_model_->setup();
        qDebug() << "set data model";
    }

    void MainController::connectSaveSignals() {
        if (!data_model_ || !workspace_model_) {
            return;
        }

        connect(data_model_, &DataModel::collectedGraphData, workspace_model_->getWorkspace(),
                &WorkspaceHandler::saveWorkspaceGraphs);
        connect(workspace_model_->getWorkspace(), &WorkspaceHandler::needSaveWorkspaceGraphs, data_model_,
                &DataModel::collectSaveGraphsData);
    }

    void MainController::disconnectSaveSignals() {
        if (!data_model_ || !workspace_model_) {
            return;
        }

        disconnect(data_model_, &DataModel::collectedGraphData, workspace_model_->getWorkspace(),
                   &WorkspaceHandler::saveWorkspaceGraphs);
        disconnect(workspace_model_->getWorkspace(), &WorkspaceHandler::needSaveWorkspaceGraphs, data_model_,
                   &DataModel::collectSaveGraphsData);
    }

    void MainController::openWorkspace() {
        if (!workspace_model_) {
            return;
        }

        disconnectSaveSignals();
        disconnect(workspace_model_, &WorkspaceModel::workspaceUpdated, this,
                   &MainController::pushSettingChange);

        menu_controller_.setWorkspace(*workspace_model_->getWorkspace());

        connect(workspace_model_, &WorkspaceModel::workspaceUpdated, this,
                &MainController::pushSettingChange);
        connectSaveSignals();
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

    DataModel *MainController::getDataModel() {
        return data_model_;
    }

    void MainController::handleNewWorkspaceClicked() {
        qDebug() << "New workspace clicked";

        if (!workspace_model_) {
            return;
        }

        workspace_model_->newWorkspace();
    }

    void MainController::handleCsvExport(const QUrl &url, std::vector<QCPDataContainer<QCPGraphData> *> &data) {
        export_handler_.exportTo(url, data);
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
        configureProviders();
        qDebug() << "pushing setting change to mainWindow";
        emit dataChanged(data);
        qDebug() << "pushed setting change to mainWindow";
    }

    void MainController::handleTitleChanged() {
        emit titleChanged(menu_model_->getTitle());
        qDebug() << "title changing to " << menu_model_->getTitle();
    }

}