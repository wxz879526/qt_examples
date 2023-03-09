#include "connection.h"
#include <QTimerEvent>
#include <QHostAddress>

static const int TransferTimeout = 30 * 1000;
static const int PongTimeout = 60 * 1000;
static const int PingInterval = 5 * 1000;

Connection::Connection(QObject* parent /*= nullptr*/)
	: QTcpSocket(parent)
	, writer(this)
{
	greetingMessage = tr("undefined");
	username = tr("unknown");
	state = WaitingForGreeting;
	currentDataType = Undefined;
	transferTimerId = -1;
	isGreetingMessageSent = false;
	pingTimer.setInterval(PingInterval);

	connect(this, &Connection::readyRead, this, &Connection::processReadyRead);
	connect(this, &Connection::disconnected, &pingTimer, &QTimer::stop);
	connect(&pingTimer, &QTimer::timeout, this, &Connection::sendPing);
	connect(this, &QTcpSocket::connected, this, &Connection::sendGreetingMessage);
}

Connection::Connection(qintptr handle, QObject* parent /*= nullptr*/)
	: Connection(parent)
{
	setSocketDescriptor(handle);
	reader.setDevice(this);
}

Connection::~Connection()
{
	if (isGreetingMessageSent)
	{
		writer.endArray();
		waitForBytesWritten(2000);
	}
}

QString Connection::name() const
{
	return username;
}

void Connection::setGreetingMessage(const QString& message)
{
	greetingMessage = message;
}

bool Connection::sendMessage(const QString& message)
{
	if (message.isEmpty())
		return false;

	writer.startMap(1);
	writer.append(PlainText);
	writer.append(message);
	writer.endMap();
	return true;
}

void Connection::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == transferTimerId) {
		abort();
		killTimer(transferTimerId);
		transferTimerId = -1;
	}
}

void Connection::processReadyRead()
{
	reader.reparse();
	while (reader.lastError() == QCborError::NoError)
	{
		if (state == WaitingForGreeting)
		{
			if (!reader.isArray())
				break;

			reader.enterContainer();
			state = ReadingGreeting;
		}
		else if (reader.containerDepth() == 1)
		{
			if (!reader.hasNext())
			{
				reader.leaveContainer();
				disconnectFromHost();
				return;
			}

			if (!reader.isMap() || !reader.isLengthKnown() || reader.length() != 1)
				break;

			reader.enterContainer();
		}
		else if (currentDataType == Undefined)
		{
			if (!reader.isInteger())
				break;
			currentDataType = DataType(reader.toInteger());
			reader.next();
		}
		else
		{
			if (reader.isString())
			{
				auto r = reader.readString();
				buffer += r.data;
				if (r.status != QCborStreamReader::EndOfString)
					continue;
			}
			else if (reader.isNull())
			{
				reader.next();
			}
			else
			{
				break;
			}

			reader.leaveContainer();
			if (transferTimerId != -1)
			{
				killTimer(transferTimerId);
				transferTimerId = -1;
			}

			if (state == ReadingGreeting)
			{
				if (currentDataType != Greeting)
					break;

				processGreeting();
			}
			else
			{
				processData();
			}
		}
	}
}

void Connection::sendPing()
{
	if (pongTimer.elapsed() > PongTimeout)
	{
		abort();
		return;
	}

	writer.startMap(1);
	writer.append(Ping);
	writer.append(nullptr);
	writer.endMap();
}

void Connection::sendGreetingMessage()
{
	writer.startArray();

	writer.startMap(1);
	writer.append(Greeting);
	writer.append(greetingMessage);
	writer.endMap();
	isGreetingMessageSent = true;
	if (!reader.device())
		reader.setDevice(this);
}

void Connection::processGreeting()
{
	username = buffer + '@' + peerAddress().toString() + ':'
		+ QString::number(peerPort());
	currentDataType = Undefined;
	buffer.clear();

	if (!isValid())
	{
		abort();
		return;
	}

	if (!isGreetingMessageSent)
		sendGreetingMessage();

	pingTimer.start();
	pongTimer.start();
	state = ReadyForUse;
	emit readyForUse();
}

void Connection::processData()
{
	switch (currentDataType)
	{
	case PlainText:
		emit newMessage(username, buffer);
		break;
	case Ping:
		writer.startMap(1);
		writer.append(Pong);
		writer.append(nullptr);
		writer.endMap();
		break;
	case Pong:
		pongTimer.restart();
		break;
	default:
		break;
	}

	currentDataType = Undefined;
	buffer.clear();
}
