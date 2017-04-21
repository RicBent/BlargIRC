#include "ircconnection.h"
#include "settings.h"
#include "mainwindow.h"

IrcConnection::IrcConnection(QString host, quint16 port, IrcUser *user, QObject* parent) : QObject(parent)
{
    this->host = host;
    this->port = port;
    this->user = user;

    serverLog = new IrcBuffer(100, this);

    connectionsItem = new QStandardItem(Settings::getInstance()->getIcon("server.png"), host);
    connectionsItem->setData(host);

    commandHandler = new IrcCommandHandler(this, this);
    QObject::connect(commandHandler, SIGNAL(ping(QString)), this, SLOT(cmd_pong(QString)));
    QObject::connect(commandHandler, SIGNAL(join(IrcUser,QString)), this, SLOT(handle_join(IrcUser,QString)));
    QObject::connect(commandHandler, SIGNAL(part(IrcUser,QString,QString)), this, SLOT(handle_part(IrcUser,QString,QString)));
    QObject::connect(commandHandler, SIGNAL(message(IrcUser,QString,QString)), this, SLOT(handle_privmsg(IrcUser,QString,QString)));
    QObject::connect(commandHandler, SIGNAL(nicklist(QString,QStringList)), this, SLOT(handle_nicklist(QString,QStringList)));
}

IrcConnection::~IrcConnection()
{
    foreach (IrcChannel* channel, channels.values())
        delete channel;

    delete commandHandler;
    delete serverLog;
}

void IrcConnection::connect()
{
    socket = new QTcpSocket(this);

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
    QObject::connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    socket->connectToHost(host, port);
}

void IrcConnection::sendRaw(QString str)
{
    if (!str.endsWith("\r\n"))
        str.append("\r\n");
    socket->write(str.toUtf8());
}

IrcBuffer* IrcConnection::getLog()
{
    return serverLog;
}

IrcBuffer* IrcConnection::getLog(QString identifier, MainWindow* connect)
{
    foreach (IrcChannel* channel, channels)
    {
        if (channel->getName() == identifier)
        {
            if (connect != NULL)
            {
                QObject::connect(connect, SIGNAL(execute(QString)), channel, SLOT(execute(QString)));
                connect->setUsersModel(channel->getUsersModel());
            }
            return channel->getLog();
        }
    }

    return NULL;
}

void IrcConnection::connected()
{
    serverLog->append("Connected. Now logging in.", EntryType_CLIENT);

    cmd_nick(user->getNick());
    cmd_user(user->getUser(), user->getRealname());
}

void IrcConnection::disconnected()
{
    serverLog->append("Disconnected.", EntryType_CLIENT);
}

void IrcConnection::readData()
{
    QString input(unifinishedCommand + socket->readAll());

    QStringList commandList = input.split("\r\n", QString::SkipEmptyParts);

    bool finished = false;
    if (input.size() >= 2)
    {
        if (input.right(2) == "\r\n")
            finished = true;
    }

    if (finished)
        unifinishedCommand = "";
    else
    {
        unifinishedCommand = commandList.last();
        commandList.removeLast();
    }

    for (int i = 0; i < commandList.size(); i++)
        commandHandler->parseCommand(commandList[i]);
}


// Slots

void IrcConnection::log_server(QString msg)
{
    serverLog->append(msg, EntryType_SERVER);
}


// Client Commands

void IrcConnection::cmd_nick(QString nick)
{
    sendRaw("NICK " + nick);
}

void IrcConnection::cmd_user(QString user, QString realname)
{
    if (realname == "")
        realname = user;

    sendRaw("USER " + user + " 0 * :" + realname);
}

void IrcConnection::cmd_ping(QString server)
{
    sendRaw("PING " + server);
}

void IrcConnection::cmd_pong(QString server)
{
    sendRaw("PONG " + server);
}

void IrcConnection::cmd_join(QString channel, QString key)
{
    if (key == "")
        sendRaw("JOIN " + channel);
    else
        sendRaw("JOIN " + channel + " " + key);
}

void IrcConnection::cmd_privmsg(QString dest, QString msg)
{
    sendRaw("PRIVMSG " + dest + " :" + msg);
}


// Server Command Handles

void IrcConnection::handle_join(IrcUser user, QString channel)
{
    if (user.getNick() == this->user->getNick())
    {
        if (!channels.contains(channel))
            addChannel(channel);
    }
    else
    {
        if (channels.contains(channel))
            channels.value(channel)->joinUser(user.getNick());
    }
}

void IrcConnection::handle_part(IrcUser user, QString channel, QString msg)
{
    if (!channels.contains(channel))
        return;

    channels.value(channel)->partUser(user.getNick(), msg);
}

void IrcConnection::handle_privmsg(IrcUser user, QString dest, QString msg)
{
    if (channels.contains(dest))
        channels.value(dest)->message(user.getNick(), msg);
}

void IrcConnection::handle_nicklist(QString channel, QStringList nicks)
{
    if (channels.contains(channel))
        channels.value(channel)->setNicks(nicks);
}


// Misc Functions

void IrcConnection::addChannel(QString name)
{
    IrcChannel* channel = new IrcChannel(this, name);
    channels.insert(name, channel);

    connectionsItem->appendRow(channel->getChannnelItem());
}
