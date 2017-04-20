#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    socket = new IrcSocket("irc.nolimitzone.com", 6667, "Ric_Client", "Ric_Client", "Bent", this);

    connect(socket, SIGNAL(log(QString)), this, SLOT(log(QString)));

    socket->connect();

    socket->join("#clienttest");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::log(QString line)
{
    ui->scrollbackBox->append(line);
}

void MainWindow::on_inputBox_returnPressed()
{
    socket->sendRaw(ui->inputBox->text());
    ui->inputBox->clear();
}
