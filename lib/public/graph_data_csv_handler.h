//
// Created by Roberts Kalnins on 08/05/2021.
//

#pragma once

#include <QObject>
#include <QUrl>

#include "qcustomplot.h"

#include "file_handler.h"

#include "globals.h"

namespace grapher {

    class GRAPHER_EXPORT GraphDataCsvHandler : public QObject {
    Q_OBJECT
    public:
        explicit GraphDataCsvHandler(QObject *parent = nullptr);

    public Q_SLOTS:

        void exportTo(const QUrl &url, std::vector<QCPDataContainer<QCPGraphData> *> &data);

    private:
        FileHandler file_handler_;

    };
}