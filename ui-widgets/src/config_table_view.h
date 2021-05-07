//
// Created by Roberts Kalnins on 07/05/2021.
//

#pragma once

#include <QObject>
#include <QTableView>

#include "data_model.h"


class ConfigTableView : public QTableView {

public:
    ConfigTableView(QWidget *parent);

    void setup(grapher::models::DataModel *model);

public Q_SLOTS:

    void handleDoubleClick(QModelIndex index);
};


