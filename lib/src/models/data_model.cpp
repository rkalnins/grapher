//
// Created by Roberts Kalnins on 05/05/2021.
//

#include "data_model.h"
#include "data_handler.h"

namespace grapher::models {

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

    std::vector<double> DataModel::getData() {
        std::vector<double> data;
        data.reserve(data_handlers_.size());
        for (const auto& handler : data_handlers_) {
            data.push_back(handler->get());
        }

        return data;
    }

    void DataModel::setDataFromJson(const QJsonObject &data) {
        data_ = data["grapher"].toObject();

        QJsonArray graphs = data_["graphs"].toArray();
        qDebug() << "setting data model data";

        for (const auto& graph : graphs) {
            DataHandler handler;
            QJsonObject graph_properties = graph.toObject();
            handler.setup(graph_properties);
            addDataHandler(handler);
            qDebug() << "added handler " << handler.getName();
        }
    }



}