#include "chatdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QTextCursor>
#include <QTextTable>
#include <QScrollBar>
#include <QMessageBox>

ChatDialog::ChatDialog(QWidget* parent /*= nullptr*/)
	: QDialog(parent)
{
	QVBoxLayout *layout = new QVBoxLayout;
	setLayout(layout);

	textEdit = new QTextEdit;
	textEdit->setReadOnly(true);

	memberList = new QListWidget;
	QHBoxLayout* h1 = new QHBoxLayout;
	h1->addWidget(textEdit);
	h1->addWidget(memberList);
	layout->addLayout(h1);

	QLabel *msgLabel = new QLabel(tr("Message"));
	inputEdit = new QLineEdit;
	QHBoxLayout* h2 = new QHBoxLayout;
	h2->addWidget(msgLabel);
	h2->addWidget(inputEdit);
	layout->addLayout(h2);

	textEdit->setFocusPolicy(Qt::NoFocus);
	memberList->setFocusPolicy(Qt::NoFocus);
	inputEdit->setFocusPolicy(Qt::StrongFocus);

	connect(inputEdit, &QLineEdit::returnPressed, this, &ChatDialog::returnPressed);
	connect(&client, &Client::newMessage, this, &ChatDialog::appendMessage);
	connect(&client, &Client::newParticipant, this, &ChatDialog::newParticipant);
	connect(&client, &Client::participantLeft, this, &ChatDialog::participantLeft);

	myNickName == client.nickName();
	newParticipant(myNickName);

	tableFormat.setBorder(0);
	QTimer::singleShot(10 * 1000, this, &ChatDialog::showInformation);
}

ChatDialog::~ChatDialog()
{

}

void ChatDialog::appendMessage(const QString& from, const QString& message)
{
	if (from.isEmpty() || message.isEmpty())
		return;

	QTextCursor cursor(textEdit->textCursor());
	cursor.movePosition(QTextCursor::End);
	QTextTable* table = cursor.insertTable(1, 2, tableFormat);
	table->cellAt(0, 0).firstCursorPosition().insertText('<' + from + '>');
	table->cellAt(0, 1).firstCursorPosition().insertText(message);
	QScrollBar* bar = textEdit->verticalScrollBar();
	bar->setValue(bar->maximum());
}

void ChatDialog::returnPressed()
{
	QString text = inputEdit->text();
	if (text.isEmpty())
		return;

	if (text.startsWith(QChar('/'))) {
		QColor color = textEdit->textColor();
		textEdit->setTextColor(Qt::red);
		textEdit->append(tr("! unknown command: %1").arg(text.left(text.indexOf(' '))));
		textEdit->setTextColor(color);
	}
	else
	{
		client.sendMessage(text);
		appendMessage(myNickName, text);
	}

	inputEdit->clear();
}

void ChatDialog::newParticipant(const QString& nick)
{
	if (nick.isEmpty())
		return;

	QColor textColor = textEdit->textColor();
	textEdit->setTextColor(Qt::gray);
	textEdit->append(tr("* %1 has joined").arg(nick));
	textEdit->setTextColor(textColor);

	memberList->addItem(nick);
}

void ChatDialog::participantLeft(const QString& nick)
{
	if (nick.isEmpty())
		return;

	QList<QListWidgetItem*> items = memberList->findItems(nick, Qt::MatchExactly);
	if (items.isEmpty())
		return;

	delete items.at(0);

	QColor textColor = textEdit->textColor();
	textEdit->setTextColor(Qt::gray);
	textEdit->append(tr("* %1 has left").arg(nick));
	textEdit->setTextColor(textColor);
}

void ChatDialog::showInformation()
{
	if (memberList->count() == 1)
	{
		QMessageBox::information(this, tr("Chat"), tr("Launch server instances of this program on you local network and start chatting!"));
	}
}
