#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ircsocket.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void log(QString line);

private slots:
    void on_inputBox_returnPressed();

private:
    Ui::MainWindow *ui;

    IrcSocket* socket;
};

#endif // MAINWINDOW_H
