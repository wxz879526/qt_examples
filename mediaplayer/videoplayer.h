#pragma once

#include <QMediaPlayer>
#include <QWidget>

class QAbstractButton;
class QSlider;
class QLabel;

class VidePlayer : public QWidget
{
	Q_OBJECT

public:
	VidePlayer(QWidget* parent = nullptr);
	~VidePlayer();

	void setUrl(const QUrl& url);

public slots:
	void openFile();
	void play();

private slots:
	void mediaStateChanged(QMediaPlayer::PlaybackState state);
	void positionChanged(qint64 position);
	void durationChanged(qint64 duration);
	void setPosition(int position);
	void handleError();

private:
	QMediaPlayer* m_mediaPlayer;
	QAbstractButton* m_playButton;
	QSlider* m_positionSlider;
	QLabel* m_errorLabel;
};

