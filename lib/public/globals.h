//
// Created by Roberts Kalnins on 30/04/2021.
//

#ifndef GRAPHER_GLOBALS_H
#define GRAPHER_GLOBALS_H

#include <QtCore/qglobal.h>

#if defined(GRAPHER_LIBRARY)
#define GRAPHER_EXPORT Q_DECL_EXPORT
#else
#define GRAPHER_EXPORT Q_DECL_IMPORT
#endif

#endif //GRAPHER_GLOBALS_H
