#include <QApplication>
#include "chatdialog.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	ChatDialog chatDialog;
	chatDialog.resize(800, 600);
	chatDialog.show();

	return a.exec();
}