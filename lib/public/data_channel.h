//
// Created by Roberts Kalnins on 08/05/2021.
//

#pragma once

#include <QObject>
#include <QJsonObject>

#include "globals.h"

namespace grapher {

    class DataHandler;

    class GRAPHER_EXPORT DataChannel : public QObject {
    Q_OBJECT

    public:
        explicit DataChannel(QJsonObject data, QObject *parent = nullptr);

        double get() const;

        void set(double data);

        const QString &getName() const;

        void setHandler(DataHandler *handler);

        const QString &getIdentifier() const;

    private:
        double data_{};

        DataHandler *handler_{nullptr};

        QString identifier_;

        QString name_;

        QMutex mutex_;
    };
}