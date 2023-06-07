#include <QAbstractItemView>
#include <QDebug>
#include <QHelpEvent>
#include <QPainter>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyleOptionViewItem>
#include <QToolTip>

#include "qtimeline_item_delegate.hpp"

QTimeLineItemDelegate::QTimeLineItemDelegate(QObject* parent)
    : QAbstractItemDelegate(parent)
{
}

void QTimeLineItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();
    const QStandardItemModel* model     = static_cast<const QStandardItemModel*>(index.model());
    QStandardItem*            item      = model->itemFromIndex(index);
    QStandardItem*            firstItem = model->item(index.row(), 0);
    QColor                    color     = firstItem->data(Qt::DecorationRole).value<QColor>();

    // draw background
    constexpr float thickness = 0.0f;
    QPen            pen(color.darker(300), thickness);
    QBrush          brush(color);
    if ((option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver))
        {
            pen.setColor(color.darker(150));
        }
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(option.rect);
    if ((option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver))
        {
            painter->drawText(option.rect, Qt::AlignCenter, item->data(Qt::ToolTipRole).toString());
        }
    painter->restore();
}

QSize QTimeLineItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return option.rect.size();
}
