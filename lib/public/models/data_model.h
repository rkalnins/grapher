//
// Created by Roberts Kalnins on 05/05/2021.
//

#pragma once

#include <QObject>
#include <QPointer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "data_handler.h"

#include "globals.h"

namespace grapher::models {

        class GRAPHER_EXPORT DataModel : public QObject {
        Q_OBJECT

            Q_DISABLE_COPY_MOVE(DataModel)

        public:

            explicit DataModel(QObject *parent = nullptr) : QObject(parent) {};

            virtual ~DataModel() = default;

            DataHandler *getDataHandler(const QString &name) const;

            void addDataHandler(DataHandler &handler);

            std::vector<double> getData();

            void setData(const QJsonObject &data);

        private:

            int getNameFromIndex(const QString &name) const;

            std::vector<std::unique_ptr<DataHandler>> data_handlers_;

            QJsonObject data_;

        };
    }