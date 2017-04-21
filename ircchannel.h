#ifndef IRCCHANNEL_H
#define IRCCHANNEL_H

#include "ircbuffer.h"
#include "settings.h"

#include <QStandardItem>
#include <QHash>

class IrcConnection;


struct IrcChannelUser
{
    QString nick;
    char prefix;
    QStandardItem* item = NULL;

    IrcChannelUser() {}
    IrcChannelUser(QString name)
    {
        this->nick = name;
        updatePrefix(name);
    }

    void updatePrefix(QString _nick)
    {
        QString sort;
        switch (_nick.at(0).toLatin1())
        {
        case '~':
            sort = "0";
            break;
        case '&':
            sort = "1";
            break;
        case '@':
            sort = "2";
            break;
        case '%':
            sort = "3";
            break;
        case '+':
            sort = "4";
            break;
        default:
            sort = "5";
            break;
        }

        if (sort != "5")
        {
            prefix = _nick.at(0).toLatin1();
            nick = _nick.mid(1);
        }
        else
        {
            prefix = '\0';
            nick = _nick;
        }

        sort.append(nick.toLower());

        if (item == NULL)
            item = new QStandardItem(nick);

        if (prefix != '\0')
            item->setIcon(Settings::getInstance()->getIcon(QString("prefix_%1.png").arg(prefix)));
        else
        {
            QPixmap pix(20,20);
            pix.fill(Qt::transparent);
            item->setIcon(QIcon(pix));
        }
        item->setData(sort, Qt::UserRole + 1);
    }
};

class IrcChannel : public QObject
{
    Q_OBJECT
public:
    IrcChannel(IrcConnection* connection, QString name, QObject* parent=0);
    ~IrcChannel();

    void joinUser(QString nick);
    void partUser(QString nick, QString msg);
    void message(QString nick, QString msg);
    void setNicks(QStringList nicks);

    QString getName() { return name; }

    IrcBuffer* getLog() { return log; }
    QStandardItem* getChannnelItem() { return channelItem; }
    QStandardItemModel* getUsersModel() { return usersModel; }

public slots:
    void execute(QString line);

private:    
    IrcConnection* connection;
    IrcBuffer* log;

    QStandardItem* channelItem;
    QStandardItemModel* usersModel;

    QString name;
    bool joined;

    QHash<QString, IrcChannelUser> nicks;

    void setJoined(bool joined);
    void addUser(QString nick, bool joinMessage = true);
};

#endif // IRCCHANNEL_H
