#include "client.h"
#include "connection.h"
#include "peermanager.h"

#include <QHostInfo>

Client::Client()
{
	peerManager = new PeerManager(this);
	peerManager->setServerPort(server.serverPort());
	peerManager->startBroadcasting();

	connect(peerManager, &PeerManager::newConnection, this, &Client::newConnection);
	connect(&server, &Server::newConnection, this, &Client::newConnection);
}

Client::~Client()
{

}

void Client::sendMessage(const QString& message)
{
	if (message.isEmpty())
		return;

	for (Connection* connection : std::as_const(peers))
		connection->sendMessage(message);
}

QString Client::nickName() const
{
	return peerManager->userName() + '@' + QHostInfo::localHostName() + ":" + QString::number(server.serverPort());
}

bool Client::hasConnection(const QHostAddress& senderIp, int senderPort /*= -1*/) const
{

}

void Client::connectionError(QAbstractSocket::SocketError socketErr)
{

}

void Client::disconnected()
{

}

void Client::readForUse()
{

}

void Client::removeConnection(Connection* connection)
{

}
