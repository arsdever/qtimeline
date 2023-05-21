#pragma once

#include <QAbstractItemDelegate>

class QTimeLineItemDelegate : public QAbstractItemDelegate
{
public:
    explicit QTimeLineItemDelegate(QObject* parent = nullptr);
    ~QTimeLineItemDelegate() = default;

#pragma region QAbstractItemDelegate

public:
    void  paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

#pragma endregion

private:
    // pixels per second
    float _time_scale = 1.0f;
};
