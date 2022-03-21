#pragma once

#include <qwidget.h>
#include <prototypes.hpp>

namespace tl
{

    class single_timeline : public QWidget
    {
        Q_OBJECT

    public:
        single_timeline(QWidget* parent = nullptr);

        void add_element(const_timeline_item_ptr item);

        void recalculate(double scale);

    protected:
        void resizeEvent(QResizeEvent* evt) override;
        void paintEvent(QPaintEvent* evt) override;

    private:
        std::vector<const_timeline_item_ptr> _items;
    };

} // namespace tl