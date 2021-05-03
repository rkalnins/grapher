//
// Created by Roberts Kalnins on 02/05/2021.
//

#pragma once

#include <QObject>

#include "globals.h"

namespace grapher {
    namespace models {
        class WorkspaceModel;
    }

    namespace controllers {
        class GRAPHER_EXPORT WorkspaceController : public QObject {
        Q_OBJECT

            Q_DISABLE_COPY_MOVE(WorkspaceController)

        public:
            explicit WorkspaceController(QObject *parent = nullptr);

            virtual ~WorkspaceController() = default;

        Q_SIGNALS:

            void settingUpdated(const QString &key, QVariant &new_data);
        };
    }
}