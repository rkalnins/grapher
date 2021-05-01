//
// Created by Roberts Kalnins on 30/04/2021.
//



#include "menu_controller.h"
#include "menu_model.h"

namespace grapher::controllers {

    MenuController::MenuController(QObject *parent) : QObject{parent} {

    }

    void MenuController::setModel(models::MenuModel &model) {
        menu_model_ = &model;
    }

}