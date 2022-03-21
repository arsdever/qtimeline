#include <qlayout.h>

#include "timeline.hpp"

#include "single_timeline.hpp"
#include "timeline_data_provider.hpp"

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

} // namespace tl