//
// Created by Roberts Kalnins on 30/04/2021.
//

#pragma once

#include <QObject>
#include <QString>
#include <QDebug>

#include "menu_controller.h"

#include "globals.h"


namespace grapher::controllers {
    class GRAPHER_EXPORT MainController : public QObject {
    Q_OBJECT

        Q_DISABLE_COPY_MOVE(MainController)

        Q_PROPERTY(controllers::MenuController * menuController READ menuController CONSTANT);

    public:
        explicit MainController(QObject *parent = nullptr);

        MenuController *menuController();

        virtual ~MainController() = default;

    private:
        MenuController menu_controller_;

    private Q_SLOTS:

        void handleNewWorkspaceClicked();

        void handleSaveWorkspaceClicked();

        void handleSaveWorkspaceAsClicked();

        void handleOpenWorkspaceClicked();

    };
}
