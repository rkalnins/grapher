//
// Created by Roberts Kalnins on 05/05/2021.
//

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include "data_handler.h"

namespace grapher {
    double DataHandler::get() const {
        static int test = 0;
        ++test;
        return qSin(test) + std::rand() / (double) RAND_MAX * 1 * qSin(test / 0.3843);
    }

    QString DataHandler::getName() const {
        return name_;
    }

    void DataHandler::setup(QJsonObject &data) {
        name_ = data["name"].toString();
    }
}