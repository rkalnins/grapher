//
// Created by Roberts Kalnins on 01/05/2021.
//

#include "workspace_handler.h"

namespace grapher {

    WorkspaceHandler::WorkspaceHandler(QObject *parent) : QObject(parent) {
        connect(&file_handler_, &FileHandler::fileUrlChanged, this, &WorkspaceHandler::workspaceUrlChanged);
        connect(&file_handler_, &FileHandler::error, this, &WorkspaceHandler::error);
        connect(&file_handler_, &FileHandler::fileOpened, this, &WorkspaceHandler::onWorkspaceOpened);
    }

    QString WorkspaceHandler::getWorkspaceName() const {
        return data_["name"].toString();
    }

    QUrl WorkspaceHandler::getWorkspaceUrl() const {
        return file_handler_.getFileUrl();
    }

    QJsonObject WorkspaceHandler::getWorkspaceData() const {
        return data_;
    }

    bool WorkspaceHandler::needsUpdating() const {
        return needs_updating_;
    }

    void WorkspaceHandler::setNeedsUpdating(bool needs_updating) {
        if (needs_updating == needs_updating_) {
            return;
        }

        if (needs_updating) {
            setNeedsUpdating(true);
        }

        needs_updating_ = needs_updating;
        emit needsUpdatingChanged();
    }

    bool WorkspaceHandler::needsSaving() const {
        return needs_saving_;
    }

    bool WorkspaceHandler::isNewWorkspace() const {
        return is_new_workspace_;
    }

    void WorkspaceHandler::load(const QUrl &url) {
        file_handler_.load(url);

        qDebug() << "Done loading";
    }

    void WorkspaceHandler::saveAs(const QUrl &url) {
        if (!file_handler_.saveAs(url, QJsonDocument(data_).toJson())) {
            qDebug() << "Couldn't save to file";
            return;
        }

        setIsNewWorkspace(false);
        setNeedsSaving(false);
        setNeedsUpdating(false);

        qDebug() << "Done saving as" << url.toString();
    }

    void WorkspaceHandler::save() {
        if (is_new_workspace_) {
            return;
        }

        if (!file_handler_.save(QJsonDocument(data_).toJson())) {
            return;
        }

        setNeedsSaving(false);
        setNeedsUpdating(false);

        qDebug() << "Done saving";
    }

    void WorkspaceHandler::setWorkspaceData(const QJsonObject &data) {
        if (!needs_updating_) {
            return;
        }

        data_ = data;
        setNeedsUpdating(false);


        qDebug() << "notifying workspace data";
        emit workspaceUpdated(data_);
        qDebug() << "notified workspace data";
        qDebug() << QJsonDocument(data_).toJson(QJsonDocument::Compact);

    }

    void WorkspaceHandler::setIsNewWorkspace(bool is_new) {
        if (is_new_workspace_ == is_new) {
            return;
        }

        is_new_workspace_ = is_new;
        emit isNewWorkspaceChanged();
    }

    void WorkspaceHandler::setNeedsSaving(bool needs_saving) {
        if (needs_saving_ == needs_saving) {
            return;
        }

        needs_saving_ = needs_saving;
        emit needsSavingChanged();
    }

    void WorkspaceHandler::onWorkspaceOpened() {
        needs_updating_ = true;

        QJsonDocument doc = QJsonDocument::fromJson(file_handler_.getData());

        if (doc.isNull() || !doc.isObject()) {
            qWarning() << "json data invalid";
            return;
        }

        setWorkspaceData(doc.object());
        needs_updating_ = false;
        setIsNewWorkspace(false);
        emit workspaceOpened();
        qDebug() << "done opening workspace";
    }


}