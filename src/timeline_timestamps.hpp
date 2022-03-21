#pragma once

#include <qwidget.h>

namespace tl
{

    class timeline_timestamps : public QWidget
    {
        Q_OBJECT

    public:
        timeline_timestamps(QWidget* parent = nullptr);

        void recalculate(double scale);

    protected:
        void paintEvent(QPaintEvent* evt) override;

    private:
        QString prettify(int seconds) const;

    private:
        // pixels per second
        double _scale;
        double _visualScale;
    };

} // namespace tl