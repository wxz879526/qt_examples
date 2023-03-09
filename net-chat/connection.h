#pragma once
#include <QTcpSocket>
#include <QCborStreamReader>
#include <QCborStreamWriter>
#include <QTimer>
#include <QElapsedTimer>

class Connection : public QTcpSocket
{
	Q_OBJECT

public:
	enum ConnectionState {
		WaitingForGreeting,
		ReadingGreeting,
		ReadyForUse
	};

	enum DataType {
		PlainText,
		Ping,
		Pong,
		Greeting,
		Undefined
	};

	Connection(QObject* parent = nullptr);
	Connection(qintptr handle, QObject* parent = nullptr);
	~Connection();

	QString name() const;
	void setGreetingMessage(const QString& message);
	bool sendMessage(const QString& message);

signals:
	void readyForUse();
	void newMessage(const QString& from, const QString& message);

protected:
	void timerEvent(QTimerEvent* event) override;

private slots:
	void processReadyRead();
	void sendPing();
	void sendGreetingMessage();

private:
	void processGreeting();
	void processData();

	QCborStreamReader reader;
	QCborStreamWriter writer;
	QString greetingMessage;
	QString username;
	QTimer pingTimer;
	QElapsedTimer pongTimer;
	QString buffer;
	ConnectionState state;
	DataType currentDataType;
	int transferTimerId;
	bool isGreetingMessageSent;
};
