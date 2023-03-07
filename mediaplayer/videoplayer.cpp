#include "videoplayer.h"
#include <QVideoWidget>
#include <QAudioOutput>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <QStyle>

VidePlayer::VidePlayer(QWidget* parent /*= nullptr*/)
	: QWidget(parent)
{
	m_mediaPlayer = new QMediaPlayer(this);
	QVideoWidget* videoWidget = new QVideoWidget;

	QAbstractButton* openButton = new QPushButton(tr("Open..."));
	connect(openButton, &QAbstractButton::clicked, this, &VidePlayer::openFile);

	m_playButton = new QPushButton;
	m_playButton->setEnabled(false);
	m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	connect(m_playButton, &QAbstractButton::clicked, this, &VidePlayer::play);

	m_positionSlider = new QSlider(Qt::Horizontal);
	m_positionSlider->setRange(0, 0);
	connect(m_positionSlider, &QSlider::sliderMoved, this, &VidePlayer::setPosition);

	m_errorLabel = new QLabel;
	m_errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

	QHBoxLayout* controlLayout = new QHBoxLayout;
	controlLayout->setContentsMargins(0, 0, 0, 0);
	controlLayout->addWidget(openButton);
	controlLayout->addWidget(m_playButton);
	controlLayout->addWidget(m_positionSlider);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(videoWidget);
	layout->addLayout(controlLayout);
	layout->addWidget(m_errorLabel);
	setLayout(layout);

	m_mediaPlayer->setVideoOutput(videoWidget);
	connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &VidePlayer::mediaStateChanged);
	connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &VidePlayer::positionChanged);
	connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &VidePlayer::durationChanged);
	connect(m_mediaPlayer, &QMediaPlayer::errorChanged, this, &VidePlayer::handleError);

	QAudioOutput* audioOutput = new QAudioOutput;
	m_mediaPlayer->setAudioOutput(audioOutput);
	audioOutput->setVolume(50);
}

VidePlayer::~VidePlayer()
{

}

void VidePlayer::openFile()
{
	QFileDialog fileDialog(this);
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setWindowTitle(tr("Open Movie"));
	fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));
	if (fileDialog.exec() == QDialog::Accepted)
		setUrl(fileDialog.selectedUrls().constFirst());
}

void VidePlayer::setUrl(const QUrl& url)
{
	m_errorLabel->setText("");
	setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());
	m_mediaPlayer->setSource(url);
	m_playButton->setEnabled(true);
}

void VidePlayer::play()
{
	switch (m_mediaPlayer->playbackState())
	{
	case QMediaPlayer::PlayingState:
		m_mediaPlayer->pause();
		break;
	default:
		m_mediaPlayer->play();
		break;
	}
}

void VidePlayer::mediaStateChanged(QMediaPlayer::PlaybackState state)
{
	switch (state)
	{
	case QMediaPlayer::PlayingState:
		m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
		break;
	default:
		m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
		break;
	}
}

void VidePlayer::positionChanged(qint64 position)
{
	m_positionSlider->setValue(position);
}

void VidePlayer::durationChanged(qint64 duration)
{
	m_positionSlider->setRange(0, duration);
}

void VidePlayer::setPosition(int position)
{
	m_mediaPlayer->setPosition(position);
}

void VidePlayer::handleError()
{
	if (m_mediaPlayer->error() == QMediaPlayer::NoError)
		return;

	m_playButton->setEnabled(false);
	const QString errorStr = m_mediaPlayer->errorString();
	QString message = "Error: ";
	if (errorStr.isEmpty())
		message += " #" + QString::number(int(m_mediaPlayer->error()));
	else
		message += errorStr;
	m_errorLabel->setText(message);
}


