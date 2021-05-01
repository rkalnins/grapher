//
// Created by Roberts Kalnins on 01/05/2021.
//

#include "file_handler.h"

namespace grapher {

    FileHandler::FileHandler(QObject *parent) : QObject(parent) {}

    QString FileHandler::getFileName() const {

        const auto path = getLocalPath(file_url_);
        const auto name = QFileInfo(path).fileName();

        if (name.isEmpty()) {
            return QStringLiteral("untitled");
        }

        return name;
    }

    QString FileHandler::getFileType() const {
        return QFileInfo(getFileName()).suffix();
    }

    QUrl FileHandler::getFileUrl() const {
        return file_url_;
    }

    QString FileHandler::getData() const {
        QFile file(getLocalPath(file_url_));

        if (!file.open(QFile::ReadOnly)) {
            return "";
        }

        return file.readAll();
    }

    void FileHandler::load(const QUrl &file_url) {
        if (file_url == file_url_) {
            return;
        }

        file_url_ = file_url;
        emit fileUrlChanged();

        const auto name = getLocalPath(file_url_);

        if (!QFile::exists(name)) {
            emit error(tr("File not found"));
            return;
        }

        QFile file(name);
        if (!file.open(QFile::ReadOnly)) {
            emit error(tr("Failed to open file"));
        }

        emit fileOpened();
    }

    bool FileHandler::saveAs(const QUrl &file_url, const QString &data) {
        const auto path = getLocalPath(file_url);

        QFile file(path);

        if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text)) {
            emit error((tr("Cannot save: ") + file.errorString()));
            return false;
        }

        QTextStream stream(&file);
        stream << data;

        if (file_url == file_url_) {
            return true;
        }

        file_url_ = file_url;
        emit fileUrlChanged();

        return true;
    }

    bool FileHandler::save(const QString &data) {
        return saveAs(file_url_, data);
    }

    QString FileHandler::getLocalPath(const QUrl &file_url) {
        const auto path = file_url.toLocalFile();
        if (!path.isEmpty()) {
            return path;
        }

        return file_url.path();
    }


}