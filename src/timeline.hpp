#pragma once

#include <prototypes.hpp>

#include <qstyle.h>
#include <qwidget.h>
#include <typedef.hpp>

class QLabel;

namespace tl
{

    class timeline_data_provider;
    class single_timeline;

    class timeline : public QWidget
    {
        Q_OBJECT

    public:
        timeline(ptr_t<QWidget>::type parent = nullptr);

        void set_data_provider(timeline_data_provider_ptr src);

        /**
         * @brief Set the scale of the timeline.
         *
         * Scale of the timeline represents the number of pixels per second.
         *
         * @param factor the scale factor
         */
        void set_scale(double unit);

    signals:
        void scale_changed(double newScale);
        void data_changed();

    private slots:
        void on_data_changed();

    private:
        void recalculate();

    private:
        double                     _scale;
        timeline_data_provider_ptr _data_source;
    };

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
        QLabel* _text;
    };

} // namespace tl