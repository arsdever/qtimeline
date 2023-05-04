#include <QStandardItemModel>
#include <qapplication.h>
#include <qscrollarea.h>
#include <qstylefactory.h>
#include <qtimeline.hpp>

void setupDarkThemePalette()
{
    qApp->setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    qApp->setPalette(darkPalette);

    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    setupDarkThemePalette();

    QTimeLine timeline;
    timeline.show();

    QStandardItemModel model;
    QStandardItem*     layer1   = new QStandardItem("Layer 1");
    QStandardItem*     section1 = new QStandardItem("Section 1");
    QStandardItem*     section2 = new QStandardItem("Section 2");
    layer1->setData(QColor(255, 0, 0), Qt::DecorationRole);
    section1->setData(1.0f, Qt::UserRole + 1);
    section1->setData(2.0f, Qt::UserRole + 2);
    section2->setData(4.0f, Qt::UserRole + 1);
    section2->setData(2.0f, Qt::UserRole + 2);
    layer1->appendColumn({ section1 });
    layer1->appendColumn({ section2 });
    QStandardItem* layer2   = new QStandardItem("Layer 2");
    QStandardItem* section3 = new QStandardItem("Section 3");
    layer2->setData(QColor(195, 140, 32), Qt::DecorationRole);
    section3->setData(1.5f, Qt::UserRole + 1);
    section3->setData(2.0f, Qt::UserRole + 2);
    layer2->appendColumn({ section3 });

    model.appendRow(layer1);
    model.appendRow(layer2);

    timeline.setModel(&model);

    return app.exec();
}
