#include <QPaintEvent>
#include <qpainter.h>

#include "timeline_timestamps.hpp"

namespace tl
{

    timeline_timestamps::timeline_timestamps(QWidget* parent)
        : QWidget { parent }
        , _scale { 1 }
    {
        setFixedHeight(30);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    void timeline_timestamps::recalculate(double scale)
    {
        _scale       = scale;
        _visualScale = _scale;
        while (_visualScale < 30)
            {
                _visualScale *= 2.0;
            }
        while (_visualScale > 60)
            {
                _visualScale /= 2.0;
            }
    }

    void timeline_timestamps::paintEvent(QPaintEvent* evt)
    {
        QPainter painter { this };
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        QLinearGradient gradient(evt->rect().topLeft(), evt->rect().bottomLeft());
        gradient.setColorAt(0, palette().window().color());
        gradient.setColorAt(1, palette().window().color().darker());
        painter.setBrush(QBrush { gradient });
        painter.setPen(Qt::NoPen);
        painter.drawRect(evt->rect());
        painter.setPen(QPen { palette().buttonText().color(), 2 });
        painter.drawLine(evt->rect().bottomLeft(), evt->rect().bottomRight());
        painter.setPen(palette().buttonText().color());

        int first_line = evt->rect().left() / _visualScale;
        int last_line  = evt->rect().right() / _visualScale + 1;
        for (; first_line <= last_line; ++first_line)
            {
                QString time_string { prettify(first_line * _visualScale / _scale) };
                painter.drawText(
                    QPointF { first_line * _visualScale -
                                  painter.fontMetrics().size(Qt::TextSingleLine, time_string).width() / 2.0,
                              10 },
                    time_string);
                painter.drawLine(first_line * _visualScale, 20, first_line * _visualScale, 30);
            }

        return QWidget::paintEvent(evt);
    }

    QString timeline_timestamps::prettify(int seconds) const
    {
        QString result { "%1:%2" };
        int     minutes = 0;
        while (seconds > 59)
            {
                ++minutes;
                seconds -= 60;
            }

        return result.arg(minutes, 2, 10, QChar { '0' }).arg(seconds, 2, 10, QChar { '0' });
    }

} // namespace tl