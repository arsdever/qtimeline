#include <qapplication.h>
#include <qscrollarea.h>
#include <qstylefactory.h>
#include <timeline.hpp>

#include "data_provider_sample.hpp"
#include "item.hpp"

std::vector<tl::const_timeline_item_ptr> populate_data()
{
    std::chrono::steady_clock::time_point    tp;
    std::vector<tl::const_timeline_item_ptr> result;
    result.push_back(std::make_shared<tl::item>(__func__, tp, tp + std::chrono::seconds { 8 }));
    tp += std::chrono::seconds { 8 };
    result.push_back(
        std::make_shared<tl::item>(__func__, tp + std::chrono::seconds { 8 }, tp + std::chrono::seconds { 15 }));
    tp += std::chrono::seconds { 23 };
    result.push_back(
        std::make_shared<tl::item>(__func__, tp + std::chrono::seconds { 20 }, tp + std::chrono::seconds { 30 }));
    tp += std::chrono::seconds { 50 };
    result.push_back(
        std::make_shared<tl::item>(__func__, tp + std::chrono::seconds { 5 }, tp + std::chrono::seconds { 30 }));
    tp += std::chrono::seconds { 35 };
    return result;
}

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
    QScrollArea  scroll;

    setupDarkThemePalette();

    tl::timeline_data_provider_ptr provider = std::make_shared<tl::data_provider_sample>(populate_data());

    tl::timeline* wdg = new tl::timeline {};
    wdg->set_data_provider(provider);
    wdg->set_scale(100);
    scroll.setWidget(wdg);
    scroll.setWidgetResizable(true);
    scroll.show();
    return app.exec();
}