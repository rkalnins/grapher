//
// Created by Roberts Kalnins on 05/05/2021.
//

#pragma once

#include <QObject>

#include "globals.h"


namespace grapher::controllers {
class GRAPHER_EXPORT DataController : public QObject {
  Q_OBJECT

    Q_DISABLE_COPY_MOVE(DataController)

  public:

    explicit DataController ( QObject *parent = nullptr ) : QObject(
            parent) {}

    virtual ~DataController () = default;

};

}
