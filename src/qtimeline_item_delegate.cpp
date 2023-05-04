#include <QDebug>
#include <QPainter>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyleOptionViewItem>

#include "qtimeline_item_delegate.hpp"

QTimeLineItemDelegate::QTimeLineItemDelegate(QObject* parent)
    : QAbstractItemDelegate(parent)
{
}

void QTimeLineItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    const QStandardItemModel* model        = static_cast<const QStandardItemModel*>(index.model());
    QStandardItem*            item         = model->itemFromIndex(index);
    auto                      segmentCount = item->columnCount();
    QColor                    color        = item->data(Qt::DecorationRole).value<QColor>();

    // draw background
    constexpr float thickness = 1.0f;
    QPen            pen(color.darker(300), thickness);
    QBrush          brush(color.darker(250));
    if ((option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver))
        {
            pen.setColor(color.darker(150));
        }
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(option.rect.adjusted(thickness / 2.0f, thickness / 2.0f, -thickness / 2.0f, -thickness / 2.0f));

    for (int i = 0; i < segmentCount; ++i)
        {
            auto   segment = item->child(0, i);
            QRectF rect    = option.rect;
            rect.setLeft(segment->data(Qt::UserRole + 1).toFloat() * 20);
            rect.setRight(rect.left() + segment->data(Qt::UserRole + 2).toFloat() * 20);
            painter->setPen(QPen(color.darker(120), thickness));
            painter->setBrush(QBrush(color));
            painter->drawRect(rect.adjusted(thickness / 2.0f, thickness / 2.0f, -thickness / 2.0f, -thickness / 2.0f));
        }
    painter->restore();
}

QSize QTimeLineItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(-1, 20);
}
