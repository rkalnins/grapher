//
// Created by Roberts Kalnins on 30/04/2021.
//

#pragma once

#include <QMainWindow>
#include <QSessionManager>
#include <QFileDialog>
#include <QString>
#include <QTabWidget>
#include <QTableView>

#include "qcustomplot.h"

#include "main_controller.h"
#include "plot_handler.h"

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

private:
    void createActions();

    void createMenus();

    void openWorkspace();

    void saveWorkspaceAs();

    void saveWorkspace();

    void newWorkspace();

private slots:

    void updateTitle(const QString &title);

    void configure(const QJsonObject &data);

private:
    QMenu *menu_{nullptr};

    QAction *newws_action_{nullptr};
    QAction *openws_action_{nullptr};
    QAction *savews_action_{nullptr};
    QAction *savewsas_action_{nullptr};

    QAction *toggle_capture_{nullptr};

private:

    Ui::MainWindow *ui_;

    MainController *main_controller_;

    PlotHandler plot_handler_;

};
