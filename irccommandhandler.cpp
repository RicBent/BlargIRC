#include "irccommandhandler.h"
#include "ircconnection.h"


const IrcCommandHandler::IrcCommand_Numerical IrcCommandHandler::cmds_Numerical[] =
{
    { 353,          &IrcCommandHandler::cmd_RPL_NAMREPLY    },
    { 366,          NULL /* RPL_ENDOFNAMES */               }
};

const IrcCommandHandler::IrcCommand_Named IrcCommandHandler::cmds_Named[] =
{
    { "NOTICE",     &IrcCommandHandler::cmd_ServerMessage   },
    { "PING",       &IrcCommandHandler::cmd_PING            },
    { "PONG",       &IrcCommandHandler::cmd_PONG            },
    { "PRIVMSG",    &IrcCommandHandler::cmd_PRIVMSG         },
    { "JOIN",       &IrcCommandHandler::cmd_JOIN            },
    { "PART",       &IrcCommandHandler::cmd_PART            }
};


IrcCommandHandler::IrcCommandHandler(IrcConnection* connection, QObject* parent) : QObject(parent)
{
    this->connection = connection;
}

void IrcCommandHandler::parseCommand(QString line)
{
    qDebug() << line;

    QStringList segments = line.split(" ", QString::SkipEmptyParts);

    if (segments.size() == 0)
        return;


    // Parse Prefix

    QString prefix = "";
    if (segments.at(0).startsWith(':'))
    {
        prefix = segments[0].remove(0, 1);
        segments.removeFirst();
    }


    // Parse Command

    if (segments.size() < 1)
        return; // wat

    QString cmd = segments.at(0);
    segments.removeFirst();


    // Isolate Other Params
    QString params = line.mid(prefix.length() + cmd.length() + 2);


    // Execute Numeric Commands
    bool isNumerical;
    int numerical = cmd.toInt(&isNumerical);
    if (isNumerical)
    {
        for (int i = 0; i < cmdCount_Numerical; i++)
        {
            if (cmds_Numerical[i].id == numerical)
            {
                IrcCommandExec exec = cmds_Numerical[i].execute;
                if (exec != NULL)
                    (this->*exec)(cmd, prefix, params);
                return;
            }
        }

        cmd_ServerMessage(cmd, prefix, params);
        return;
    }

    // Execute Named Commands
    for (int i = 0; i < cmdCount_Named; i++)
    {
        if (cmds_Named[i].name == cmd)
        {
            IrcCommandExec exec = cmds_Named[i].execute;
            if (exec != NULL)
                (this->*exec)(cmd, prefix, params);
            return;
        }
    }

    connection->log_server("Unhandled command: " + line);
}

QString IrcCommandHandler::removeColon(QString str)
{
    if (str.startsWith(':'))
        return str.remove(0, 1);
    return str;
}

QStringList IrcCommandHandler::splitParams(QString params, bool removeColon)
{
    QStringList list;
    int start = 0;
    int end;

    while ((end = params.indexOf(' ', start)) != -1)
    {
        if (start != end)
        {
            if (params.at(start) == ":")
            {
                list.append(params.mid(removeColon ? start+1 : start));
                return list;
            }
            list.append(params.mid(start, end - start));
        }

        start = end + 1;
    }

    if (start != params.size())
    {
        if (params.at(start) == ":")
        {
            list.append(params.mid(removeColon ? start+1 : start));
            return list;
        }
        list.append(params.mid(start));
    }

    return list;
}

void IrcCommandHandler::cmd_ServerMessage(QString command, QString prefix, QString params)
{
    QStringList segs = splitParams(params, true);

    if (segs.size() >= 2)
        connection->log_server(segs[segs.size()-1]);
}

void IrcCommandHandler::cmd_RPL_NAMREPLY(QString command, QString prefix, QString params)
{
    // ":kuroi.irc.nolimitzone.com 353 Ric_Client_rly = #clienttest :Ric_Client_rly RicBent "

    QStringList segs = splitParams(params, true);

    if (segs.size() < 4)
        return; // wat

    emit nicklist(segs[2], segs[3].split(' ', QString::SkipEmptyParts));
}

void IrcCommandHandler::cmd_PING(QString command, QString prefix, QString params)
{
    QStringList segs = splitParams(params);

    if (segs.size() < 1)
        return; // wat

    QString server = segs[segs.size()-1];

    connection->log_server("Received ping from " + server);

    emit ping(server);
}

void IrcCommandHandler::cmd_PONG(QString command, QString prefix, QString params)
{
    QStringList segs = splitParams(params);

    if (segs.size() < 1)
        return; // wat

    QString server = segs[segs.size()-1];

    connection->log_server("Received pong from " + server);
}

void IrcCommandHandler::cmd_PRIVMSG(QString command, QString prefix, QString params)
{
    QStringList segs = splitParams(params, true);

    if (segs.size() < 2)
        return; // wat

    emit message(IrcUser(prefix), segs[0], segs[1]);
}

void IrcCommandHandler::cmd_JOIN(QString command, QString prefix, QString params)
{
    QStringList segs = splitParams(params, true);

    if (segs.size() < 1)
        return; // wat

    emit join(IrcUser(prefix), segs[segs.size()-1]);
}

void IrcCommandHandler::cmd_PART(QString command, QString prefix, QString params)
{
    // ":RicBent!RicBent@46.101.129.38 PART #clienttest :Leaving"

    QStringList segs = splitParams(params, true);

    if (segs.size() < 1)
        return; // wat

    QString msg;
    if (segs.size() > 1)
        msg = segs[1];
    else
        msg = "";

    emit part(IrcUser(prefix), segs[0], msg);
}
