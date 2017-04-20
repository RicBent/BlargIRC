#include "ircsocket.h"

IrcSocket::IrcSocket(QString host, quint16 port, QString nick, QString user, QString realName, QObject* parent) : QObject(parent)
{
    this->host = host;
    this->port = port;
    this->nick = nick;
    this->user = user;
    this->realName = realName;

    _welcomed = false;
}

void IrcSocket::sendRaw(QString str)
{
    qDebug() << "Sending:" << str;

    socket->write(str.toUtf8() + "\r\n");
}

void IrcSocket::connect()
{
    socket = new QTcpSocket(this);

    QObject::connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));

    socket->connectToHost(host, port);
}

void IrcSocket::connected()
{
    qDebug() << "connected";
    sendRaw("NICK " + nick + "\r\nUSER " + user + " " + user + " " + socket->peerName() + " :" + realName);
}

void IrcSocket::disconnected()
{
    qDebug() << "disconnected";
}

void IrcSocket::welcomed()
{
    qDebug() << "welcomed";
    _welcomed = true;

    foreach (Channel c, toJoinChannels)
        join(c);

    toJoinChannels.clear();
}

void IrcSocket::readData()
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
        parseCommand(commandList.at(i));
}

void IrcSocket::parseCommand(QString line)
{
    //
    //  MAJOR TODO:
    //  Move this shit to own class
    //

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

    qDebug() << cmd;


    // Check for Reply Codes

    bool isCode;
    int code = cmd.toInt(&isCode);

    if (isCode)
    {
        switch (code)
        {
        case 1:
            emit welcomed();
            break;
        default:
            emit log(QString("<font color=\"red\">Unhandled reply code: %1</font>").arg(code));
            break;
        }

        return;
    }


    // Check for normal commands

    if (cmd == "PING")
    {
        QString pingOrigin;
        if (segments.size() == 1)
            pingOrigin = segments.at(0);
        else if (segments.size() >= 1)
            pingOrigin = segments.at(1);
        else
        {
            log("wat: PING without destination");
            return;
        }

        if (pingOrigin.startsWith(':'))
            pingOrigin = pingOrigin.remove(0, 1);

        sendRaw("PONG " + pingOrigin);

        emit log("Recieved PING from " + pingOrigin);
    }
    else if (cmd == "PRIVMSG")
    {

    }
}

void IrcSocket::join(QString name, QString key)
{
    Channel c;
    c.name = name;
    c.key = key;
    join(c);
}

void IrcSocket::join(Channel channel)
{
    if (!_welcomed)
    {
        toJoinChannels.append(channel);
        return;
    }

    if (channel.key == "")
        sendRaw("JOIN " + channel.name);
    else
        sendRaw("JOIN " + channel.name + " " + channel.key);

    joinedChannels.append(channel);
}

void IrcSocket::sendMsg(QString dest, QString msg)
{
    sendRaw("PRIVMSG " + dest + " :" + msg);
}
