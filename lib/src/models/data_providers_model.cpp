//
// Created by Roberts Kalnins on 08/05/2021.
//

#include "data_providers_model.h"

#include <QJsonArray>
#include <QJsonObject>

#include "test_provider.h"

namespace grapher::models {
    void DataProvidersModel::populateProviders(const QJsonArray &providers) {
        reset();
        qDebug() << providers;
        for (const auto &p : providers) {
            QString type = p.toObject()["type"].toString();
            if (type == "udp") {

            } else if (type == "test") {
                AbstractDataProviderModel *test_p = new providers::TestProvider();
                test_p->setup(p.toObject());
                test_p->start();
                providers_.push_back(test_p);
                qDebug() << "added provider " << test_p->getName();
            }
        }
    }

    void DataProvidersModel::reset() {
        providers_.erase(providers_.cbegin(), providers_.cend());
        providers_.clear();
    }

    int DataProvidersModel::rowCount(const QModelIndex &parent) const {
        return providers_.size();
    }

    bool DataProvidersModel::setData(const QModelIndex &index, const QVariant &value, int role) {
        return QAbstractItemModel::setData(index, value, role);
    }

    QVariant DataProvidersModel::data(const QModelIndex &index, int role) const {
        if (role == Qt::DisplayRole) {
            return providers_.at(index.row())->getName();
        }

        return QVariant();
    }

    const QList<AbstractDataProviderModel *> &DataProvidersModel::getProviders() const {
        return providers_;
    }

    AbstractDataProviderModel *DataProvidersModel::getProviderByName(const QString &name) {
        for (const auto &item : providers_) {
            if (item->getName() == name) {
                return item;
            }
        }
        return nullptr;
    }
}