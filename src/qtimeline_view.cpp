#include <QEvent>
#include <QHelpEvent>
#include <QHoverEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
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
            for (int i = static_cast<int>(_scrollOffset.x() / 100 + 1) * 100;
                 i < event->rect().width() + _scrollOffset.x();
                 i += 100)
                {
                    // int     position = i + _scrollOffset.x() / 100;
                    QString text     = tr("%1us").arg(pixelsToDuration(i).count() * 1000000);
                    QRect   textRect = painter.fontMetrics().boundingRect(text);

                    textRect.translate(-_scrollOffset.x(), 0);
                    textRect.translate(i - textRect.width() / 2, _timestampsSectionHeight - 13);
                    if (textRect.right() > event->rect().width() - 5)
                        {
                            textRect.translate(event->rect().width() - textRect.right() - 5, 0);
                        }
                    else if (textRect.left() < 5)
                        {
                            textRect.translate(-textRect.left() + 5, 0);
                        }

                    painter.drawLine(i - _scrollOffset.x(),
                                     _timestampsSectionHeight - 10,
                                     i - _scrollOffset.x(),
                                     _timestampsSectionHeight);
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
                    // option.rect.translate(-_scrollOffset.x(), -_scrollOffset.y());
                    option.state.setFlag(QStyle::State_MouseOver, segment == _hoverIndex);
                    if (i == 0 && j == 1)
                        qDebug() << (option.state & QStyle::State_MouseOver) << event->rect() << option.rect
                                 << visualRect(segment) << _scrollOffset;
                    if (option.rect.intersects(event->rect()))
                        {
                            // option.rect = visualRect(segment) & event->rect();
                            option.rect = option.rect & event->rect();
                            qDebug() << (option.state & QStyle::State_MouseOver) << option.rect;
                            itemDelegate()->paint(&painter, option, segment);
                        }
                }
        }

    for (int i = static_cast<int>((event->rect().left() - _scrollOffset.x()) / 100 + 1) * 100;
         i < event->rect().right() + _scrollOffset.x();
         i += 100)
        {
            qDebug() << i;
            painter.setPen(QColor(0, 0, 0, 50));
            painter.drawLine(i - _scrollOffset.x(),
                             std::max(_timestampsSectionHeight, event->rect().top()),
                             i - _scrollOffset.x(),
                             event->rect().bottom());
        }
}

void QTimeLineView::resizeEvent(QResizeEvent* event)
{
    updateScrollBars();
    QAbstractItemView::resizeEvent(event);
}

void QTimeLineView::showEvent(QShowEvent* event)
{
    updateScrollBars();
    QAbstractItemView::showEvent(event);
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

QRect QTimeLineView::visualRect(const QModelIndex& index) const { return itemRect(index).translated(-_scrollOffset); }

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
                option.rect = visualRect(index).translated(-_scrollOffset.x(), -_scrollOffset.y());
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

void QTimeLineView::scrollContentsBy(int dx, int dy)
{
    _scrollOffset -= QPoint(dx, dy);
    qDebug() << _scrollOffset;
    QAbstractItemView::scrollContentsBy(dx, dy);
}

QRect QTimeLineView::itemRect(const QModelIndex& index) const
{
    double startTime = index.data(Qt::UserRole + 1).toDouble();
    double duration  = index.data(Qt::UserRole + 2).toDouble();
    int    x         = durationToPixels(std::chrono::duration<double>(startTime));
    int    width     = durationToPixels(std::chrono::duration<double>(duration));
    return QRect(x, index.row() * _layerHeight + _timestampsSectionHeight, width, _layerHeight);
}

void QTimeLineView::updateScrollBars()
{
    if (model() == nullptr)
        return;

    int max = 0;
    for (int i = 0; i < model()->rowCount(); ++i)
        {
            auto item = model()->index(i, model()->columnCount() - 1);
            if (!item.isValid())
                {
                    continue;
                }
            max = std::max(max, itemRect(item).right() - viewport()->width());
        }

    horizontalScrollBar()->setRange(0, max);
    // verticalScrollBar()->setRange(0, model()->rowCount() * _layerHeight);
}

double QTimeLineView::durationToPixels(std::chrono::duration<double> value) const
{
    return value.count() * _scale * 1000;
}

std::chrono::duration<double> QTimeLineView::pixelsToDuration(int value) const
{
    return std::chrono::duration<double>(value / _scale / 1000);
}
