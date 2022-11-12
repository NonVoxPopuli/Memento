////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#include "searchwidget.h"

#include <QGuiApplication>
#include <QSettings>
#include <QThreadPool>
#include <QVBoxLayout>

#include "dict/dictionary.h"
#include "gui/widgets/definition/definitionwidget.h"
#include "util/constants.h"
#include "util/globalmediator.h"
#include "util/utils.h"

/* Prevents large searches from being executed and freezing everything up */
#define MAX_SEARCH_SIZE 40

/* Begin SearchEdit Class */

SearchEdit::SearchEdit(Qt::KeyboardModifier modifier, QWidget *parent)
    : QLineEdit(parent), m_modifier(modifier)
{
    connect(this, &QLineEdit::textChanged, this, [this] { m_lastIndex = -1; });
}

void SearchEdit::setModifier(Qt::KeyboardModifier modifier)
{
    m_modifier = modifier;
}

void SearchEdit::mouseMoveEvent(QMouseEvent *event)
{
    QLineEdit::mouseMoveEvent(event);
    if (!(QGuiApplication::keyboardModifiers() & m_modifier))
    {
        return;
    }

    int i = cursorPositionAt(event->pos());
    if (i == m_lastIndex)
    {
        return;
    }

    m_lastIndex = i;
    Q_EMIT searchTriggered(text(), m_lastIndex);
}

/* End SearchEdit Class */
/* Begin SearchWidget */

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent),
      m_dictionary(GlobalMediator::getGlobalMediator()->getDictionary())
{
    if (m_dictionary == nullptr)
    {
        m_dictionary = new Dictionary;
    }

    setWindowTitle("Term Search");
    setAutoFillBackground(true);

    m_layoutParent = new QVBoxLayout(this);

    m_searchEdit = new SearchEdit;
    m_searchEdit->setPlaceholderText("Search");
    m_layoutParent->addWidget(m_searchEdit);

    m_definition = new DefinitionWidget;
    m_definition->layout()->setContentsMargins(QMargins(0, 0, 0, 0));
    m_definition->setMinimumSize(300, 300);
    m_definition->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_layoutParent->addWidget(m_definition);

    connect(
        m_searchEdit, &QLineEdit::textEdited,
        this, qOverload<const QString &>(&SearchWidget::updateSearch),
        Qt::QueuedConnection
    );
    connect(
        m_searchEdit, &SearchEdit::searchTriggered,
        this, qOverload<const QString &, int>(&SearchWidget::updateSearch),
        Qt::QueuedConnection
    );
    connect(
        this, &SearchWidget::searchUpdated,
        m_definition, &DefinitionWidget::setTerms,
        Qt::QueuedConnection
    );

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();
    connect(
        mediator, &GlobalMediator::interfaceSettingsChanged,
        this, [this] { updateSearch(m_searchEdit->text()); },
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::searchSettingsChanged,
        this, &SearchWidget::initSettings,
        Qt::QueuedConnection
    );

    initSettings();
}

void SearchWidget::initSettings()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_SEARCH);

    QString modifier = settings.value(
            SETTINGS_SEARCH_MODIFIER,
            SETTINGS_SEARCH_MODIFIER_DEFAULT
        ).toString();
    if (modifier == SEARCH_MODIFIER_ALT)
    {
        m_searchEdit->setModifier(Qt::KeyboardModifier::AltModifier);
    }
    else if (modifier == SEARCH_MODIFIER_CTRL)
    {
        m_searchEdit->setModifier(Qt::KeyboardModifier::ControlModifier);
    }
    else if (modifier == SEARCH_MODIFIER_SHIFT)
    {
        m_searchEdit->setModifier(Qt::KeyboardModifier::ShiftModifier);
    }
    else if (modifier == SEARCH_MODIFIER_SUPER)
    {
        m_searchEdit->setModifier(Qt::KeyboardModifier::MetaModifier);
    }
    else
    {
        m_searchEdit->setModifier(Qt::KeyboardModifier::ShiftModifier);
    }

    settings.endGroup();
}

void SearchWidget::setSearch(const QString &text)
{
    m_searchEdit->setText(text);
    updateSearch(text);
}

void SearchWidget::updateSearch(const QString &text)
{
    updateSearch(text, 0);
}

void SearchWidget::updateSearch(const QString &text, const int index)
{
    QThreadPool::globalInstance()->start(
        [=] {
            QString query = text.mid(index, MAX_SEARCH_SIZE);
            SharedTermList terms =
                m_dictionary->searchTerms(query, text, index, &index);

            SharedKanji kanji = nullptr;
            if (!text.isEmpty() && CharacterUtils::isKanji(text[0]))
            {
                kanji = SharedKanji(m_dictionary->searchKanji(text[0]));
            }

            Q_EMIT searchUpdated(terms, kanji);
        }
    );
}

/* End SearchWidget */
