//
// Created by Roberts Kalnins on 08/05/2021.
//

#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QList>

#include "data_channel.h"

#include "globals.h"

namespace grapher {

    class DataChannel;

    namespace models {
        class GRAPHER_EXPORT AbstractDataProviderModel : public QAbstractListModel {

        public:

            AbstractDataProviderModel(QObject *parent = nullptr);

            QVariant data(const QModelIndex &index, int role) const override;

            int rowCount(const QModelIndex &parent) const override;

            DataChannel *getChannelById(const QString &id);

            virtual void populateChannels(QJsonArray channels) = 0;

            virtual void start() = 0;

            virtual void setup(QJsonObject provider_data) = 0;

            const QString &getName() const;


        protected:
            QList<DataChannel *> channels_;

            QString name_;


        };
    }

}