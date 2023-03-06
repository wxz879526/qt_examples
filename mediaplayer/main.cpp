#include <QApplication>
#include <QWidget>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QWidget* w = new QWidget;
	w->resize(800, 600);
	w->show();

	return a.exec();
}