//
// Created by Roberts Kalnins on 30/04/2021.
//

#include <QtWidgets>

#include "main_window.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setGeometry(400, 250, 542, 390);

    setupPlot(ui->customPlot);

    setWindowTitle(window_title_);
    statusBar()->clearMessage();
    ui->customPlot->replot();
}

MainWindow::~MainWindow() noexcept {
    delete ui;
}

void MainWindow::setupPlot(QCustomPlot *plot) {
    plot->addGraph();
    plot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    plot->addGraph();
    plot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    plot->xAxis->setTicker(timeTicker);
    plot->axisRect()->setupFullAxesBox();
    plot->yAxis->setRange(-1.2, 1.2);

    connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)), plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(plot->yAxis, SIGNAL(rangeChanged(QCPRange)), plot->yAxis2, SLOT(setRange(QCPRange)));

    connect(&data_timer_, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    data_timer_.start(0);
}

void MainWindow::realtimeDataSlot() {
    static QTime time_start = QTime::currentTime();
    static double last_point_key = 0;

    double key = time_start.msecsTo(QTime::currentTime()) / 1000.0;

    if (key - last_point_key > 0.002) {
        ui->customPlot->graph(0)->addData(key, qSin(key) + std::rand() / (double) RAND_MAX * 1 * qSin(key / 0.3843));
        ui->customPlot->graph(1)->addData(key, qCos(key) + std::rand() / (double) RAND_MAX * 0.5 * qSin(key / 0.4364));
        last_point_key = key;
    }
    ui->customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->customPlot->replot();

    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key - lastFpsKey > 2) {
        ui->statusBar->showMessage(
                QString("%1 FPS, Total Data points: %2")
                        .arg(frameCount / (key - lastFpsKey), 0, 'f', 0)
                        .arg(ui->customPlot->graph(0)->data()->size() + ui->customPlot->graph(1)->data()->size()), 0);
        lastFpsKey = key;
        frameCount = 0;
    }
}