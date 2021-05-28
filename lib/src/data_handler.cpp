//
// Created by Roberts Kalnins on 05/05/2021.
//

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>

#include "data_handler.h"


namespace grapher {
void DataHandler::setup ( QJsonObject &data ) {
    name_    = data["name"].toString();
    visible_ = data["visible"].toBool();

    QJsonArray pen_color = data["pen"].toArray();
    pen_color_.setRed(pen_color[0].toInt());
    pen_color_.setGreen(pen_color[1].toInt());
    pen_color_.setBlue(pen_color[2].toInt());

    id_       = data["id"].toString();
    provider_ = data["provider"].toString();
}

const QString &DataHandler::getId () const {
    return id_;
}

double DataHandler::get () const {
    return channel_->get();
}

const QString &DataHandler::getName () const {
    return name_;
}

const QColor &DataHandler::getPenColor () const {
    return pen_color_;
}

void DataHandler::setName ( const QString &name ) {
    name_ = name;
}

void DataHandler::setPenColor ( const QColor &penColor ) {
    pen_color_ = penColor;
}

bool DataHandler::isVisible () const {
    return visible_;
}

void DataHandler::setIsVisible ( bool visibility ) {
    visible_ = visibility;
}

void DataHandler::setChannel ( DataChannel *channel ) {
    channel_ = channel;
}

DataChannel *DataHandler::getChannel () const {
    return channel_;
}

const QString &DataHandler::getProvider () const {
    return provider_;
}
}