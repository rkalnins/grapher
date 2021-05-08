//
// Created by Roberts Kalnins on 08/05/2021.
//

#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QMutex>

#include "abstract_data_provider_model.h"

#include "globals.h"

namespace grapher::models {


    class GRAPHER_EXPORT DataProvidersModel : public QAbstractListModel {
    Q_OBJECT

    public:
        explicit DataProvidersModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

        int rowCount(const QModelIndex &parent) const override;

        QVariant data(const QModelIndex &index, int role) const override;

        void populateProviders(const QJsonArray &providers);

        AbstractDataProviderModel *getProviderByName(const QString &name);

        const QList<AbstractDataProviderModel *> &getProviders() const;

        void reset();

    private:

        QList<AbstractDataProviderModel *> providers_;

    };


}