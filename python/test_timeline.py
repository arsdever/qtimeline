import sys

from PyQt5.QtCore import Qt
from PyQt5.QtGui import QStandardItemModel, QStandardItem, QColorConstants, QPalette, QColor
from PyQt5.QtWidgets import QMainWindow, QTabWidget, QApplication

from QTimeLineView import QTimeLineView


def setup_dark_theme(qapp: QApplication):
    darkPalette = QPalette()
    darkPalette.setColor(QPalette.Window, QColor(53, 53, 53))
    darkPalette.setColor(QPalette.WindowText, QColorConstants.White)
    darkPalette.setColor(QPalette.Base, QColor(25, 25, 25))
    darkPalette.setColor(QPalette.AlternateBase, QColor(53, 53, 53))
    darkPalette.setColor(QPalette.ToolTipBase, QColorConstants.White)
    darkPalette.setColor(QPalette.ToolTipText, QColorConstants.White)
    darkPalette.setColor(QPalette.Text, QColorConstants.White)
    darkPalette.setColor(QPalette.Button, QColor(53, 53, 53))
    darkPalette.setColor(QPalette.ButtonText, QColorConstants.White)
    darkPalette.setColor(QPalette.BrightText, QColorConstants.Red)
    darkPalette.setColor(QPalette.Link, QColor(42, 130, 218))
    darkPalette.setColor(QPalette.Highlight, QColor(42, 130, 218))
    darkPalette.setColor(QPalette.HighlightedText, QColorConstants.Black)

    qapp.setPalette(darkPalette)
    qapp.setStyleSheet("QToolTip { color: #fffff0; background-color: #2a82da; "
                       "border: 1px solid white; }")


class MainWindow(QMainWindow):

    def __init__(self):
        super().__init__()

        self.setWindowTitle("GEOS standard UI")
        self.tabWidget = QTabWidget()
        self.setCentralWidget(self.tabWidget)

    def add_timeline(self):
        # by hand
        timeline = QTimeLineView()

        timeline.setModel(QStandardItemModel(timeline))
        timeline.model().clear()
        timeline.setScale(1.0)

        layer = QStandardItem("tada")
        layer.setData(QColorConstants.Black, Qt.DecorationRole)
        layer.setData("layer", Qt.ToolTipRole)
        timeline.model().appendRow(layer)

        section = QStandardItem("SECTION")
        section.setData(QColorConstants.Blue.lighter(100), Qt.DecorationRole)
        section.setData("sec_data", Qt.ToolTipRole)
        section.setData(12.0, Qt.UserRole + 1)
        section.setData(122.0, Qt.UserRole + 2)

        timeline.model().setItem(layer.row(), 1, section)

        ## new layer

        layer2 = QStandardItem("todo")
        layer2.setData(QColorConstants.Black, Qt.DecorationRole)
        layer2.setData("layer-2", Qt.ToolTipRole)
        timeline.model().appendRow(layer2)

        section = QStandardItem("SECTION-2")
        section.setData(QColorConstants.Blue, Qt.DecorationRole)
        section.setData("sec2_data", Qt.ToolTipRole)
        section.setData(145.0, Qt.UserRole + 1)
        section.setData(222.0, Qt.UserRole + 2)

        timeline.model().setItem(layer2.row(), 1, section)

        timeline.show()

        self.tabWidget.addTab(timeline, "toto")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    # setup_dark_theme(app)

    window = MainWindow()
    window.show()
    window.add_timeline()

    app.exec()
