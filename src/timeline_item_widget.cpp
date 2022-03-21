#include <chrono>
#include <qevent.h>
#include <qpainter.h>

#include "timeline_item_widget.hpp"

#include "timeline_item.hpp"

namespace tl
{

    timeline_item_widget::timeline_item_widget(const_timeline_item_ptr item, QWidget* parent)
        : QWidget { parent }
        , _item { item }
        , _hovering { false }
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        setAttribute(Qt::WA_Hover);
        setMinimumHeight(30);
    }

    void timeline_item_widget::recalculate(double scale)
    {
        double diff  = std::chrono::duration<double> { _item->end() - _item->start() }.count();
        double start = std::chrono::duration<double> { _item->start().time_since_epoch() }.count();
        setFixedWidth(scale * diff);
        move(scale * start, 0);
        repaint();
    }

    void timeline_item_widget::paintEvent(QPaintEvent* evt)
    {
        QPainter painter { this };
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        QLinearGradient bg_gradient(rect().topLeft(), rect().bottomLeft());
        bg_gradient.setColorAt(0, palette().button().color().lighter());
        bg_gradient.setColorAt(1, palette().button().color());
        painter.setBrush(QBrush { bg_gradient });
        if (_hovering)
            painter.setPen(QPen { palette().highlight(), 1 });
        else
            painter.setPen(QPen { palette().buttonText(), 1 });
        painter.drawRoundedRect(rect(), 5, 5);
        painter.setBrush(Qt::NoBrush);
        if (_hovering)
            painter.setPen(QPen { palette().highlightedText(), 1 });
        else
            painter.setPen(QPen { palette().buttonText(), 1 });

        QRect text_rect = rect().adjusted(5, 5, -5, -5);
        painter.drawText(
            text_rect,
            Qt::AlignCenter,
            painter.fontMetrics().elidedText(_item->name().data(), Qt::TextElideMode::ElideRight, text_rect.width()));
        QWidget::paintEvent(evt);
    }

    bool timeline_item_widget::event(QEvent* evt)
    {
        switch (evt->type())
            {
            case QEvent::HoverEnter:
            case QEvent::HoverLeave:
                {
                    if ((evt->type() == QEvent::HoverEnter) != _hovering)
                        {
                            update(rect());
                            _hovering = evt->type() == QEvent::HoverEnter;
                        }
                    return true;
                }
            }

        return QWidget::event(evt);
    }

} // namespace tl