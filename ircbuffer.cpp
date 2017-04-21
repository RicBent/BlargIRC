#include "ircbuffer.h"

#include <QDateTime>

IrcBuffer::IrcBuffer(int bufferSize, QObject* parent) : QObject(parent)
{
    this->bufferSize = bufferSize;
    bufferPos = 0;
    bufferLength = 0;

    buffer = new IrcBufferEntry*[bufferSize];
}

IrcBuffer::~IrcBuffer()
{
    for (int i = 0; i < bufferLength; i++)
        delete buffer[i];

    delete[] buffer;
}

void IrcBuffer::append(QString message, IrcBufferEntryType type, QString origin)
{
    IrcBufferEntry* entry = new IrcBufferEntry;
    entry->message = message;
    entry->timestamp = QDateTime::currentSecsSinceEpoch();
    entry->type = type;
    entry->origin = origin;

    buffer[bufferPos] = entry;

    if (bufferLength < bufferSize)
        bufferLength++;

    bufferPos = (bufferPos+1) % bufferSize;


    emit changed();
}

IrcBufferEntry* IrcBuffer::get(int at)
{
    if (at > bufferLength)
        throw std::runtime_error("Out of index IrcBuffer request");

    int index;
    if (bufferLength < bufferSize)
        index = at;
    else
        index = (bufferPos + at) % bufferSize;

    return buffer[index];
}

int IrcBuffer::getCount()
{
    return bufferLength;
}

int IrcBuffer::getMaxCount()
{
    return bufferSize;
}
