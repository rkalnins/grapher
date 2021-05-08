//
// Created by Roberts Kalnins on 08/05/2021.
//

#include "abstract_data_provider_model.h"

namespace grapher::models {
    QVariant AbstractDataProviderModel::data(const QModelIndex &index, int role) const {
        if (role == Qt::DisplayRole) {
            return channels_[index.row()]->getName();
        }

        return QVariant();
    }

    DataChannel * AbstractDataProviderModel::getChannelById(const QString &id) {
        for (auto channel : channels_) {
            if (channel->getIdentifier() == id) {
                return channel;
            }
        }

        return nullptr;
    }

    int AbstractDataProviderModel::rowCount(const QModelIndex &parent) const {
        return channels_.size();
    }

    AbstractDataProviderModel::AbstractDataProviderModel(QObject *parent) : QAbstractListModel(parent) {}

    const QString &AbstractDataProviderModel::getName() const {
        return name_;
    }
}