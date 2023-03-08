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
		ReadForUse
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
	void sendMessage(const QString& message);

signals:
	void readyForUse();
	void newMessage(const QString& from, const QString& message);

protected:
	void timerEvent(QTimerEvent* event) override;

private slots:
	void processReadRead();
	void sendPing();
	void sendGreetingMessage();

private:
	bool hasEnoughData();
	void processGreeting();
	void processData();

	QCborStreamReader reader;
	QCborStreamWriter writer;
	QString greetingMessage;
	QString username;
	QTimer pingTimer;
	QElapsedTimer pongTimer;
	QString buffer;
	DataType currentDataType;
	int transferTimerId;
	bool isGreetingMessageSent;
};
