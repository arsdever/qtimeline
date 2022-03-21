#include <qlayout.h>

#include "timeline.hpp"

#include "single_timeline.hpp"
#include "timeline_data_provider.hpp"
#include "timeline_timestamps.hpp"

namespace tl
{

    timeline::timeline(ptr_t<QWidget>::type parent)
        : QWidget { parent }
        , _scale { 1 }
        , _timestamps { new timeline_timestamps {} }
    //, _timelines { new QVBoxLayout {} }
    {
        setLayout(new QVBoxLayout {});
        layout()->addWidget(_timestamps);
        // layout()->addItem(_timelines);

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
        // for (int i = 0; i < _timelines->count(); ++i)
        //     {
        //         QLayoutItem* item = _timelines->takeAt(i);
        //         _timelines->removeItem(item);
        //         _timelines->removeWidget(item->widget());
        //         delete item->widget();
        //         delete item;
        //     }
        for (int i = 1; i < layout()->count(); ++i)
            {
                QLayoutItem* item = _timelines->takeAt(i);
                layout()->removeItem(item);
                layout()->removeWidget(item->widget());
                delete item->widget();
                delete item;
            }

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
        timeline_timestamps* timestamps = qobject_cast<timeline_timestamps*>(layout()->itemAt(0)->widget());
        timestamps->recalculate(_scale);

        for (int i = 1; i < layout()->count(); ++i)
            {
                single_timeline* line = qobject_cast<single_timeline*>(layout()->itemAt(i)->widget());
                line->recalculate(_scale);
            }
        update();
    }

} // namespace tl