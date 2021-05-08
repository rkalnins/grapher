//
// Created by Roberts Kalnins on 05/05/2021.
//

#pragma once

#include <QObject>
#include <QPointer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QAbstractTableModel>

#include "data_handler.h"

#include "globals.h"

namespace grapher::models {

    class GRAPHER_EXPORT DataModel : public QAbstractTableModel {
    Q_OBJECT

        Q_DISABLE_COPY_MOVE(DataModel)

    public:

        explicit DataModel(QObject *parent = nullptr);

        virtual ~DataModel() = default;

        void setup();

        int columnCount(const QModelIndex &parent) const override;

        int rowCount(const QModelIndex &parent) const override;

        QVariant data(const QModelIndex &index, int role) const override;

        bool setData(const QModelIndex &index, const QVariant &value, int role) override;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        DataHandler *getDataHandler(const QString &name) const;

        DataHandler *getDataHandler(const int &idx) const;

        std::vector<double> getStreamData();

        void setDataFromJson(const QJsonObject &data);

        int getHandlerCount();

        Qt::ItemFlags flags(const QModelIndex &index) const override;

    public Q_SLOTS:

        void collectSaveGraphsData();

    Q_SIGNALS:

        void collectedGraphData(const QJsonArray &graphs);

    private:

        int getNameFromIndex(const QString &name) const;

        std::vector<std::unique_ptr<DataHandler>> data_handlers_;

        int column_count_ = 5;

    };
}