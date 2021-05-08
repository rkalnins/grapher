//
// Created by Roberts Kalnins on 08/05/2021.
//

#pragma once

#include <QObject>
#include <QThread>
#include <QJsonObject>
#include <QJsonArray>

#include "abstract_data_provider_model.h"

#include "globals.h"

namespace grapher::providers {

    class GRAPHER_EXPORT TestProvider : public models::AbstractDataProviderModel {
    public:
        TestProvider(QObject *parent = nullptr);

        void populateChannels(QJsonArray channels) override;

        void start() override;

        void setup(QJsonObject provider_data) override;

    private:

        void push1();

        void push2();

        void push3();

        void push4();

    private:
        QThread *source_{nullptr};

    };

}