#pragma once
#include <QTcpServer>

class Connection;

class Server : public QTcpServer
{
	Q_OBJECT

public:
	Server(QObject* parent = nullptr);
	~Server();

signals:
	void newConnection(Connection* connection);

protected:
	void incomingConnection(qintptr handle) override;
};

