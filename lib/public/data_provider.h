//
// Created by Roberts Kalnins on 05/05/2021.
//

#pragma once

#include <QObject>

#include "globals.h"


namespace grapher {
    namespace models {
        class DataModel;
    }

    class GRAPHER_EXPORT DataProvider : public QObject {
    Q_OBJECT

    public:
        explicit DataProvider(QObject *parent = nullptr) : QObject(parent) {}

        void setModel(models::DataModel *model);

        std::vector<double> getData();

    private:
        models::DataModel *model_{nullptr};

    };

}
