#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settings.h"
#include "ircconnection.h"
#include "ircbuffer.h"

#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setUsersModel(QStandardItemModel* model);

signals:
    void execute(QString line);

public slots:
    void update_log();

private slots:
    void on_inputBox_returnPressed();

    void on_connectionsList_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    Settings* settings;

    QList<IrcConnection*> connections;

    IrcBuffer* view_log;

    QStandardItemModel* connectionsModel;

    void setStyleSheetFromPath(QString path);

    void addConnection(IrcConnection* connection);
    void selectLog(QString identifier);
    void deselectLog();
};

#endif // MAINWINDOW_H
