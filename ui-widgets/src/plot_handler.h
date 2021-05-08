//
// Created by Roberts Kalnins on 04/05/2021.
//

#pragma once

#include <QObject>

#include "qcustomplot.h"

#include "data_model.h"
#include "data_handler.h"

namespace Ui {
    class MainWindow;
}

class PlotHandler : public QObject {
Q_OBJECT

    Q_DISABLE_COPY_MOVE(PlotHandler);

public:
    explicit PlotHandler(QObject *parent = nullptr) : QObject(parent) {}

    void setUI(Ui::MainWindow *ui);

    void setModel(grapher::models::DataModel *model);

    void setupPlots(const QJsonObject &grapher_data);

private:
    void setupGraph(const QJsonObject &graph, int index);

private:
    QCustomPlot *plot_{nullptr};
    QTimer data_timer_;
    Ui::MainWindow *ui_{nullptr};

    grapher::models::DataModel *data_model_{nullptr};

    QTime time_start_{};
    double last_point_key_{0};
    double min_replot_ms_{0.0};

    int x_default_span_{0};
    double default_lower_y_bound_{-1.0};
    double default_upper_y_bound_{1.0};

    size_t graph_count_{0};

    bool is_paused_{true};

public Q_SLOTS:

    void dataSlot();

    void start();

    void pause();

    void toggle();

    void toggleFreeMove(bool on);

    void resetView();

    void updateGraph(int idx, grapher::DataHandler *handler);


};
