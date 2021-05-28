//
// Created by Roberts Kalnins on 07/05/2021.
//


#include <QColorDialog>

#include "config_table_view.h"


ConfigTableView::ConfigTableView ( QWidget *parent ) : QTableView(parent) {
    connect(this, &QTableView::doubleClicked, this,
            &ConfigTableView::handleDoubleClick);
}

void ConfigTableView::setup ( grapher::models::DataModel *model ) {
    model_ = model;
}

void ConfigTableView::handleDoubleClick ( QModelIndex index ) {
    if ( index.column() == 1 ) {
        auto   current_color = index.model()->data(index,
                                                   Qt::BackgroundRole).value< QColor >();
        QColor new_color     = QColorDialog::getColor(current_color, this,
                                                      "Pick graph color",
                                                      QColorDialog::DontUseNativeDialog);

        if ( new_color.isValid()) {
            model()->setData(index, new_color, Qt::BackgroundRole);
        }
    }

    qDebug() << "updating " << index.row();
    emit settingsUpdated(index.row(), model_->getDataHandler(index.row()));
}