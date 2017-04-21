#ifndef IRCUSER_H
#define IRCUSER_H

#include <QString>
#include <QStandardItem>

class IrcUser
{
public:
    IrcUser(QString nick, QString user, QString realname = "", QString host = "");
    IrcUser(QString prefix);

    QString getNick() const;
    void setNick(const QString &value);
    QString getUser() const;
    void setUser(const QString &value);
    QString getRealname() const;
    void setRealname(const QString &value);
    QString getHost() const;
    void setHost(const QString &value);

private:
    QString nick;
    QString user;
    QString realname;
    QString host;
};

#endif // IRCUSER_H
