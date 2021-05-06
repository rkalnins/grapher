//
// Created by Roberts Kalnins on 05/05/2021.
//

#include "data_provider.h"

#include "data_model.h"

namespace grapher {
    void DataProvider::setModel(models::DataModel *model) {
        model_ = model;
    }

    std::vector<double> DataProvider::getData() {
        return model_->getStreamData();
    }

}