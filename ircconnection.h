#ifndef IRCCONNECTION_H
#define IRCCONNECTION_H

#include "ircuser.h"
#include "irccommandhandler.h"
#include "ircbuffer.h"
#include "ircchannel.h"

#include <QTcpSocket>
#include <QHash>

class MainWindow;

class IrcConnection : public QObject
{
    Q_OBJECT

public:
    IrcConnection(QString host, quint16 port, IrcUser* user, QObject* parent = 0);
    ~IrcConnection();

    void connect();
    void sendRaw(QString str);

    QString getHost() { return host; }

    IrcBuffer* getLog();
    IrcBuffer* getLog(QString identifier, MainWindow* connect = NULL);

    IrcUser* getUser() { return user; }

    QStandardItem* getConnectionsitem() { return connectionsItem; }

public slots:
    void execute(QString line) { sendRaw(line); }

public slots:
    void log_server(QString msg);

    // Client Commands
    void cmd_nick(QString nick);
    void cmd_user(QString user, QString realname = "");
    void cmd_ping(QString server);
    void cmd_pong(QString pong);
    void cmd_join(QString channel, QString key);
    void cmd_privmsg(QString dest, QString msg);

    // Server Command Handles
    void handle_join(IrcUser user, QString channel);
    void handle_part(IrcUser user, QString channel, QString msg);
    void handle_privmsg(IrcUser user, QString dest, QString msg);
    void handle_nicklist(QString channel, QStringList nicks);

private slots:
    void connected();
    void disconnected();
    void readData();

private:
    IrcUser* user;
    QString host;
    quint16 port;

    QTcpSocket* socket;
    IrcCommandHandler* commandHandler;

    QString unifinishedCommand;

    QStandardItem* connectionsItem;

    IrcBuffer* serverLog;

    QHash<QString, IrcChannel*> channels;


    // Misc Functions

    void addChannel(QString name);
};

#endif // IRCCONNECTION_H
