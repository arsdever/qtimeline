#include "qtimeline.hpp"

#include "qtimeline_item_delegate.hpp"

QTimeLine::QTimeLine(QWidget* parent)
	: QListView(parent)
{
	setItemDelegate(new QTimeLineItemDelegate(this));
}

QTimeLine::~QTimeLine() = default;
