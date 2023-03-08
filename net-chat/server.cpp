#include "server.h"
#include "connection.h"

Server::Server(QObject* parent /*= nullptr*/)
	: QTcpServer(parent)
{
	listen();
}

Server::~Server()
{

}

void Server::incomingConnection(qintptr handle)
{
	Connection* connection = new Connection(handle, this);
	emit newConnection(connection);
}
