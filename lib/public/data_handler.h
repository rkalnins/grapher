//
// Created by Roberts Kalnins on 05/05/2021.
//

#pragma once

#include <QObject>

#include "globals.h"

namespace grapher {

    class GRAPHER_EXPORT DataHandler : public QObject {
        Q_OBJECT

        Q_PROPERTY(QString name READ getName CONSTANT)

    public:
        explicit DataHandler(QObject *parent = nullptr) : QObject(parent) {}

        double get() const;

        QString getName() const;

        void setup(QJsonObject &data);

    private:
        QString name_;

    };

}
