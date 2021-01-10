////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "playeradapter.h"

#include "widgets/definitionwidget.h"
#include "widgets/ankisettings.h"
#include "../anki/ankiclient.h"

#include <QMainWindow>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QActionGroup>
#include <QPair>
#include <QDragEnterEvent>
#include <QDropEvent>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

Q_SIGNALS:
    void keyPressed(const QKeyEvent *event);
    void wheelMoved(const QWheelEvent *event);

public Q_SLOTS:
    void setFullscreen(bool value);

private Q_SLOTS:
    void open();
    void setTracks(QVector<const PlayerAdapter::Track *> tracks);
    void setDefinitionWidgetLocation();
    void hideControls();
    void hidePlayerCursor();

protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::MainWindow *m_ui;
    PlayerAdapter *m_player;
    QActionGroup *m_actionGroupAudio;
    QActionGroup *m_actionGroupVideo;
    QActionGroup *m_actionGroupSubtitle;
    QVector<QPair<QAction *, const PlayerAdapter::Track *>> m_audioTracks;
    QVector<QPair<QAction *, const PlayerAdapter::Track *>> m_videoTracks;
    QVector<QPair<QAction *, const PlayerAdapter::Track *>> m_subtitleTracks;
    DefinitionWidget *m_definition;
    AnkiSettings *m_ankiSettings;
    AnkiClient *m_ankiClient;

    void clearTracks();
    void clearTrack(QVector<QPair<QAction *, const PlayerAdapter::Track *>> &tracks, QMenu *menu, QActionGroup *actionGroup, QAction *actionDisable);
};

#endif // MAINWINDOW_H
