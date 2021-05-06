//
// Created by Roberts Kalnins on 05/05/2021.
//

#include "data_model.h"
#include "data_handler.h"

namespace grapher::models {

    DataModel::DataModel(QObject *parent) : QAbstractTableModel(parent) {
    }

    void DataModel::setup() {
        setHeaderData(0, Qt::Horizontal, tr("Name"));
    }

    int DataModel::columnCount(const QModelIndex &parent) const {
        if (parent.isValid()) {
            qDebug() << "parent col is valid";
            return 0;
        }
        return column_count_;
    }

    int DataModel::rowCount(const QModelIndex &parent) const {
        if (parent.isValid()) {
            qDebug() << "parent row is valid";
            return 0;
        }
        return data_handlers_.size();
    }

    QVariant DataModel::data(const QModelIndex &index, int role) const {
        if (data_handlers_.empty()) {
            qDebug() << "no handlers";
            return QVariant();
        }

        if (!index.isValid()) {
            return QVariant();
        }

        int row = index.row();
        int col = index.column();

        if (data_handlers_[row].get() == nullptr) {
            qDebug() << "data handler null";
            return QVariant();
        }

        if (role == Qt::DisplayRole) {
            switch (col) {
                case 0:
                    return data_handlers_[row]->getName();
                default:
                    return QVariant();
            }
        }

        return QVariant();
    }

    void DataModel::addDataHandler(DataHandler &handler) {
        data_handlers_.push_back(std::make_unique<DataHandler>(&handler));
    }

    int DataModel::getNameFromIndex(const QString &name) const {
        for (int i = 0; i < data_handlers_.size(); ++i) {
            if (data_handlers_[i]->getName() == name) {
                return i;
            }
        }

        return -1;
    }

    DataHandler *DataModel::getDataHandler(const QString &name) const {
        int idx = getNameFromIndex(name);

        if (idx < 0) {
            return nullptr;
        }

        return data_handlers_[idx].get();
    }

    std::vector<double> DataModel::getStreamData() {
        std::vector<double> data;
        data.reserve(data_handlers_.size());
        for (const auto &handler : data_handlers_) {
            data.push_back(handler->get());
        }

        return data;
    }

    void DataModel::setDataFromJson(const QJsonObject &data) {
        data_ = data["grapher"].toObject();

        QJsonArray graphs = data_["graphs"].toArray();
        qDebug() << "setting data model data";

        int i = 0;

        for (const auto &graph : graphs) {
            auto handler = std::make_unique<DataHandler>();
            QJsonObject graph_properties = graph.toObject();
            handler->setup(graph_properties);
            data_handlers_.push_back(std::move(handler));
            qDebug() << "added handler " << data_handlers_[i++]->getName();
        }
    }


}