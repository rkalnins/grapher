//
// Created by Roberts Kalnins on 08/05/2021.
//

#include "graph_data_csv_handler.h"

namespace grapher {
    GraphDataCsvHandler::GraphDataCsvHandler(QObject *parent) : QObject(parent) {}

    void GraphDataCsvHandler::exportTo(const QUrl &url, std::vector<QCPDataContainer<QCPGraphData> *> &data) {
        QByteArray byte_array;

        size_t entries = data[0]->size();

        for (size_t i = 0; i < entries; ++i) {
            QString line = QString::number(data[0]->at(i)->key);
            for (const auto &graph : data) {
                line.push_back("," + QString::number(graph->at(i)->value));
            }
            line.push_back("\n");
            byte_array.push_back(line.toStdString().c_str());

        }

        if (!file_handler_.saveAs(url, byte_array)) {
            qDebug() << "Couldn't export csv to file";
            return;
        }

        qDebug() << "Done exporting to" << url.toString();
    }

}
