//
// Created by Roberts Kalnins on 05/05/2021.
//

#include "data_model.h"
#include "data_handler.h"


namespace grapher::models {

    DataModel::DataModel(QObject *parent) : QAbstractTableModel(parent) {
    }

    void DataModel::setup() {
        qDebug() << "table setup";
        setHeaderData(0, Qt::Horizontal, tr("Name"));
        setHeaderData(1, Qt::Horizontal, tr("Color"));
    }

    int DataModel::getHandlerCount() {
        return data_handlers_.size();
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
                case 1:
                    // get background color instead
                    return QVariant();
                default:
                    return QVariant();
            }
        } else if (role == Qt::BackgroundRole) {
            switch (col) {
                case 1:
                    return data_handlers_[row]->getPenColor();
                default:
                    return QVariant();
            }
        }

        return QVariant();
    }


    int DataModel::getNameFromIndex(const QString &name) const {
        for (int i = 0; i < data_handlers_.size(); ++i) {
            if (data_handlers_[i]->getName() == name) {
                return i;
            }
        }

        return -1;
    }

    DataHandler *DataModel::getDataHandler(const int &idx) const {
        if (idx < 0) {
            return nullptr;
        }

        return data_handlers_[idx].get();
    }

    DataHandler *DataModel::getDataHandler(const QString &name) const {
        return getDataHandler(getNameFromIndex(name));
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
        data_handlers_.erase(data_handlers_.begin(), data_handlers_.end());
        data_handlers_.clear();
        data_ = data["grapher"].toObject();

        QJsonArray graphs = data_["graphs"].toArray();
        qDebug() << "setting data model data";

        int i = 0;
        data_handlers_.reserve(graphs.size());

        for (const auto &graph : graphs) {
            auto handler = std::make_unique<DataHandler>();
            QJsonObject graph_properties = graph.toObject();
            handler->setup(graph_properties);
            data_handlers_.push_back(std::move(handler));
            qDebug() << "added handler " << data_handlers_[i++]->getName();
        }
    }

    QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            switch (section) {
                case 0:
                    return QString("Name");
                case 1:
                    return QString("Color");

            }
        }

        return QVariant();
    }

    Qt::ItemFlags DataModel::flags(const QModelIndex &index) const {
        if (index.column() == 1) {
            return Qt::ItemIsEnabled;
        } else {
            return QAbstractTableModel::flags(index) | Qt::ItemIsEnabled;
        }
    }

    bool DataModel::setData(const QModelIndex &index, const QVariant &value, int role) {

        if (index.row() == 1) {
            data_handlers_[index.row()]->setPenColor(qvariant_cast<QColor>(value));
        }

        return QAbstractItemModel::setData(index, value, role);
    }


}