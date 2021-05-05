
#include <QDebug>

#include "plot_handler.h"

#include "ui_mainwindow.h"


void PlotHandler::setUI(Ui::MainWindow *ui) {
    ui_ = ui;
}

void PlotHandler::setProvider(grapher::DataProvider *provider) {
    data_provider_ = provider;
}

void PlotHandler::setupPlots(const QJsonObject &grapher_data) {
    qDebug() << "Setting up plots";

    plot_ = ui_->customPlot;
    plot_->clearGraphs();

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

    start();
}

void PlotHandler::start() {
    connect(&data_timer_, SIGNAL(timeout()), this, SLOT(dataSlot()));

    time_start_ = QTime::currentTime();
    last_point_key_ = 0;

    data_timer_.start(0);

    ui_->customPlot->replot();
}

void PlotHandler::pause() {
    disconnect(&data_timer_, SIGNAL(timeout()), this, SLOT(dataSlot()));
}

void PlotHandler::setupGraph(const QJsonObject &graph, int index) {
    plot_->addGraph();
    QJsonArray rgb = graph["pen"].toArray();
    qDebug() << "adding plot " << index;
    plot_->graph(index)->setPen(
            QPen(QColor(rgb[0].toInt(), rgb[1].toInt(), rgb[2].toInt())));
}

void PlotHandler::dataSlot() {
    double key = time_start_.msecsTo(QTime::currentTime()) / 1000.0;

    if (key - last_point_key_ > min_replot_ms_) {
        std::vector<double> data = data_provider_->getData();

        for (int i = 0; i < graph_count_; ++i) {
            plot_->graph(i)->addData(key, data[i]);
        }
        last_point_key_ = key;
    }
    plot_->xAxis->setRange(key, x_default_span_, Qt::AlignRight);
    plot_->replot();
}