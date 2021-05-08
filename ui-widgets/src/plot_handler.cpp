
#include <QDebug>

#include "plot_handler.h"

#include "ui_mainwindow.h"


void PlotHandler::setUI(Ui::MainWindow *ui) {
    ui_ = ui;
}

void PlotHandler::setModel(grapher::models::DataModel *model) {
    data_model_ = model;
}

void PlotHandler::toggleFreeMove(bool on) {
    if (on) {
        plot_->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
        plot_->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    } else {
        plot_->axisRect()->setRangeDrag(Qt::Horizontal);
        plot_->axisRect()->setRangeZoom(Qt::Horizontal);
    }
}

void PlotHandler::resetView() {
    plot_->yAxis->setRange(default_lower_y_bound_, default_upper_y_bound_);
    plot_->xAxis->setRange(last_point_key_, x_default_span_, Qt::AlignRight);
    plot_->replot();
}

void PlotHandler::setupPlots(const QJsonObject &grapher_data) {
    qDebug() << "Setting up plots";

    plot_ = ui_->customPlot;
    plot_->clearGraphs();
    plot_->setInteraction(QCP::iSelectPlottables, true);
    plot_->setInteraction(QCP::iRangeZoom, true);
    plot_->setInteraction(QCP::iRangeDrag, true);
    plot_->setInteraction(QCP::iSelectAxes, true);

    toggleFreeMove(false);

    QJsonArray graphs = grapher_data["graphs"].toArray();

    graph_count_ = graphs.size();
    qDebug() << "graph count: " << graph_count_;

    for (int i = 0; i < graph_count_; ++i) {
        setupGraph(graphs[i].toObject(), i);
    }

    min_replot_ms_ = grapher_data["min-replot"].toDouble();
    x_default_span_ = grapher_data["x-default-span"].toInt();

    QSharedPointer<QCPAxisTickerTime> time_ticker(new QCPAxisTickerTime);
    time_ticker->setTimeFormat(grapher_data["time-format"].toString());
    plot_->axisRect()->setupFullAxesBox();
    plot_->xAxis->setTicker(time_ticker);

    QJsonArray bounds = grapher_data["y-range-default"].toArray();
    default_lower_y_bound_ = bounds[0].toDouble();
    default_upper_y_bound_ = bounds[1].toDouble();
    plot_->yAxis->setRange(default_lower_y_bound_, default_upper_y_bound_);

    connect(plot_->xAxis, SIGNAL(rangeChanged(QCPRange)), plot_->xAxis2, SLOT(setRange(QCPRange)));
    connect(plot_->yAxis, SIGNAL(rangeChanged(QCPRange)), plot_->yAxis2, SLOT(setRange(QCPRange)));

    ui_->statusBar->showMessage("Press \'Capture\' to start plotting");
}

void PlotHandler::start() {
    if (!is_paused_) {
        return;
    }

    connect(&data_timer_, SIGNAL(timeout()), this, SLOT(dataSlot()));

    time_start_ = QTime::currentTime();
    last_point_key_ = 0;

    data_timer_.start(0);

    ui_->customPlot->replot();

    is_paused_ = false;
    ui_->statusBar->showMessage("Capturing");
}

void PlotHandler::pause() {
    disconnect(&data_timer_, SIGNAL(timeout()), this, SLOT(dataSlot()));
    is_paused_ = true;
    ui_->statusBar->showMessage("Paused");
}

void PlotHandler::toggle() {
    if (is_paused_) {
        start();
    } else {
        pause();
    }
}

void PlotHandler::setupGraph(const QJsonObject &graph, int index) {
    plot_->addGraph();
    QJsonArray rgb = graph["pen"].toArray();
    qDebug() << "adding plot " << index;
    plot_->graph(index)->setPen(
            QPen(QColor(rgb[0].toInt(), rgb[1].toInt(), rgb[2].toInt())));
    plot_->graph(index)->setVisible(graph["visible"].toBool());
    plot_->graph(index)->setName(graph["name"].toString());
}

void PlotHandler::updateGraph(int idx, grapher::DataHandler *handler) {
    plot_->graph(idx)->setPen(QPen(handler->getPenColor()));
    plot_->graph(idx)->setVisible(handler->isVisible());
    plot_->graph(idx)->setName(handler->getName());
    plot_->replot();
}

void PlotHandler::dataSlot() {
    double key = time_start_.msecsTo(QTime::currentTime()) / 1000.0;

    if (key - last_point_key_ > min_replot_ms_) {
        for (int i = 0; i < graph_count_; ++i) {
            plot_->graph(i)->addData(key, data_model_->getStreamData(i));
        }
        last_point_key_ = key;
    }
    plot_->xAxis->setRange(key, x_default_span_, Qt::AlignRight);
    plot_->replot();
}