//
// Created by Roberts Kalnins on 05/05/2021.
//

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>


#include "data_handler.h"

namespace grapher {
    void DataHandler::setup(QJsonObject &data) {
        name_ = data["name"].toString();

        QJsonArray pen_color = data["pen"].toArray();
        pen_color_.setRed(pen_color[0].toInt());
        pen_color_.setGreen(pen_color[1].toInt());
        pen_color_.setBlue(pen_color[2].toInt());
    }

    double DataHandler::get() const {
#ifdef HANDLER_PROVIDE_TEST_DATA
        static int test = 0;
        ++test;
        return qSin(test) + std::rand() / (double) RAND_MAX * 1 * qSin(test / 0.3843);
#else
        return current_data_;
#endif
    }

    const QString &DataHandler::getName() const {
        return name_;
    }

    const QColor &DataHandler::getPenColor() const {
        return pen_color_;
    }

    void DataHandler::setName(const QString &name) {
        name_ = name;
    }

    void DataHandler::setPenColor(const QColor &penColor) {
        pen_color_ = penColor;
    }
}