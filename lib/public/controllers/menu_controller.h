//
// Created by Roberts Kalnins on 30/04/2021.
//

#pragma once

#include <QObject>

#include "globals.h"


namespace grapher {
    namespace models {
        class MenuModel;
    }

    namespace controllers {

        class GRAPHER_EXPORT MenuController : public QObject {
        Q_OBJECT

            Q_DISABLE_COPY_MOVE(MenuController);

        public:
            explicit MenuController(QObject *parent = nullptr);

            virtual ~MenuController() = default;

            void setModel(models::MenuModel &model);

        Q_SIGNALS:

            void newWorkspaceClicked();

            void saveWorkspaceClicked();

            void saveWorkspaceAsClicked(const QUrl &url);

            void openWorkspaceClicked(const QUrl &url);

        private:
            models::MenuModel *menu_model_{nullptr};

        };

    }

}
