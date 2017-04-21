#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    view_log = NULL;

    ui->setupUi(this);

    ui->connectionListSplitter->setStretchFactor(0, 0);
    ui->connectionListSplitter->setStretchFactor(1, 1);

    ui->userListSplitter->setStretchFactor(0, 1);
    ui->userListSplitter->setStretchFactor(1, 0);

    connectionsModel = new QStandardItemModel(this);
    ui->connectionsList->setModel(connectionsModel);

    ui->usersList->setLayoutMode(QListView::Batched);

    QCoreApplication::setOrganizationName("Blarg City");
    QCoreApplication::setApplicationName("BlargIRC");
    settings = Settings::init(this);

    setWindowIcon(settings->getIcon("channel.png"));

    /*
     *  TEST AREA
     */

    IrcUser* user = new IrcUser("Ric_Client", "Ric_Client", "RicBent");

    IrcConnection* connection = new IrcConnection("irc.nolimitzone.com", 6667, user, this);
    connection->connect();
    addConnection(connection);

    selectLog(connection->getHost());

    setStyleSheetFromPath("NightMode.qss");
}

MainWindow::~MainWindow()
{
    foreach (IrcConnection* connection, connections)
        delete connection;
    delete ui;
    delete settings;
}

void MainWindow::setStyleSheetFromPath(QString path)
{
    QFile styleSheetFile(settings->getDataFolderPath() + path);

    if (!styleSheetFile.exists())
    {
        setStyleSheet("");
        return;
    }

    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheetTxt = QLatin1String(styleSheetFile.readAll());
    setStyleSheet(styleSheetTxt);
}

void MainWindow::setUsersModel(QStandardItemModel* model)
{
    ui->usersList->setHidden(model == NULL);

    ui->usersList->setModel(model);
}

void MainWindow::update_log()
{
    QTextEdit* scrollback = ui->scrollbackBox;

    if (view_log == NULL)
    {
        scrollback->clear();
        return;
    }

    scrollback->clear();

    for (int i = 0; i < view_log->getCount(); i++)
    {
        IrcBufferEntry* entry = view_log->get(i);

        QString time = QDateTime::fromSecsSinceEpoch(entry->timestamp).toString("hh:mm:ss");

        QString origin;
        if (entry->type == EntryType_MESSAGE)
            origin = entry->origin;
        else if (entry->type == EntryType_SERVER)
            origin = "SERVER";
        else
            origin = "CLIENT";

        scrollback->append("[" + time + "] " + origin + ": " + entry->message);
    }
}

void MainWindow::addConnection(IrcConnection* connection)
{
    connections.append(connection);
    connectionsModel->invisibleRootItem()->appendRow(connection->getConnectionsitem());
}

void MainWindow::selectLog(QString identifier)
{
    deselectLog();

    int slashIndex = identifier.indexOf('/');

    QString host = identifier.mid(0, slashIndex);

    IrcBuffer* new_log = NULL;

    foreach (IrcConnection* connection, connections)
    {
        if (connection->getHost() == host)
        {
            if (slashIndex == -1)
            {
                QObject::connect(this, SIGNAL(execute(QString)), connection, SLOT(execute(QString)));
                new_log = connection->getLog();
            }
            else
            {
                QString sub = identifier.mid(slashIndex + 1);
                new_log = connection->getLog(sub, this);
            }

            break;
        }
    }

    view_log = new_log;

    if (view_log == NULL)
        return;

    update_log();

    QObject::connect(view_log, SIGNAL(changed()), this, SLOT(update_log()));
}

void MainWindow::deselectLog()
{
    setUsersModel(NULL);

    if (view_log == NULL)
        return;

    QObject::disconnect(view_log, SIGNAL(changed()), this, SLOT(update_log()));
    QObject::disconnect(this, SIGNAL(execute(QString)), 0, 0);
}


// Slots

void MainWindow::on_connectionsList_clicked(const QModelIndex& index)
{
    selectLog(index.data(Qt::UserRole + 1).toString());
}

void MainWindow::on_inputBox_returnPressed()
{
    if (view_log == NULL)
        return;

    emit execute(ui->inputBox->text());
    ui->inputBox->clear();
}
