//
// Created by Roberts Kalnins on 30/04/2021.
//

#ifndef GRAPHER_MAIN_WINDOW_H
#define GRAPHER_MAIN_WINDOW_H

#include <QMainWindow>
#include <QSessionManager>
#include <QPlainTextEdit>

#include "qcustomplot.h"

#include "main_controller.h"

namespace Ui {
    class MainWindow;
}

using grapher::controllers::MainController;
using grapher::controllers::MenuController;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(MainController &main_controller, QWidget *parent = nullptr);

    ~MainWindow() noexcept override;

    void setupPlot(QCustomPlot *plot);

private:
    void createActions();

    void createMenus();

private slots:

    void realtimeDataSlot();

private:
    QMenu *menu{nullptr};
    QMenu *edit{nullptr};
    QMenu *help{nullptr};

    QAction *newws_action{nullptr};
    QAction *openws_action{nullptr};
    QAction *savews_action{nullptr};
    QAction *savewsas_action{nullptr};


private:

    Ui::MainWindow *ui_;

    MainController *main_controller_;


    QString window_title_;
    QTimer data_timer_;

};


#endif //GRAPHER_MAIN_WINDOW_H
