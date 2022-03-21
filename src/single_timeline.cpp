#include "single_timeline.hpp"

#include <QResizeEvent>

#include "timeline_item.hpp"
#include "timeline_item_widget.hpp"

namespace tl
{

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

} // namespace tl