#include "peermanager.h"
#include "client.h"
#include "connection.h"
#include <QCborStreamWriter>
#include <QCborStreamReader>
#include <QNetworkInterface>

static const qint32 BroadcastInterval = 2000;
static const unsigned broadcastPort = 45000;

PeerManager::PeerManager(Client* client)
	: QObject(client)
{
	this->client = client;

	static const char* envVariables[] = {
		"USERNAME", "USER", "USERDOMAIN", "HOSTNAME", "DOMAINNAME"
	};

	for (const char *var : envVariables)
	{
		username = qEnvironmentVariable(var);
		if (!username.isNull())
			break;
	}

	if (username.isEmpty())
		username = "unknown";

	updateAddress();
	serverPort = 0;

	broadcastSocket.bind(QHostAddress::Any, broadcastPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
	connect(&broadcastSocket, &QUdpSocket::readyRead, this, &PeerManager::readBroadcastDatagram);

	broadcastTimer.setInterval(BroadcastInterval);
	connect(&broadcastTimer, &QTimer::timeout, this, &PeerManager::sendBroadcastDatagram);
}

PeerManager::~PeerManager()
{

}

void PeerManager::setServerPort(int port)
{
	serverPort = port;
}

QString PeerManager::userName() const
{
	return username;
}

void PeerManager::startBroadcasting()
{
	broadcastTimer.start();
}

bool PeerManager::isLocalHostAddress(const QHostAddress& address) const
{
	for (const QHostAddress &localAddress : ipAddresses)
	{
		if (address.isEqual(localAddress))
			return true;
	}

	return false;
}

void PeerManager::sendBroadcastDatagram()
{
	QByteArray datagram;
	{
		QCborStreamWriter writer(&datagram);
		writer.startArray(2);
		writer.append(username);
		writer.append(serverPort);
		writer.endArray();
	}

	bool validBroadcastAddresses = true;
	for (const QHostAddress &address : qAsConst(broadcastAddresses))
	{
		if (broadcastSocket.writeDatagram(datagram, address, broadcastPort) == -1)
			validBroadcastAddresses = false;
	}

	if (!validBroadcastAddresses)
		updateAddress();
}

void PeerManager::readBroadcastDatagram()
{
	while (broadcastSocket.hasPendingDatagrams())
	{
		QHostAddress senderIp;
		quint16 senderPort;
		QByteArray datagram;
		datagram.resize(broadcastSocket.pendingDatagramSize());
		if (broadcastSocket.readDatagram(datagram.data(), datagram.size(), &senderIp, &senderPort) == -1)
			continue;

		int senderSeverPort;
		{
			QCborStreamReader reader(datagram);
			if (reader.lastError() != QCborError::NoError || !reader.isArray())
				continue;

			if (!reader.isLengthKnown() || reader.length() != 2)
				continue;

			reader.enterContainer();

			if (reader.lastError() != QCborError::NoError || !reader.isString())
				continue;

			while (reader.readString().status == QCborStreamReader::Ok)
			{
			}

			if (reader.lastError() != QCborError::NoError || !reader.isUnsignedInteger())
				continue;

			senderSeverPort = reader.toInteger();
			reader.next();
			reader.leaveContainer();
		}

		if (isLocalHostAddress(senderIp) && senderSeverPort == serverPort)
			continue;

		if (!client->hasConnection(senderIp)) {
			Connection* connection = new Connection(this);
			emit newConnection(connection);
			connection->connectToHost(senderIp, senderSeverPort);
		}
	}
}

void PeerManager::updateAddress()
{
	broadcastAddresses.clear();
	ipAddresses.clear();
	const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	for (const QNetworkInterface &interface : interfaces)
	{
		const QList<QNetworkAddressEntry> entries = interface.addressEntries();
		for (const QNetworkAddressEntry &entry : entries)
		{
			QHostAddress broadcastAddress = entry.broadcast();
			if (broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost)
			{
				broadcastAddresses << broadcastAddress;
				ipAddresses << entry.ip();
			}
		}
	}
}
