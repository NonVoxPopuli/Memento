#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QWidget>

#include "../../util/iconfactory.h"

namespace Ui
{
    class PlayerControls;
}

struct Entry;

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    PlayerControls(QWidget *parent = 0);
    ~PlayerControls();

public Q_SLOTS:
    void setDuration(const int value);
    void setPaused(const bool paused);
    void setFullscreen(const bool value);
    void setVolumeLimit(const int value);
    void setVolume(const int value);
    void setPosition(const int value);
    void setSubtitle(const QString &subtitle, const int end);

Q_SIGNALS:
    void play();
    void pause();
    void stop();
    void seekForward();
    void seekBackward();
    void skipForward();
    void skipBackward();
    void sliderMoved(const int value);
    void volumeSliderMoved(const int value);
    void fullscreenChanged(const bool value);
    void entryChanged(const Entry *entry);
    void definitionHidden();
    void hideDefinition();

private Q_SLOTS:
    void pauseResume();
    void toggleFullscreen();

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::PlayerControls *m_ui;
    bool m_paused;
    bool m_fullscreen;
    int m_endtime;
    IconFactory *m_iconFactory;

    QString formatTime(int time);
    void setIcons();
};

#endif // PLAYERCONTROLS_H