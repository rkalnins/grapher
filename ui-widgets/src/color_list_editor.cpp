//
// Created by Roberts Kalnins on 07/05/2021.
//

#include "color_list_editor.h"

ColorListEditor::ColorListEditor(QWidget *parent) : QComboBox(parent) {
    connect(this, &QComboBox::activated, this, &ColorListEditor::setColorInHandler);
    populateList();
}

QColor ColorListEditor::getColor() const {
    return qvariant_cast<QColor>(itemData(currentIndex(), Qt::DecorationRole));
}

void ColorListEditor::setColor(const QColor &color) {
    setCurrentIndex(findData(color, Qt::DecorationRole));
}

void ColorListEditor::setColorInHandler(int idx) {
    handler_->setPenColor(qvariant_cast<QColor>(itemData(idx, Qt::DecorationRole)));
}

void ColorListEditor::populateList() {
    const QStringList colorNames = QColor::colorNames();

    for (int i = 0; i < colorNames.size(); ++i) {
        QColor color(colorNames[i]);

        insertItem(i, colorNames[i]);
        setItemData(i, color, Qt::DecorationRole);
    }
}

void ColorListEditor::setHandler(grapher::DataHandler *handler) {
    handler_ = handler;
}
