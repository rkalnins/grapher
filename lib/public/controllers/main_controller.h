//
// Created by Roberts Kalnins on 30/04/2021.
//

#pragma once

#include <QObject>
#include <QString>
#include <QDebug>
#include <QUrl>

#include "menu_controller.h"
#include "workspace_controller.h"
#include "data_controller.h"

#include "data_provider.h"
#include "data_model.h"

#include "globals.h"


namespace grapher {
    namespace models {
        class MenuModel;

        class DataModel;

        class WorkspaceModel;
    }

    namespace controllers {
        class GRAPHER_EXPORT MainController : public QObject {
        Q_OBJECT

            Q_DISABLE_COPY_MOVE(MainController)

            Q_PROPERTY(controllers::MenuController *menuController READ getMenuController CONSTANT)
            Q_PROPERTY(controllers::WorkspaceController *workspaceController READ getWorkspaceController CONSTANT)
            Q_PROPERTY(controllers::DataController *dataController READ getDataController CONSTANT)
            Q_PROPERTY(DataProvider *dataProvider READ getDataProvider CONSTANT)
            Q_PROPERTY(models::DataModel *dataModel READ getDataModel CONSTANT)

        public:
            explicit MainController(QObject *parent = nullptr);

            MenuController *getMenuController();

            WorkspaceController *getWorkspaceController();

            DataController *getDataController();

            DataProvider *getDataProvider();

            models::DataModel *getDataModel();

            virtual ~MainController() = default;

            void setWorkspaceModel(models::WorkspaceModel &model);

            void setMenuModel(models::MenuModel &model);

            void setDataModel(models::DataModel &model);

            void connectSaveSignals();

            void disconnectSaveSignals();

        Q_SIGNALS:

            void titleChanged(const QString &new_title);

            void dataChanged(const QJsonObject &data);

        private:
            MenuController menu_controller_;
            WorkspaceController workspace_controller_;
            DataController data_controller_;

            models::MenuModel *menu_model_{nullptr};
            models::WorkspaceModel *workspace_model_{nullptr};
            models::DataModel *data_model_{nullptr};

            DataProvider data_provider_;

            QMetaObject::Connection workspace_created_connection_;


        private Q_SLOTS:

            void openWorkspace();

            void handleNewWorkspaceClicked();

            void handleSaveWorkspaceClicked();

            void handleSaveWorkspaceAsClicked(const QUrl &url);

            void handleOpenWorkspaceClicked(const QUrl &url);

            void handleSettingChange(const QJsonObject &data);

            void pushSettingChange(const QJsonObject &data);

            void handleTitleChanged();

        };
    }
}