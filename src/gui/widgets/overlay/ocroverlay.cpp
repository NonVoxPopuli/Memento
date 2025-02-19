////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2022 Ripose
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

#include "ocroverlay.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QRubberBand>
#include <QScreen>
#include <QSettings>
#include <QtConcurrent>

#include "util/constants.h"
#include "util/globalmediator.h"

/* Begin Constructor/Destructor */

OCROverlay::OCROverlay(QWidget *parent)
    : QWidget(parent),
      m_rubberBand(new QRubberBand(QRubberBand::Rectangle, this))
{
    setCursor(Qt::CrossCursor);
    setAutoFillBackground(false);

    m_rubberBand->hide();

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    connect(
        &m_resultWatcher, &QFutureWatcher<QString>::finished,
        this, [this] () { Q_EMIT finished(m_resultWatcher.result()); },
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::ocrSettingsChanged,
        this, &OCROverlay::initOCRSettings,
        Qt::DirectConnection
    );
    connect(
        mediator, &GlobalMediator::keyPressed,
        this, &OCROverlay::handleKeyPress,
        Qt::DirectConnection
    );

    initOCRSettings();
}

OCROverlay::~OCROverlay()
{
    delete m_model;
    m_model = nullptr;
}
/* End Constructor/Destructor */
/* Begin Initializers */

void OCROverlay::initOCRSettings()
{
    if (m_model)
    {
        OCRModel *model = m_model;
        m_model = nullptr;
        QtConcurrent::run([model] { delete model; });
    }

    QSettings settings;
    settings.beginGroup(SETTINGS_OCR);

    bool enabled = settings.value(
            SETTINGS_OCR_ENABLE, SETTINGS_OCR_ENABLE_DEFAULT
        ).toBool();
    if (!enabled)
    {
        return;
    }

    QString model = settings.value(
            SETTINGS_OCR_MODEL, SETTINGS_OCR_MODEL_DEFAULT
        ).toString();
    bool useGPU = settings.value(
            SETTINGS_OCR_ENABLE_GPU, SETTINGS_OCR_ENABLE_GPU_DEFAULT
        ).toBool();
    m_model = new OCRModel(model, useGPU);

    settings.endGroup();
}

/* End Initializers */
/* Begin Event Handlers */

void OCROverlay::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    m_startPoint = event->pos();
    m_rubberBand->setGeometry(event->x(), event->y(), 0, 0);
    m_rubberBand->show();
}

void OCROverlay::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    /* TODO: Replace with QRect::span() in Qt6 */
    m_rubberBand->setGeometry(OCROverlay::span(m_startPoint, event->pos()));
}

void OCROverlay::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    if (m_rubberBand->isVisible())
    {
        /* TODO: Replace with QRect::span() in Qt6 */
        getText(OCROverlay::span(m_startPoint, event->pos()));
    }
    m_rubberBand->hide();
    hide();
}

void OCROverlay::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);

    Q_EMIT widgetHidden();
}

void OCROverlay::handleKeyPress(QKeyEvent *event)
{
    if (isHidden() || event->key() != Qt::Key::Key_Escape)
    {
        return;
    }

    m_rubberBand->hide();
    hide();
    event->accept();
}

/* Hide Event Handlers */
/* Begin Helpers */

void OCROverlay::getText(QRect rect)
{
    if (m_model == nullptr)
    {
        return;
    }

    QOpenGLWidget *glWidget = dynamic_cast<QOpenGLWidget *>(parentWidget());
    if (glWidget == nullptr)
    {
        return;
    }
    qreal ratio = screen() ? screen()->devicePixelRatio() : 1.0;
    QImage image{
        glWidget->grabFramebuffer().copy(QRect(
            static_cast<int>(rect.x() * ratio),
            static_cast<int>(rect.y() * ratio),
            static_cast<int>(rect.width() * ratio),
            static_cast<int>(rect.height() * ratio)
        ))
    };
    m_resultWatcher.setFuture(m_model->getText(image));
}

QRect OCROverlay::span(const QPoint &p1, const QPoint &p2)
{
    int x = p1.x() < p2.x() ? p1.x() : p2.x();
    int y = p1.y() < p2.y() ? p1.y() : p2.y();
    int w = std::abs(p1.x() - p2.x());
    int h = std::abs(p1.y() - p2.y());
    return QRect(x, y, w, h);
}

/* End Helpers */
