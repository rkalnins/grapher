//
// Created by Roberts Kalnins on 30/04/2021.
//

#include "menu_model.h"


namespace grapher::models {

    MenuModel::MenuModel(QObject *parent) : QObject{parent} {}

    QString MenuModel::getTitle() const {
        return QString("test-title");
    }

    bool MenuModel::isNewWorkspace() const {
        return false;
    }

}