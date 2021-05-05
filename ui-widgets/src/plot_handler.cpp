
#include <QDebug>

#include "ui_mainwindow.h"

#include "plot_handler.h"

void PlotHandler::setUI(Ui::MainWindow *ui) {
    ui_ = ui;
}

void PlotHandler::setupPlots(const QJsonObject &grapher_data) {
    qDebug() << "Setting up plots";

    plot_ = ui_->customPlot;

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
    plot_->yAxis->setRange(bounds[0].toDouble(), bounds[1].toDouble());

    connect(plot_->xAxis, SIGNAL(rangeChanged(QCPRange)), plot_->xAxis2, SLOT(setRange(QCPRange)));
    connect(plot_->yAxis, SIGNAL(rangeChanged(QCPRange)), plot_->yAxis2, SLOT(setRange(QCPRange)));

    connect(&data_timer_, SIGNAL(timeout()), this, SLOT(dataSlot()));
    data_timer_.start(0);


    ui_->customPlot->replot();
}

void PlotHandler::setupGraph(const QJsonObject &graph, int index) {
    plot_->addGraph();
    QJsonArray rgb = graph["pen"].toArray();
    qDebug() << "adding plot " << index;
    plot_->graph(index)->setPen(
            QPen(QColor(rgb[0].toInt(), rgb[1].toInt(), rgb[2].toInt())));
}

void PlotHandler::dataSlot() {
    static QTime time_start = QTime::currentTime();
    static double last_point_key = 0;

    double key = time_start.msecsTo(QTime::currentTime()) / 1000.0;

    if (key - last_point_key > min_replot_ms_) {
        for (int i = 0; i < graph_count_; ++i) {
            plot_->graph(i)->addData(key, qSin(key + i) + std::rand() / (double) RAND_MAX * 1 * qSin(key / 0.3843 + i));
        }
        last_point_key = key;
    }
    plot_->xAxis->setRange(key, x_default_span_, Qt::AlignRight);
    plot_->replot();
}