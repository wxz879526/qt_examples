#pragma once

#include <QHostAddress>
#include <QUdpSocket>
#include <QTimer>

class Client;
class Connection;

class PeerManager : public QObject {
	Q_OBJECT

public:
	PeerManager(Client* client);
	~PeerManager();

	void setServerPort(int port);
	QString userName() const;
	void startBroadcasting();
	bool isLocalHostAddress(const QHostAddress& address) const;

signals:
	void newConnection(Connection* connection);

private slots:
	void sendBroadcastDatagram();
	void readBroadcastDatagram();

private:
	void updateAddress();

	Client* client;
	QList<QHostAddress> broadcastAddresses;
	QList<QHostAddress> ipAddresses;
	QUdpSocket broadcastSocket;
	QTimer broadcastTimer;
	QString username;
	int serverPort;
};