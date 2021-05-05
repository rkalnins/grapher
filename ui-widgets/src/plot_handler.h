//
// Created by Roberts Kalnins on 04/05/2021.
//

#pragma once

#include <QObject>

#include "qcustomplot.h"

namespace Ui {
    class MainWindow;
}

class PlotHandler : public QObject {
Q_OBJECT

    Q_DISABLE_COPY_MOVE(PlotHandler);

public:
    PlotHandler(QObject *parent = nullptr) : QObject(parent) {}

    void setUI(Ui::MainWindow *ui);

    void setupPlots(const QJsonObject &grapher_data);

private:
    void setupGraph(const QJsonObject &graph, int index);

private:
    QCustomPlot *plot_{nullptr};
    QTimer data_timer_;
    Ui::MainWindow *ui_;

    QTime time_start_{};
    double last_point_key_{0};
    double min_replot_ms_{0.0};
    int x_default_span_{0};

    size_t graph_count_;


public Q_SLOTS:

    void dataSlot();

};
