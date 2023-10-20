from PyQt5.QtCore import QPersistentModelIndex, QPoint, QRect, QEvent, QModelIndex, Qt, QRectF
from PyQt5.QtGui import QHoverEvent, QHelpEvent, QPainter, QPalette, QPen, QColor, QColorConstants, QRegion, QKeyEvent, \
    QWheelEvent
from PyQt5.QtWidgets import (QListView,
                             QAbstractItemView,
                             QTableView,
                             QTreeView, QStyleOptionViewItem, QStyle, QAbstractScrollArea)

from QTimeLineItemDelegate import QTimeLineItemDelegate


class QTimeLineView(QAbstractItemView):

    def __init__(self):

        super().__init__()
        self.hoverIndex = QPersistentModelIndex()
        self.scrollOffset = QPoint()

        self.scale: float = 1.
        self.timestempSectionHeight: int = 30
        self.timestampPer100Pixels: int = 1.0
        self.layerHeight: int = 20

        self.viewport().setAttribute(Qt.WA_Hover)
        self.setItemDelegate(QTimeLineItemDelegate())
        self.horizontalScrollBar().setSingleStep(10)
        self.horizontalScrollBar().setPageStep(100)
        self.verticalScrollBar().setSingleStep(self.layerHeight)
        self.verticalScrollBar().setPageStep(self.layerHeight * 5)
        self.viewport().setMinimumHeight(self.layerHeight + self.timestempSectionHeight)

    def setScale(self, val: float):
        self.scale = val
        self.updateScrollBars()
        self.viewport().update()

    def paintEvent(self, a0):
        painter = QPainter(self.viewport())
        painter.setRenderHint(QPainter.Antialiasing, True)

        timestempsRectIntersection = a0.rect() & QRect(0, 0, a0.rect().width(), self.timestempSectionHeight)
        if not timestempsRectIntersection.isEmpty():
            painter.setBrush(self.palette().color(QPalette.Window).lighter(130))
            painter.setPen(QColorConstants.Black)

            painter.fillRect(timestempsRectIntersection, painter.brush())
            painter.drawLine(0, self.timestempSectionHeight, a0.rect().width(), self.timestempSectionHeight)

            painter.setPen(QPen(self.palette().color(QPalette.WindowText), 1))
            for i in range(int(self.scrollOffset.x()/ 100 + 1) * 100, a0.rect().width() + self.scrollOffset.x(), 100):
                text = '{}s'.format(self.pixelsToDuration(i))
                textRect = painter.fontMetrics().boundingRect(text)

                textRect.translate(-self.scrollOffset.x(), 0)
                textRect.translate(int(i - textRect.width() / 2), int(self.timestempSectionHeight - 13))
                if textRect.right() > a0.rect().width() - 5:
                    textRect.translate(a0.rect().width() - textRect.right() - 5, 0)
                elif textRect.left():
                    textRect.translate(textRect.left() + 5, 0)

                painter.drawLine(i - self.scrollOffset.x(),
                                 self.timestempSectionHeight - 10,
                                 i - self.scrollOffset.x(),
                                 self.timestempSectionHeight)
                painter.drawText(QRectF(textRect), text)

            option = QStyleOptionViewItem()
            for i in range(self.model().rowCount()):
                horizontalSeparatorLineY = i * self.layerHeight + self.timestempSectionHeight - self.scrollOffset.y() + self.layerHeight

                if horizontalSeparatorLineY < self.timestempSectionHeight:
                    continue
                if horizontalSeparatorLineY > self.viewport().height() + self.layerHeight:
                    break

                item = self.model().index(i, 0)
                bgPenColor = QColor(item.data(Qt.DecorationRole)).darker(200)
                bgFillColor = QColor(item.data(Qt.DecorationRole)).darker(150)
                #replace colors
                # bgPenColor = QColor(QColorConstants.Red)
                # bgFillColor = QColor(QColorConstants.lue)
                rectWithoutTimeStamps = a0.rect()
                rectWithoutTimeStamps.setTop(self.timestempSectionHeight)
                painter.setPen(bgPenColor)
                painter.fillRect(QRect(0, i * self.layerHeight + self.timestempSectionHeight - self.scrollOffset.y(),
                                       rectWithoutTimeStamps.width(), self.layerHeight) & rectWithoutTimeStamps,
                                 bgFillColor)
                painter.drawLine(0, horizontalSeparatorLineY, rectWithoutTimeStamps.width(), horizontalSeparatorLineY)
                for j in range(1,self.model().columnCount()):
                    segment = self.model().index(i, j)
                    if not segment.isValid() or (segment.data(Qt.UserRole + 1) is None) or (segment.data(Qt.UserRole + 2) is None):
                        continue
                    option.rect = self.visualRect(segment)
                    # ???
                    option.state.State_MouseOver = (segment == self.hoverIndex)

                    if option.rect.intersects(rectWithoutTimeStamps):
                        option.rect = option.rect & rectWithoutTimeStamps
                        self.itemDelegate().paint(painter, option, segment)
            for i in range(int((a0.rect().left() - self.scrollOffset.x()) / 100 + 1) * 100,
                           a0.rect().right() + self.scrollOffset.x(), 100):
                    painter.setPen(QColor(0, 0, 0, 50))
                    painter.drawLine(i - self.scrollOffset.x(),
                                     self.timestempSectionHeight if (
                                             self.timestempSectionHeight > a0.rect().top()) else a0.rect().top(),
                                     i - self.scrollOffset.x(),
                                     a0.rect().bottom())

    def resizeEvent(self, e):
        self.updateScrollBars()
        super().resizeEvent(e)

    def showEvent(self, a0):
        self.updateScrollBars()
        super().showEvent(a0)

    def indexAt(self, p):
        row = max(0, (p.y() - self.timestempSectionHeight) / self.layerHeight)
        row = min(row, self.model().rowCount()-1)
        for i in range(1,self.model().columnCount()):
            if self.visualRect(self.model().index(int(row), i)).contains(p):
                return self.model().index(int(row), i)
        return QModelIndex()

    def scrollTo(self, index, hint=None):
        return

    def visualRect(self, index):
        if (index.row() >= 0 ) and (index.row() < index.model().rowCount() ) :
            return self.itemRect(index).translated(- self.scrollOffset)
        else:
            return QRect()

    def horizontalOffset(self):
        return 0

    def isIndexHidden(self, index):
        return False

    def moveCursor(self, cursorAction, modifiers):
        return QModelIndex()

    def setSelection(self, rect, command):
        return

    def verticalOffset(self):
        return 0

    def visualRegionForSelection(self, selection):
        return QRegion()

    def viewportEvent(self, e: QEvent):

        if e.type() in [QEvent.MouseButtonPress]:
            self.update(QModelIndex(self.hoverIndex))
            self.hoverIndex = self.indexAt(e.pos())
            mult = 0
            offset = 100
            alpha = 1.
            if e.button() == Qt.LeftButton:
                mult = 1
            elif e.button() == Qt.RightButton:
                mult = -1
            elif e.button() == Qt.MiddleButton:
                alpha = .5
            if self.hoverIndex.isValid():
                val = self.hoverIndex.data(Qt.UserRole + 1)
                self.hoverIndex.model().setData(self.hoverIndex,val+ mult*offset, Qt.UserRole+1)
                val = self.hoverIndex.data(Qt.UserRole + 2)
                self.hoverIndex.model().setData(self.hoverIndex, alpha*val, Qt.UserRole+2)
                self.update(self.hoverIndex)
        # elif e.type() in [QEvent.HoverMove, QEvent.HoverEnter]:
        #     self.update(QModelIndex(self.hoverIndex))
        #     self.hoverIndex = self.indexAt(e.pos())
        #     if self.hoverIndex.isValid():
        #         self.hoverIndex = self.indexAt(e.pos())
        #         self.update(self.hoverIndex)
        # elif e.type() in [QEvent.HoverLeave]:
        #     self.update(QModelIndex(self.hoverIndex))
        #     self.hoverIndex = QModelIndex()
        #     self.update(self.hoverIndex)
        elif e.type() in [QEvent.ToolTip, QEvent.QueryWhatsThis, QEvent.WhatsThis]:
            he = QHelpEvent(e)
            index = self.indexAt(he.pos())
            option = QStyleOptionViewItem()
            option.rect = self.visualRect(index).translated(-self.scrollOffset.x(), -self.scrollOffset.y())
            # option.state |= QStyle.State_HasFocus if (index == self.currentIndex()) else QStyle.State_None

            delegate = self.itemDelegateForRow(index.row())
            if not delegate:
                return False
            return delegate.helpEvent(he, self, option, index)
        #
        # return QAbstractScrollArea.viewportEvent(e)
        return super().viewportEvent(e)

    # QAbstactScrollArea
    def scrollContentsBy(self, dx, dy):
        self.scrollOffset -= QPoint(dx, dy)
        QAbstractItemView.scrollContentsBy(dx, dy)

    def itemRect(self, index):
        startTime = index.data(Qt.UserRole + 1)
        duration = index.data(Qt.UserRole + 2)
        x = self.durationToPixels(startTime)
        width = self.durationToPixels(duration)
        return QRect(int(x), index.row() * self.layerHeight + self.timestempSectionHeight, int(width), self.layerHeight)

    def updateScrollBars(self):
        if self.model() is None:
            return
        max_ = 0
        for i in range(1,self.model().rowCount()):
            item = self.model().index(i, self.model().columnCount() - 1)
            if not item.isValid():
                continue
            max_ = max(max_,self.itemRect(item).right() - self.viewport().width())

        self.horizontalScrollBar().setRange(0, max_)
        self.verticalScrollBar().setRange(0,
                                          self.model().rowCount() * self.layerHeight + self.timestempSectionHeight - self.viewport().height())

    def durationToPixels(self, val):
        return val * self.scale

    def pixelsToDuration(self, val):
        return val / self.scale
