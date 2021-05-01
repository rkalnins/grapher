//
// Created by Roberts Kalnins on 30/04/2021.
//

#pragma once

#include <QObject>

#include "globals.h"

namespace grapher::models {
    class GRAPHER_EXPORT MenuModel : public QObject {
    Q_OBJECT

        Q_DISABLE_COPY_MOVE(MenuModel)

        Q_PROPERTY(QString title READ title NOTIFY titleChanged)
        Q_PROPERTY(bool isNewWorkspace READ isNewWorkspace NOTIFY isNewWorkspaceChanged)

    public:

        explicit MenuModel(QObject *parent = nullptr);

        virtual ~MenuModel() = default;

        QString title() const;

        bool isNewWorkspace() const;

    Q_SIGNALS:

        void titleChanged();

        void isNewWorkspaceChanged();
    };
}
