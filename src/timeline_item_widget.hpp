#pragma once

#include <qwidget.h>
#include <prototypes.hpp>

namespace tl
{

    class timeline_item_widget : public QWidget
    {
        Q_OBJECT

    public:
        timeline_item_widget(const_timeline_item_ptr item, QWidget* parent = nullptr);

        void recalculate(double scale);

    protected:
        bool event(QEvent* evt) override;
        void paintEvent(QPaintEvent* evt) override;

    private:
        const_timeline_item_ptr _item;
        bool                    _hovering;
    };

}