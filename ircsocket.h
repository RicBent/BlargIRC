#ifndef IRCSOCKET_H
#define IRCSOCKET_H

#include <QObject>
#include <QTcpSocket>

struct Channel
{
    QString name;
    QString key;
};

class IrcSocket : public QObject
{
    Q_OBJECT
public:
    explicit IrcSocket(QString host, quint16 port, QString nick, QString user, QString realName, QObject* parent = 0);
    void connect();
    void sendRaw(QString str);
    void sendMsg(QString dest, QString msg);

    void join(QString name, QString key = "");
    void join(Channel channel);

signals:
    void log(QString line);

public slots:
    void connected();
    void welcomed();
    void disconnected();

    void readData();

private:
    QTcpSocket* socket;

    QString host;
    quint16 port;
    QString nick;
    QString user;
    QString realName;
    QString channel;

    QString unifinishedCommand;
    void parseCommand(QString line);

    bool _welcomed;
    QList<Channel> toJoinChannels;

    QList<Channel> joinedChannels;
};

#endif // IRCSOCKET_H
