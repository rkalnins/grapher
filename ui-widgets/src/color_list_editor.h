//
// Created by Roberts Kalnins on 07/05/2021.
//

#pragma once

#include <QObject>
#include <QComboBox>

#include "data_handler.h"

class ColorListEditor : public QComboBox {
Q_OBJECT
    Q_PROPERTY(QColor color READ getColor WRITE setColor USER true)

public:
    explicit ColorListEditor(QWidget *parent = nullptr);

public:
    QColor getColor() const;

    void setHandler(grapher::DataHandler *handler);

    void setColor(const QColor &color);

public Q_SLOTS:

    void setColorInHandler(int idx);

private:
    void populateList();

    grapher::DataHandler *handler_{nullptr};
};