//
// Created by Roberts Kalnins on 01/05/2021.
//

#include "workspace_model.h"


namespace grapher::models {

WorkspaceModel::WorkspaceModel ( QObject *parent ) : QStandardItemModel(
        parent) {
    connect(workspace_handler_.get(), &WorkspaceHandler::workspaceUpdated,
            this,
            &WorkspaceModel::notifyWorkspaceUpdated);
}

WorkspaceHandler *WorkspaceModel::getWorkspace () const {
    return workspace_handler_.get();
}

QString WorkspaceModel::getWorkspaceName () const {
    return workspace_handler_->getWorkspaceName();
}

QUrl WorkspaceModel::getWorkspaceUrl () const {
    return workspace_handler_->getWorkspaceUrl();
}

bool WorkspaceModel::isWorkspaceNew () const {
    return workspace_handler_->isNewWorkspace();
}

bool WorkspaceModel::needsUpdating () const {
    return workspace_handler_->needsUpdating();
}

bool WorkspaceModel::needsSaving () const {
    return workspace_handler_->needsSaving();
}

void WorkspaceModel::setNeedsUpdating () {
    workspace_handler_->setNeedsUpdating(true);
}

void WorkspaceModel::save () {
    workspace_handler_->save();
}

void WorkspaceModel::saveAs ( const QUrl &file_url ) {
    workspace_handler_->saveAs(file_url);
}

void WorkspaceModel::newWorkspace () {
    if ( workspace_handler_ ) {
        workspace_handler_->save();
    }

    workspace_handler_ = std::make_unique< WorkspaceHandler >();
    workspace_handler_->load(QUrl("defaults.json"));
    emit workspaceCreated();
    emit workspaceUpdated(workspace_handler_->getWorkspaceData());
}

void WorkspaceModel::openWorkspace ( const QUrl &url ) {
    if ( workspace_handler_ ) {
        workspace_handler_->save();
    }

    workspace_handler_ = std::make_unique< WorkspaceHandler >();
    workspace_handler_->load(url);
    emit workspaceCreated();
    emit workspaceUpdated(workspace_handler_->getWorkspaceData());
}

void WorkspaceModel::closeWorkspace () {
    qDebug() << "File closed";
}

void WorkspaceModel::setData ( const QJsonObject &data ) {
    workspace_handler_->setWorkspaceData(data);
}

void WorkspaceModel::notifyWorkspaceUpdated ( const QJsonObject &data ) {
    qDebug() << "notifying data set";
    emit workspaceUpdated(data);
    qDebug() << "notified data set";
}
}