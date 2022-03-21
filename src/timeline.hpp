#pragma once

#include <prototypes.hpp>

#include <qstyle.h>
#include <qwidget.h>
#include <typedef.hpp>

class QLabel;

namespace tl
{

    class timeline_timestamps;
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
        timeline_timestamps*       _timestamps;
        QLayout*                   _timelines;
    };

} // namespace tl