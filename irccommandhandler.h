#ifndef IRCCOMMANDHANDLER_H
#define IRCCOMMANDHANDLER_H

#include <QObject>
#include <QVector>

#include "ircuser.h"

class IrcConnection;

class IrcCommandHandler : public QObject
{
    Q_OBJECT
public:
    explicit IrcCommandHandler(IrcConnection* connection, QObject* parent = 0);
    void parseCommand(QString line);

signals:
    void ping(QString server);
    void pong(QString server);
    void message(IrcUser user, QString dest, QString msg);
    void join(IrcUser user, QString channel);
    void part(IrcUser user, QString channel, QString msg);
    void nicklist(QString channel, QStringList nicks);

private:
    IrcConnection* connection;

    QString removeColon(QString str);
    QStringList splitParams(QString params, bool removeColon = false);

    typedef void (IrcCommandHandler::*IrcCommandExec)(QString command, QString prefix, QString params);

    struct IrcCommand_Numerical
    {
        int id;
        IrcCommandExec execute;

        IrcCommand_Numerical(int id, IrcCommandExec execute)
        {
            this->id = id;
            this->execute = execute;
        }
    };

    struct IrcCommand_Named
    {
        QString name;
        IrcCommandExec execute;

        IrcCommand_Named(QString name, IrcCommandExec execute)
        {
            this->name = name;
            this->execute = execute;
        }
    };

    static const int cmdCount_Numerical = 2;
    static const IrcCommand_Numerical cmds_Numerical[];

    static const int cmdCount_Named = 6;
    static const IrcCommand_Named cmds_Named[];

    void cmd_ServerMessage(QString command, QString prefix, QString params);

    void cmd_RPL_NAMREPLY(QString command, QString prefix, QString params);

    void cmd_PING(QString command, QString prefix, QString params);
    void cmd_PONG(QString command, QString prefix, QString params);
    void cmd_PRIVMSG(QString command, QString prefix, QString params);
    void cmd_JOIN(QString command, QString prefix, QString params);
    void cmd_PART(QString command, QString prefix, QString params);
};

#endif // IRCCOMMANDHANDLER_H
