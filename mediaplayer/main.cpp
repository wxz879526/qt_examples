#include <QApplication>
#include <QCommandLineParser>
#include <QScreen>
#include <QDir>
#include "videoplayer.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setApplicationName("Video Widget Example");
	QCoreApplication::setOrganizationName("QtProject");
	QGuiApplication::setApplicationDisplayName(QCoreApplication::applicationName());
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);

	QCommandLineParser parser;
	parser.setApplicationDescription("Video Widget Example");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("uri", "The URL to open.");
	parser.process(a);

	VidePlayer player;
	if (!parser.positionalArguments().isEmpty())
	{
		const QUrl url = QUrl::fromUserInput(parser.positionalArguments().constFirst(), QDir::currentPath(), QUrl::AssumeLocalFile);
		player.setUrl(url);
	}

	const QSize availableGeometry = player.screen()->availableSize();
	player.resize(availableGeometry.width() / 6, availableGeometry.height() / 6);
	player.show();

	return a.exec();
}