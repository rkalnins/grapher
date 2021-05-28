//
// Created by Roberts Kalnins on 01/05/2021.
//

#pragma once

#include <QStandardItemModel>
#include <QObject>

#include <memory>

#include "workspace_handler.h"

#include "globals.h"


namespace grapher {

class WorkspaceHandler;

namespace models {

class GRAPHER_EXPORT WorkspaceModel : public QStandardItemModel {
  Q_OBJECT

    Q_DISABLE_COPY_MOVE(WorkspaceModel)


  public:

    explicit WorkspaceModel ( QObject *parent = nullptr );

    WorkspaceHandler *getWorkspace () const;

    QString getWorkspaceName () const;

    QUrl getWorkspaceUrl () const;

    bool isWorkspaceNew () const;

    bool needsUpdating () const;

    bool needsSaving () const;

    void setNeedsUpdating ();

    void save ();

    void saveAs ( const QUrl &file_url );

    void setData ( const QJsonObject &data );

  public Q_SLOTS:

    void newWorkspace ();

    void openWorkspace ( const QUrl &url );

    void closeWorkspace ();

    void notifyWorkspaceUpdated ( const QJsonObject &data );

  Q_SIGNALS:

    void workspaceCreated ();

    void workspaceUpdated ( const QJsonObject &data );

  private:

    std::unique_ptr< WorkspaceHandler > workspace_handler_;

};
}
}