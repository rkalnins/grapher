//
// Created by Roberts Kalnins on 30/04/2021.
//

#include <QtWidgets>

#include "main_window.h"
#include "ui_mainwindow.h"

#include "main_controller.h"
#include "menu_controller.h"


MainWindow::MainWindow(MainController &main_controller, QWidget *parent) : QMainWindow(parent),
                                                                           main_controller_(&main_controller),
                                                                           ui_(new Ui::MainWindow) {
    ui_->setupUi(this);
    setGeometry(400, 250, 542, 390);

    setupPlot(ui_->customPlot);

    setWindowTitle(window_title_);
    statusBar()->clearMessage();

    createActions();
    createMenus();

    ui_->customPlot->replot();
}

MainWindow::~MainWindow() noexcept {
    delete ui_;
}


void MainWindow::createMenus() {
    menu_ = menuBar()->addMenu(tr("&File"));
    menu_->addAction(newws_action_);
    menu_->addAction(openws_action_);
    menu_->addAction(savews_action_);
    menu_->addAction(savewsas_action_);

    menuBar()->addAction(menu_->menuAction());
    menuBar()->setNativeMenuBar(false);
}

void MainWindow::createActions() {

    newws_action_ = new QAction(tr("&New workspace..."), this);
    connect(newws_action_, &QAction::triggered, this,
            &MainWindow::newWorkspace);

    openws_action_ = new QAction(tr("&Open workspace..."), this);
    connect(openws_action_, &QAction::triggered, this, &MainWindow::openWorkspace);

    savews_action_ = new QAction(tr("&Save workspace"), this);
    connect(savews_action_, &QAction::triggered, this,
            &MainWindow::saveWorkspace);

    savewsas_action_ = new QAction(tr("&Save workspace as..."), this);
    connect(savewsas_action_, &QAction::triggered, this,
            &MainWindow::saveWorkspaceAs);
}


void MainWindow::openWorkspace() {
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);


    if (dialog.exec()) {
        auto filenames = dialog.selectedUrls();
        if (!filenames.empty()) {
            emit main_controller_->getMenuController()->openWorkspaceClicked(filenames[0]);
        }
    }
}

void MainWindow::newWorkspace() {
    emit main_controller_->getMenuController()->newWorkspaceClicked();
}

void MainWindow::saveWorkspace() {
    emit main_controller_->getMenuController()->saveWorkspaceClicked();
}

void MainWindow::saveWorkspaceAs() {
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setViewMode(QFileDialog::Detail);

    auto filename = QFileDialog::getSaveFileUrl();
    if (!filename.isEmpty()) {
        emit main_controller_->getMenuController()->saveWorkspaceAsClicked(filename);
    }
}

void MainWindow::setupPlot(QCustomPlot *plot) {
    plot->addGraph();
    plot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    plot->addGraph();
    plot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    QSharedPointer<QCPAxisTickerTime> time_ticker(new QCPAxisTickerTime);
    time_ticker->setTimeFormat("%h:%m:%s");
    plot->axisRect()->setupFullAxesBox();
    plot->xAxis->setTicker(time_ticker);
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
        ui_->customPlot->graph(0)->addData(key, qSin(key) + std::rand() / (double) RAND_MAX * 1 * qSin(key / 0.3843));
        ui_->customPlot->graph(1)->addData(key, qCos(key) + std::rand() / (double) RAND_MAX * 0.5 * qSin(key / 0.4364));
        last_point_key = key;
    }
    ui_->customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    ui_->customPlot->replot();

    static double last_fps_key;
    static int frame_count;

    ++frame_count;
    if (key - last_fps_key > 2) {
        ui_->statusBar->showMessage(
                QString("%1 FPS, Total Data points: %2")
                        .arg(frame_count / (key - last_fps_key), 0, 'f', 0)
                        .arg(ui_->customPlot->graph(0)->data()->size() + ui_->customPlot->graph(1)->data()->size()), 0);
        last_fps_key = key;
        frame_count = 0;
    }
}