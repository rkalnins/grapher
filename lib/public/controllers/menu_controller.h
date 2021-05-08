//
// Created by Roberts Kalnins on 30/04/2021.
//

#pragma once

#include <QObject>
#include <QDebug>

#include "qcustomplot.h"

#include "globals.h"


namespace grapher {
    namespace models {
        class MenuModel;
    }

    class WorkspaceHandler;

    namespace controllers {

        class GRAPHER_EXPORT MenuController : public QObject {
        Q_OBJECT

            Q_DISABLE_COPY_MOVE(MenuController)

        public:
            explicit MenuController(QObject *parent = nullptr);

            virtual ~MenuController() = default;

            void setModel(models::MenuModel &model);

            void setWorkspace(WorkspaceHandler &handler);

        Q_SIGNALS:

            void newWorkspaceClicked();

            void saveWorkspaceClicked();

            void saveWorkspaceAsClicked(const QUrl &url);

            void openWorkspaceClicked(const QUrl &url);

            void exportDataClicked(const QUrl &url, std::vector<QCPDataContainer<QCPGraphData> *> &data);

        private:
            models::MenuModel *menu_model_{nullptr};

        };

    }

}
