//
// Created by Roberts Kalnins on 30/04/2021.
//

#include <QtWidgets>

#include "main_window.h"
#include "ui_mainwindow.h"

#include "main_controller.h"

#include "menu_model.h"


MainWindow::MainWindow(MainController &main_controller, QWidget *parent) : QMainWindow(parent),
                                                                           main_controller_(&main_controller),
                                                                           ui_(new Ui::MainWindow) {
    connect(main_controller_, &MainController::titleChanged, this, &MainWindow::updateTitle);
    connect(main_controller_, &MainController::dataChanged, this, &MainWindow::configure);

    ui_->setupUi(this);
    setGeometry(400, 250, 542, 390);

    createActions();
    createMenus();

    statusBar()->clearMessage();

    plot_handler_.setUI(ui_);
    plot_handler_.setProvider(main_controller_->getDataProvider());

}

MainWindow::~MainWindow() noexcept {
    delete ui_;
}

void MainWindow::configure(const QJsonObject &data) {
    qDebug() << "configuring with " << data["name"].toString() << "file";

    QJsonObject grapher_config = data["grapher"].toObject();
    plot_handler_.setupPlots(grapher_config);
    statusBar()->clearMessage();
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

    connect(ui_->startCapture, &QAction::triggered, &plot_handler_, &PlotHandler::start);
    connect(ui_->stopCapture, &QAction::triggered, &plot_handler_, &PlotHandler::pause);

    toggle_capture_ = new QAction("&Toggle capture");
    toggle_capture_->setShortcut(Qt::Key_Space);
    addAction(toggle_capture_);
    connect(toggle_capture_, &QAction::triggered, &plot_handler_, &PlotHandler::toggle);
}

void MainWindow::updateTitle(const QString &title) {
    setWindowTitle(title);
    qDebug() << "title changed to " << title;
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