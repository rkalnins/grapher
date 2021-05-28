//
// Created by Roberts Kalnins on 08/05/2021.
//

#ifndef GRAPHER_JSON_UTIL_H
#define GRAPHER_JSON_UTIL_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>


QVariant getValue ( QJsonObject const &settings,
                    QString const &path,
                    QVariant const &defaultValue = QVariant()) {
    QVariant result = defaultValue;

    QStringList pathComponents = path.split("/");
    QJsonObject parentObject   = settings;

    while ( pathComponents.size() > 0 ) {
        QString const &currentKey = pathComponents.takeFirst();

        if ( parentObject.contains(currentKey)) {
            if ( pathComponents.size() == 0 ) {
                result = parentObject.value(currentKey);
            } else {
                parentObject = parentObject.value(currentKey).toObject();
            }
        } else {
            qWarning() << "Settings could not access unknown key"
                       << currentKey << " in " << parentObject;
            break;
        }
    }

    return result;
}

QJsonObject setValue ( QJsonObject const &settings, QString const &path,
                       QVariant const &value ) {
    QStringList pathComponents = path.split("/");

    QVector< QJsonObject > valuesUpToPath { settings };

    //Ensure the path up to the last value exists,
    for ( int i = 0; i < pathComponents.size() - 1; i++ ) {
        QString const &currentKey = pathComponents[i];

        if ( !valuesUpToPath.last().contains(currentKey)) {
            valuesUpToPath.last()[currentKey] = QJsonObject();
        }

        valuesUpToPath.push_back(
                valuesUpToPath.last()[currentKey].toObject());
    }

    //Set the last item's key = value
    valuesUpToPath.last()[pathComponents.last()] = QJsonValue::fromVariant(
            value);

    //Now merge all the items back into one
    QJsonObject result = valuesUpToPath.takeLast();

    while ( valuesUpToPath.size() > 0 ) {
        QJsonObject parent = valuesUpToPath.takeLast();
        parent[pathComponents[valuesUpToPath.size()]] = result;
        result = parent;
    }

    return result;
}

#endif //GRAPHER_JSON_UTIL_H
