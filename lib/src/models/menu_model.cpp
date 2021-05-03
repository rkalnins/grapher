//
// Created by Roberts Kalnins on 30/04/2021.
//

#include "menu_model.h"
#include "workspace_handler.h"


namespace grapher::models {

    MenuModel::MenuModel(QObject *parent) : QObject{parent} {}

    void MenuModel::setWorkspace(WorkspaceHandler &handler) {
        if (workspace_handler_ == &handler) {
            return;
        }

        if (workspace_handler_) {
            disconnect(workspace_handler_.data(),
                       &WorkspaceHandler::workspaceUrlChanged,
                       this,
                       &MenuModel::titleChanged);
            disconnect(workspace_handler_.data(),
                       &WorkspaceHandler::isNewWorkspaceChanged,
                       this,
                       &MenuModel::isNewWorkspaceChanged);
        }

        workspace_handler_ = &handler;
        if (!workspace_handler_) {
            return;
        }

        connect(workspace_handler_.data(),
                &WorkspaceHandler::workspaceUrlChanged,
                this,
                &MenuModel::titleChanged);
        connect(workspace_handler_.data(),
                &WorkspaceHandler::isNewWorkspaceChanged,
                this,
                &MenuModel::isNewWorkspaceChanged);

        emit titleChanged();
        emit isNewWorkspaceChanged();

        qDebug() << "set workspace to " << workspace_handler_->getWorkspaceName();

    }

    QString MenuModel::getTitle() const {

        if (!workspace_handler_) {
            return {};
        }
        return workspace_handler_->getWorkspaceName();
    }

    bool MenuModel::isNewWorkspace() const {
        if (!workspace_handler_) {
            return false;
        }

        return workspace_handler_->isNewWorkspace();
    }

}