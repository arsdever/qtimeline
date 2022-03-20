#include <QPaintEvent>
#include <QResizeEvent>
#include <memory>
#include <qdebug.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpushbutton.h>

#include "timeline.hpp"

#include "timeline_data_provider.hpp"
#include "timeline_item.hpp"

namespace tl
{

    timeline::timeline(ptr_t<QWidget>::type parent)
        : QWidget { parent }
        , _scale { 1 }
    {
        setLayout(new QVBoxLayout {});

        connect(this, &timeline::scale_changed, this, &timeline::recalculate);
        connect(this, SIGNAL(data_changed()), this, SLOT(on_data_changed()));
    }

    void timeline::set_data_provider(timeline_data_provider_ptr src)
    {
        _data_source = src;
        emit data_changed();
    }

    void timeline::set_scale(double factor)
    {
        _scale = factor;
        emit scale_changed(factor);
    }

    void timeline::on_data_changed()
    {
        if (layout())
            delete layout();
        setLayout(new QVBoxLayout {});

        for (int i = 0; i < _data_source->layer_count(); ++i)
            {
                single_timeline* layer = new single_timeline {};

                for (auto it = _data_source->layer_begin(i); it != _data_source->layer_end(i); ++it)
                    layer->add_element(*it);

                layout()->addWidget(layer);
            }

        recalculate();
    }

    void timeline::recalculate()
    {
        for (int i = 0; i < layout()->count(); ++i)
            {
                single_timeline* line = qobject_cast<single_timeline*>(layout()->itemAt(i)->widget());
                line->recalculate(_scale);
            }
        repaint();
    }

#pragma region single_timeline

    single_timeline::single_timeline(QWidget* parent)
        : QWidget { parent }
    {
    }

    void single_timeline::add_element(const_timeline_item_ptr item) { new timeline_item_widget(item, this); }

    void single_timeline::recalculate(double scale)
    {
        int mw = 0;
        int mh = 0;
        for (auto child : children())
            {
                timeline_item_widget* wdg = qobject_cast<timeline_item_widget*>(child);
                wdg->recalculate(scale);
                mw = std::max(mw, wdg->pos().x() + wdg->minimumWidth());
                mh = std::max(mh, wdg->pos().y() + wdg->minimumHeight());
            }
        setMinimumSize(mw, mh);
    }

    void single_timeline::resizeEvent(QResizeEvent* evt)
    {
        for (auto child : children())
            {
                qobject_cast<QWidget*>(child)->setFixedHeight(evt->size().height());
            }
        QWidget::resizeEvent(evt);
    }

    void single_timeline::paintEvent(QPaintEvent* evt) { QWidget::paintEvent(evt); }

#pragma endregion

#pragma region timeline_item_widget

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
        bg_gradient.setColorAt(0, palette().button().color().lighter(200));
        bg_gradient.setColorAt(1, palette().button().color());
        painter.setBrush(QBrush { bg_gradient });
        if (_hovering)
            painter.setPen(QPen { palette().highlight(), 2 });
        else
            painter.setPen(QPen { palette().buttonText(), 2 });
        painter.drawRoundedRect(rect(), 7, 7);
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

#pragma endregion

} // namespace tl