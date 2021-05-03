//
// Created by Roberts Kalnins on 01/05/2021.
//

#include "workspace_model.h"

namespace grapher::models {

    WorkspaceModel::WorkspaceModel(QObject *parent) : QStandardItemModel(parent) {}

    WorkspaceHandler *WorkspaceModel::getWorkspace() const {
        return workspace_handler_.get();
    }

    QString WorkspaceModel::getWorkspaceName() const {
        return workspace_handler_->getWorkspaceName();
    }

    QUrl WorkspaceModel::getWorkspaceUrl() const {
        return workspace_handler_->getWorkspaceUrl();
    }

    bool WorkspaceModel::isWorkspaceNew() const {
        return workspace_handler_->isNewWorkspace();
    }

    bool WorkspaceModel::needsUpdating() const {
        return workspace_handler_->needsUpdating();
    }

    bool WorkspaceModel::needsSaving() const {
        return workspace_handler_->needsSaving();
    }

    void WorkspaceModel::setNeedsUpdating() {
        workspace_handler_->setNeedsUpdating(true);
    }

    void WorkspaceModel::save() {
        workspace_handler_->save();
    }

    void WorkspaceModel::saveAs(const QUrl &file_url) {
        workspace_handler_->saveAs(file_url);
    }

    void WorkspaceModel::newWorkspace() {
        workspace_handler_ = std::make_unique<WorkspaceHandler>();
        emit workspaceCreated();
    }

    void WorkspaceModel::openWorkspace(const QUrl &url) {
        workspace_handler_ = std::make_unique<WorkspaceHandler>();
        workspace_handler_->load(url);
        emit workspaceCreated();
    }

    void WorkspaceModel::closeWorkspace() {
        qDebug() << "File closed";
    }

    void WorkspaceModel::setData(const QString &key, QVariant &new_data) {
        workspace_handler_->getWorkspaceData()[key] = QJsonValue::fromVariant(new_data);
    }
}