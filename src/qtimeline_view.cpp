#include <QEvent>
#include <QHelpEvent>
#include <QHoverEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QToolTip>

#include "qtimeline_view.hpp"

#include "qtimeline_item_delegate.hpp"

QTimeLineView::QTimeLineView(QWidget* parent)
    : QAbstractItemView(parent)
{
    viewport()->setAttribute(Qt::WA_Hover);
    setItemDelegate(new QTimeLineItemDelegate(this));
}

QTimeLineView::~QTimeLineView() = default;

void QTimeLineView::paintEvent(QPaintEvent* event)
{
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing, true);

    // check if header part of the window should be updated
    QRect timestampsRectIntersection = event->rect() & QRect(0, 0, event->rect().width(), _timestampsSectionHeight);
    if (!timestampsRectIntersection.isEmpty())
        {
            painter.setBrush(palette().color(QPalette::Window).lighter(130));
            painter.setPen(Qt::black);

            painter.fillRect(timestampsRectIntersection, painter.brush());
            painter.drawLine(0, _timestampsSectionHeight, event->rect().width(), _timestampsSectionHeight);

            painter.setPen(QPen(palette().color(QPalette::WindowText), 1));
            for (int i = 0; i < event->rect().width(); i += 100)
                {
                    QString text     = tr("%1us").arg(pixelsToDuration(i).count() * 1000000);
                    QRect   textRect = painter.fontMetrics().boundingRect(text);

                    textRect.translate(i - textRect.width() / 2, _timestampsSectionHeight - 13);
                    if (textRect.right() > event->rect().width() - 5)
                        {
                            textRect.translate(event->rect().width() - textRect.right() - 5, 0);
                        }
                    else if (textRect.left() < 5)
                        {
                            textRect.translate(-textRect.left(), 0);
                        }

                    painter.drawLine(i, _timestampsSectionHeight - 10, i, _timestampsSectionHeight);
                    painter.drawText(textRect, text);
                }
        }

    QStyleOptionViewItem option;
    QAbstractItemView::initViewItemOption(&option);
    for (int i = 0; i < model()->rowCount(); ++i)
        {
            auto   item        = model()->index(i, 0);
            QColor bgPenColor  = item.data(Qt::DecorationRole).value<QColor>().darker(200);
            QColor bgFillColor = item.data(Qt::DecorationRole).value<QColor>().darker(150);
            painter.setPen(bgPenColor);
            painter.fillRect(
                0, i * _layerHeight + _timestampsSectionHeight, event->rect().width(), _layerHeight, bgFillColor);
            painter.drawLine(0,
                             i * _layerHeight + _timestampsSectionHeight,
                             event->rect().width(),
                             i * _layerHeight + _timestampsSectionHeight);
            for (int j = 0; j < model()->columnCount(); ++j)
                {
                    auto segment = model()->index(i, j);
                    if (!segment.isValid())
                        {
                            continue;
                        }

                    option.rect = visualRect(segment);
                    option.state.setFlag(QStyle::State_MouseOver, segment == _hoverIndex);
                    if (option.rect.intersects(event->rect()))
                        {
                            option.rect = visualRect(segment) & event->rect();
                            itemDelegate()->paint(&painter, option, segment);
                        }
                }
        }
}

QModelIndex QTimeLineView::indexAt(const QPoint& point) const
{
    int row = (point.y() - _timestampsSectionHeight) / _layerHeight;
    for (int i = 0; i < model()->columnCount(); ++i)
        {
            if (visualRect(model()->index(row, i)).contains(point))
                {
                    return model()->index(row, i);
                }
        }
    return {};
}

void QTimeLineView::scrollTo(const QModelIndex& index, ScrollHint hint) { }

QRect QTimeLineView::visualRect(const QModelIndex& index) const
{
    double startTime = index.data(Qt::UserRole + 1).toDouble();
    double duration  = index.data(Qt::UserRole + 2).toDouble();
    int    x         = durationToPixels(std::chrono::duration<double>(startTime));
    int    width     = durationToPixels(std::chrono::duration<double>(duration));
    return { x, index.row() * _layerHeight + _timestampsSectionHeight, width, _layerHeight };
}

int QTimeLineView::horizontalOffset() const { return 0; }

bool QTimeLineView::isIndexHidden(const QModelIndex& index) const { return false; }

QModelIndex QTimeLineView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) { return {}; }

void QTimeLineView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags flags) { }

int QTimeLineView::verticalOffset() const { return 0; }

QRegion QTimeLineView::visualRegionForSelection(const QItemSelection& selection) const { return {}; }

bool QTimeLineView::viewportEvent(QEvent* event)
{
    switch (event->type())
        {
        case QEvent::HoverMove:
        case QEvent::HoverEnter:
            {
                update(_hoverIndex);
                _hoverIndex = indexAt(static_cast<QHoverEvent*>(event)->position().toPoint());
                if (_hoverIndex.isValid())
                    {
                        _hoverIndex = indexAt(static_cast<QHoverEvent*>(event)->position().toPoint());
                        update(_hoverIndex);
                    }
                break;
            }
        case QEvent::HoverLeave:
            {
                update(_hoverIndex);
                _hoverIndex = QModelIndex {};
                update(_hoverIndex);
                break;
            }
        case QEvent::ToolTip:
        case QEvent::QueryWhatsThis:
        case QEvent::WhatsThis:
            {
                QHelpEvent*          he    = static_cast<QHelpEvent*>(event);
                const QModelIndex    index = indexAt(he->pos());
                QStyleOptionViewItem option;
                initViewItemOption(&option);
                option.rect = visualRect(index);
                option.state |= (index == currentIndex() ? QStyle::State_HasFocus : QStyle::State_None);

                QAbstractItemDelegate* delegate = itemDelegateForIndex(index);
                if (!delegate)
                    return false;
                return delegate->helpEvent(he, this, option, index);
            }
        default:
            {
                break;
            }
        }
    return QAbstractScrollArea::viewportEvent(event);
}

double QTimeLineView::durationToPixels(std::chrono::duration<double> value) const
{
    return value.count() * _scale * 1000;
}

std::chrono::duration<double> QTimeLineView::pixelsToDuration(int value) const
{
    return std::chrono::duration<double>(value / _scale / 1000);
}
