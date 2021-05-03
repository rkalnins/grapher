//
// Created by Roberts Kalnins on 01/05/2021.
//

#pragma once

#include <QObject>
#include <QUrl>
#include <QJsonObject>
#include <QDebug>
#include <QPointer>

#include "file_handler.h"

#include "globals.h"

namespace grapher {

    class GRAPHER_EXPORT WorkspaceHandler : public QObject {
    Q_OBJECT

        Q_DISABLE_COPY_MOVE(WorkspaceHandler)

        Q_PROPERTY(QString workspaceName READ getWorkspaceName)
        Q_PROPERTY(QUrl workspaceUrl READ getWorkspaceUrl NOTIFY workspaceUrlChanged)
        Q_PROPERTY(QJsonObject workspaceData READ getWorkspaceData WRITE setWorkspaceData)
        Q_PROPERTY(bool needsUpdating READ needsUpdating WRITE setNeedsUpdating NOTIFY needsUpdatingChanged)
        Q_PROPERTY(bool needsSaving READ needsSaving WRITE setNeedsSaving NOTIFY needsSavingChanged)
        Q_PROPERTY(bool isNewWorkspace READ isNewWorkspace WRITE setIsNewWorkspace NOTIFY isNewWorkspaceChanged)

    public:
        explicit WorkspaceHandler(QObject *parent = nullptr);


        QString getWorkspaceName() const;

        QUrl getWorkspaceUrl() const;

        QJsonObject getWorkspaceData() const;

        bool needsUpdating() const;

        void setNeedsUpdating(bool needs_updating);

        bool needsSaving() const;

        void setNeedsSaving(bool needs_saving);

        bool isNewWorkspace() const;

        void setIsNewWorkspace(bool is_new);

    public Q_SLOTS:

        void load(const QUrl &url);

        void saveAs(const QUrl &url);

        void save();

        void setWorkspaceData(const QJsonObject &data);

    Q_SIGNALS:

        void workspaceUrlChanged();

        void workspaceOpened();

        void error(const QString &message);

        void workspaceUpdated();

        void needsUpdatingChanged();

        void isNewWorkspaceChanged();

        void needsSavingChanged();

    private:
        FileHandler file_handler_;
        QJsonObject data_;
        bool needs_updating_{false};
        bool is_new_workspace_{true};
        bool needs_saving_{false};

    private Q_SLOTS:

        void onWorkspaceOpened();

    };
}
Q_DECLARE_METATYPE(grapher::WorkspaceHandler *)
