/*
 *   Copyright 2008 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "lineedit.h"

#include <QPainter>

#include <klineedit.h>
#include <kmimetype.h>

#include <plasma/private/style_p.h>

#include "theme.h"
#include "svg.h"
#include "framesvg.h"

namespace Plasma
{

class LineEditPrivate
{
public:
    LineEditPrivate(LineEdit *lineEdit)
        :q(lineEdit),
         customFont(false)
    {
    }

    ~LineEditPrivate()
    {
    }

    void setPalette()
    {
        KLineEdit *native = q->nativeWidget();
        QColor color = Theme::defaultTheme()->color(Theme::TextColor);
        QPalette p = native->palette();

        p.setColor(QPalette::Normal, QPalette::Text, color);
        p.setColor(QPalette::Inactive, QPalette::Text, color);
        native->setPalette(p);
        native->setFont(Plasma::Theme::defaultTheme()->font(Plasma::Theme::DefaultFont));

        if (!customFont) {
            q->nativeWidget()->setFont(Plasma::Theme::defaultTheme()->font(Plasma::Theme::DefaultFont));
        }
    }

    LineEdit *q;
    Plasma::Style::Ptr style;
    Plasma::FrameSvg *background;
    bool customFont;
};

LineEdit::LineEdit(QGraphicsWidget *parent)
    : QGraphicsProxyWidget(parent),
      d(new LineEditPrivate(this))
{
    KLineEdit *native = new KLineEdit;
    d->style = Plasma::Style::sharedStyle();
    d->background = new Plasma::FrameSvg(this);
    d->background->setImagePath("widgets/lineedit");

    native->setStyle(d->style.data());
    native->setAttribute(Qt::WA_NoSystemBackground);
    setWidget(native);

    connect(native, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
    connect(native, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));
    connect(native, SIGNAL(textEdited(const QString&)), this, SIGNAL(textEdited(const QString&)));
    connect(native, SIGNAL(textChanged(const QString&)), this, SIGNAL(textChanged(const QString&)));

    d->setPalette();
    connect(Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(setPalette()));
}

LineEdit::~LineEdit()
{
    delete d;
    Plasma::Style::doneWithSharedStyle();
}

void LineEdit::setText(const QString &text)
{
    static_cast<KLineEdit*>(widget())->setText(text);
}

QString LineEdit::text() const
{
    return static_cast<KLineEdit*>(widget())->text();
}

void LineEdit::setClearButtonShown(bool show)
{
    nativeWidget()->setClearButtonShown(show);
}

bool LineEdit::isClearButtonShown() const
{
    return nativeWidget()->isClearButtonShown();
}

void LineEdit::setStyleSheet(const QString &stylesheet)
{
    widget()->setStyleSheet(stylesheet);
}

QString LineEdit::styleSheet()
{
    return widget()->styleSheet();
}

KLineEdit *LineEdit::nativeWidget() const
{
    return static_cast<KLineEdit*>(widget());
}

void LineEdit::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    update();
}

void LineEdit::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    update();
}

void LineEdit::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (hasFocus() || isUnderMouse()) {
        if (hasFocus()) {
            d->background->setElementPrefix("focus");
        } else {
            d->background->setElementPrefix("hover");
        }
        qreal left, top, right, bottom;
        d->background->getMargins(left, top, right, bottom);
        d->background->resizeFrame(size()+QSizeF(left+right, top+bottom));
        d->background->paintFrame(painter, QPoint(-left, -top));
    }

    nativeWidget()->render(painter, QPoint(0, 0), QRegion(), QWidget::DrawChildren|QWidget::IgnoreMask);
}

void LineEdit::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::FontChange) {
        d->customFont = true;
        nativeWidget()->setFont(font());
    }

    QGraphicsProxyWidget::changeEvent(event);
}

} // namespace Plasma

#include <lineedit.moc>

