//
// Created by Roberts Kalnins on 30/04/2021.
//

#ifndef GRAPHER_MAIN_WINDOW_H
#define GRAPHER_MAIN_WINDOW_H

#include <QMainWindow>
#include <QSessionManager>
#include <QPlainTextEdit>

#include "qcustomplot.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() noexcept override;

    void setupPlot(QCustomPlot *plot);


private slots:

    void realtimeDataSlot();

private:

    Ui::MainWindow *ui;

    QString window_title_;
    QTimer data_timer_;

};


#endif //GRAPHER_MAIN_WINDOW_H
