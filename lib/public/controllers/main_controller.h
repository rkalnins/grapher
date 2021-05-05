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


#include "globals.h"


namespace grapher {
    namespace models {
        class MenuModel;

        class WorkspaceModel;
    }

    namespace controllers {
        class GRAPHER_EXPORT MainController : public QObject {
        Q_OBJECT

            Q_DISABLE_COPY_MOVE(MainController)

            Q_PROPERTY(controllers::MenuController *menuController READ getMenuController CONSTANT);
            Q_PROPERTY(controllers::WorkspaceController *workspaceController READ getWorkspaceController CONSTANT);

        public:
            explicit MainController(QObject *parent = nullptr);

            MenuController *getMenuController();

            WorkspaceController *getWorkspaceController();

            virtual ~MainController() = default;

            void setWorkspaceModel(models::WorkspaceModel &model);

            void setMenuModel(models::MenuModel &model);

        Q_SIGNALS:

            void titleChanged(const QString &new_title);

            void dataChanged(const QJsonObject &data);

        private:
            MenuController menu_controller_;
            WorkspaceController workspace_controller_;
            models::MenuModel *menu_model_{nullptr};
            models::WorkspaceModel *workspace_model_{nullptr};
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