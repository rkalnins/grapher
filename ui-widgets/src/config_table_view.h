//
// Created by Roberts Kalnins on 07/05/2021.
//

#pragma once

#include <QObject>
#include <QTableView>

#include "data_model.h"


class ConfigTableView : public QTableView {
Q_OBJECT

public:
    explicit ConfigTableView(QWidget *parent);

    void setup(grapher::models::DataModel *model);

Q_SIGNALS:

    void settingsUpdated(int idx, grapher::DataHandler *handler);

public Q_SLOTS:

    void handleDoubleClick(QModelIndex index);

private:
    grapher::models::DataModel *model_;
};


