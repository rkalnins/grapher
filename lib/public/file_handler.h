//
// Created by Roberts Kalnins on 01/05/2021.
//

#pragma once

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>


#include "globals.h"

namespace grapher {

    class GRAPHER_EXPORT FileHandler : public QObject {
    Q_OBJECT

        Q_DISABLE_COPY_MOVE(FileHandler);

        Q_PROPERTY(QString fileName READ getFileName NOTIFY fileUrlChanged)
        Q_PROPERTY(QString fileType READ getFileType NOTIFY fileUrlChanged)
        Q_PROPERTY(QUrl fileUrl READ getFileUrl NOTIFY fileUrlChanged)
        Q_PROPERTY(QByteArray data READ getData)

    public:
        explicit FileHandler(QObject *parent = nullptr);

        QString getFileName() const;

        QString getFileType() const;

        QUrl getFileUrl() const;

        QByteArray getData() const;


    public Q_SLOTS:

        void load(const QUrl &file_url);

        bool saveAs(const QUrl &file_url, const QByteArray &data);

        bool save(const QByteArray &data);

    Q_SIGNALS:

        void fileUrlChanged();

        void fileOpened();

        void error(const QString &message);

    private:
        QUrl file_url_;

        static QString getLocalPath(const QUrl &file_url);
    };


}