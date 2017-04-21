#include "ircuser.h"

IrcUser::IrcUser(QString nick, QString user, QString realname, QString host)
{
    this->nick = nick;
    this->user = user;
    this->realname = realname;
    this->host = host;
}

IrcUser::IrcUser(QString prefix)
{
    QStringList exSplit = prefix.split('!');

    if (exSplit.size() >= 1)
        nick = exSplit[0];
    if (exSplit.size() < 2)
        return;

    QStringList atSplit = prefix.split(exSplit[1]);

    if (atSplit.size() >= 1)
        user = atSplit[0];
    if (atSplit.size() >= 2)
        host = atSplit[1];
}


QString IrcUser::getNick() const
{
    return nick;
}

void IrcUser::setNick(const QString &value)
{
    nick = value;
}

QString IrcUser::getUser() const
{
    return user;
}

void IrcUser::setUser(const QString &value)
{
    user = value;
}

QString IrcUser::getRealname() const
{
    return realname;
}

void IrcUser::setRealname(const QString &value)
{
    realname = value;
}

QString IrcUser::getHost() const
{
    return host;
}

void IrcUser::setHost(const QString &value)
{
    host = value;
}
