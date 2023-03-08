#pragma once

#include <QDialog>
#include <QTextTableFormat>

#include "client.h"

class QTextEdit;
class QListWidget;

class ChatDialog : public QDialog
{
	Q_OBJECT

public:
	ChatDialog(QWidget* parent = nullptr);
	~ChatDialog();

public slots:
	void appendMessage(const QString& from, const QString& message);

private slots:
	void returnPressed();
	void newParticipant(const QString &nick);
	void participantLeft(const QString& nick);
	void showInformation();

private:
	Client client;
	QString myNickName;
	QTextTableFormat tableFormat;

	QTextEdit* textEdit;
	QListWidget* memberList;
	QLineEdit* inputEdit;
};

