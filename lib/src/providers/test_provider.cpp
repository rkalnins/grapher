//
// Created by Roberts Kalnins on 08/05/2021.
//

#include "test_provider.h"


namespace grapher::providers {

TestProvider::TestProvider ( QObject *parent ) : AbstractDataProviderModel(
        parent) {
}

void TestProvider::setup ( QJsonObject provider_data ) {
    name_ = provider_data["name"].toString();
    QJsonArray channels = provider_data["channels"].toArray();
    populateChannels(channels);

}

void TestProvider::start () {
    source_ = QThread::create([this] {
        while ( true ) {
            push1();
            push2();
            push3();
            QThread::msleep(10);
        }
//            push3();
//            push4();
    });
    source_->start();
}

void TestProvider::populateChannels ( QJsonArray channels ) {
    for ( auto &&channel : channels ) {
        channels_.push_back(new DataChannel(channel.toObject()));
    }

    for ( int i = 0; i < channels.size(); ++i ) {
        qDebug() << "channel added: " << channels_[i]->getIdentifier();
    }
}

void TestProvider::push1 () {
    static int i = 0;
    i++;
    channels_[0]->set(qSin(i * 3.0));
}

void TestProvider::push2 () {
    static int i = 0;
    i++;
    channels_[1]->set(5.0 * qSin(2.0 * i) + 1.0);
}

void TestProvider::push3 () {
    static int i = 0;
    i++;
    channels_[2]->set(qSin(2.0 * i) + 1.0);
}

void TestProvider::push4 () {
    static int i = 0;
    i++;
    channels_[3]->set(qSin(i / 50));
}


}