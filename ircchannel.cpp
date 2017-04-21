#include "ircchannel.h"
#include "ircconnection.h"
#include "settings.h"

IrcChannel::IrcChannel(IrcConnection* connection, QString name, QObject* parent) : QObject(parent)
{
    this->connection = connection;
    this->name = name;

    log = new IrcBuffer(100, this);

    channelItem = new QStandardItem(Settings::getInstance()->getIcon("channel.png"), name);
    channelItem->setData(connection->getHost() + "/" + name);

    usersModel = new QStandardItemModel(this);
    usersModel->setSortRole(Qt::UserRole + 1);

    joined = true;
}

IrcChannel::~IrcChannel()
{
    delete usersModel;
    delete log;
}

void IrcChannel::joinUser(QString nick)
{
    if (!nicks.contains(nick))
        addUser(nick);
}

void IrcChannel::partUser(QString nick, QString msg)
{
    qDebug() << nick;

    if (!nicks.contains(nick))
        return;

    qDebug() << "REMOVED";

    usersModel->removeRow(nicks.value(nick).item->row());
    nicks.remove(nick);

    if (msg != "")
        log->append(nick + " left the channel (" + msg + ")", EntryType_SERVER);
    else
        log->append(nick + " left the channel", EntryType_SERVER);
}

void IrcChannel::message(QString nick, QString msg)
{
    log->append(msg, EntryType_MESSAGE, nick);
}

void IrcChannel::setNicks(QStringList nicks)
{
    this->nicks.clear();
    usersModel->clear();

    for (int i = 0; i < nicks.size(); i++)
        addUser(nicks[i], false);

    usersModel->sort(0);
}

void IrcChannel::addUser(QString nick, bool joinMessage)
{
    IrcChannelUser channeluser(nick);
    nicks.insert(channeluser.nick, channeluser);
    usersModel->appendRow(channeluser.item);

    if (joinMessage)
        log->append(nick + " joined the channel.", EntryType_SERVER);

    usersModel->sort(0);
}

void IrcChannel::setJoined(bool joined)
{
    QString channelText = name;
    if (!joined)
    {
        channelText.prepend("(");
        channelText.append(")");
    }

    channelItem->setText(channelText);

    this->joined = joined;
}

void IrcChannel::execute(QString line)
{
    if (line.length() == 0)
        return;

    if (line.at(0) == '/')
    {
        QString cmd = line.mid(1);
        if (cmd.length() == 0)
            return;

        // jump to normal executor eventually
    }

    log->append(line, EntryType_MESSAGE, connection->getUser()->getNick());
    connection->cmd_privmsg(name, line);
}
