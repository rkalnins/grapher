//
// Created by Roberts Kalnins on 08/05/2021.
//

#include "data_handler.h"
#include "data_channel.h"

namespace grapher {

    DataChannel::DataChannel(QJsonObject data, QObject *parent)
            : QObject(parent), identifier_(data["id"].toString()), name_(data["name"].toString()) {}

    void DataChannel::setHandler(DataHandler *handler) {
        handler_ = handler;
    }

    const QString &DataChannel::getName() const {
        return name_;
    }

    double DataChannel::get() const {
        return data_;
    }

    void DataChannel::set(double data) {
        mutex_.lock();
        data_ = data;
        mutex_.unlock();
    }

    const QString &DataChannel::getIdentifier() const {
        return identifier_;
    }

}