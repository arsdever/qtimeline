#include <frame.hpp>
#include <profiler.hpp>
#include <qapplication.h>
#include <qscrollarea.h>
#include <qstylefactory.h>
#include <scoped_profiler.hpp>
#include <timeline.hpp>

#include "data_provider_sample.hpp"
#include "item.hpp"

class unit
{
public:
    unit() { }

    void slow_motion()
    {
        auto       p = prof::profiler::profile(__func__);
        static int a[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        while (std::ranges::next_permutation(a).found) { }
    }

    void long_running_task(int n = 10)
    {
        auto p = prof::profiler::profile(__func__);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (n)
            long_running_task(n - 1);
    }

    long long_running_task_2branch(int n = 10)
    {
        auto p = prof::profiler::profile(__func__);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (!n || n == 1)
            {
                return 1;
            }

        return long_running_task_2branch(n - 1) + long_running_task_2branch(n - 2);
    }
};

std::vector<std::vector<tl::const_timeline_item_ptr>> populate_data()
{
    std::vector<std::vector<std::vector<tl::const_timeline_item_ptr>>> result {};
    unit                                                               u {};
    u.slow_motion();
    std::thread t1 { [ &u ]() { u.long_running_task_2branch(); } };
    std::thread t2 { [ &u ]() { u.long_running_task_2branch(); } };
    std::thread t3 { [ &u ]() { u.long_running_task_2branch(); } };
    t1.join();
    t2.join();
    t3.join();
    prof::profiler::for_each([ &result ](prof::profiler const& p) {
        result.emplace_back(std::vector<std::vector<tl::const_timeline_item_ptr>> {});
        auto& back = result.back();
        p.for_each_data([ st = p.start_time(), &back ](prof::profiler::data_t const& d) {
            if (d.depth() >= back.size())
                {
                    back.resize(d.depth() + 1);
                }
            back[ d.depth() ].emplace_back(new tl::item { d.name(), d.start() - st, d.end() - st });
        });
    });
    return result[ 1 ].size() == 1 ? result[0] : result[1];
}

// std::vector<tl::const_timeline_item_ptr> populate_data()
// {
//     std::chrono::steady_clock::time_point    tp;
//     std::vector<tl::const_timeline_item_ptr> result;
//     result.push_back(std::make_shared<tl::item>(__func__, tp, tp + std::chrono::seconds { 8 }));
//     tp += std::chrono::seconds { 8 };
//     result.push_back(
//         std::make_shared<tl::item>(__func__, tp + std::chrono::seconds { 8 }, tp + std::chrono::seconds { 15 }));
//     tp += std::chrono::seconds { 23 };
//     result.push_back(
//         std::make_shared<tl::item>(__func__, tp + std::chrono::seconds { 20 }, tp + std::chrono::seconds { 30 }));
//     tp += std::chrono::seconds { 50 };
//     result.push_back(
//         std::make_shared<tl::item>(__func__, tp + std::chrono::seconds { 5 }, tp + std::chrono::seconds { 30 }));
//     tp += std::chrono::seconds { 35 };
//     return result;
// }

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
    wdg->set_scale(500);
    scroll.setWidget(wdg);
    scroll.setWidgetResizable(true);
    scroll.show();
    return app.exec();
}