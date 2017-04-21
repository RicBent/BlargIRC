#ifndef IRCBUFFER_H
#define IRCBUFFER_H

#include <QList>
#include <QObject>

enum IrcBufferEntryType
{
    EntryType_MESSAGE,
    EntryType_SERVER,
    EntryType_CLIENT
};

struct IrcBufferEntry
{
    QString message;
    qint64 timestamp;
    IrcBufferEntryType type;
    QString origin;
};

class IrcBuffer : public QObject
{
    Q_OBJECT
public:
    IrcBuffer(int bufferSize = 100, QObject* parent = 0);
    ~IrcBuffer();

    void append(QString message, IrcBufferEntryType type, QString origin = "");
    IrcBufferEntry* get(int at);

    int getCount();
    int getMaxCount();

signals:
    void changed();

private:
    int bufferSize;
    int bufferLength;
    int bufferPos;
    IrcBufferEntry** buffer;
};

#endif // IRCBUFFER_H
