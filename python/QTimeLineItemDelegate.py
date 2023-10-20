from PyQt5.QtGui import QStandardItemModel, QPen, QBrush
from PyQt5.QtWidgets import QAbstractItemDelegate, QStyle
from PyQt5.QtCore import Qt


class QTimeLineItemDelegate(QAbstractItemDelegate):

    def __init__(self):
        super().__init__()
        self.time_scale = 1.

    def paint(self, painter, option, index):
        painter.save()
        # model = QStandardItemModel(index.model())
        # item = model.itemFromIndex(index)
        # firstItem = model.item(index.row(), 0)
        # color = firstItem.data(Qt.DecorationRole).value()
        color = index.data(Qt.DecorationRole)

        thickness = 0.0
        pen = QPen(color.darker(300), thickness)
        brush = QBrush(color)
        # if (option.state & QStyle.State_Selected) or (option.state & QStyle.State_MouseOver):
        #     pen.setColor(color.darker(150))
        painter.setPen(pen)
        painter.setBrush(brush)
        painter.drawRect(option.rect)
        # if (option.state & QStyle.State_Selected) or (option.state & QStyle.State_MouseOver):
        painter.drawText(option.rect, Qt.AlignCenter, index.data(Qt.ToolTipRole))
        painter.restore()

    def sizeHint(self, option, index):
        return option.rect.size()
