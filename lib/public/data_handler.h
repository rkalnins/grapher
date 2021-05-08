//
// Created by Roberts Kalnins on 05/05/2021.
//

#pragma once

#include <QObject>
#include <QColor>

#include "globals.h"

#define HANDLER_PROVIDE_TEST_DATA

namespace grapher {

    class GRAPHER_EXPORT DataHandler : public QObject {
    Q_OBJECT

        Q_PROPERTY(QString name READ getName WRITE setName USER true)
        Q_PROPERTY(QColor penColor READ getPenColor WRITE setPenColor USER true)
        Q_PROPERTY(bool visible READ isVisible WRITE setIsVisible USER true)

    public:
        explicit DataHandler(QObject *parent = nullptr) : QObject(parent) {}

        double get() const;

        const QString &getName() const;

        const QColor &getPenColor() const;

        void setName(const QString &name);

        void setPenColor(const QColor &penColor);

        void setup(QJsonObject &data);

        bool isVisible() const;

        void setIsVisible(bool visibility);

    private:
        double current_data_{0.0};

        QString name_;
        QColor pen_color_{QColor::Spec::Rgb};
        bool visible_;

    };

}
