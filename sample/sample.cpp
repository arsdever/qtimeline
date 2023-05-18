#include <QRandomGenerator>
#include <QStandardItemModel>
#include <QString>
#include <qapplication.h>
#include <qscrollarea.h>
#include <qstylefactory.h>
#include <qtimeline.hpp>

#include "prof/frame.hpp"
#include "prof/profiler.hpp"
#include "prof/profiler_scope_keeper.hpp"

class unit
{
public:
    unit() { }

    void multithreaded_load()
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i)
            {
                threads.emplace_back([ this ]() {
                    while (counter > 0)
                        {
                            auto                        p = prof::profile(__func__);
                            std::lock_guard<std::mutex> guard(mutex);
                            std::this_thread::sleep_for(std::chrono::milliseconds(100));
                            --counter;
                        }

                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                });
            }

        for (auto& thread : threads)
            {
                thread.join();
            }
    }

    long       counter = 10;
    std::mutex mutex;
};

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

    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; "
                        "border: 1px solid white; }");
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    unit u;
    u.multithreaded_load();

    setupDarkThemePalette();

    QTabWidget window;
    window.show();

    QList<QTimeLine*>          timelines;
    QList<QStandardItemModel*> models;
    const auto                 time = std::chrono::high_resolution_clock::now();

    auto threads = prof::known_threads();

    for (const auto& thd : threads)
        {
            QTimeLine*          timeline = new QTimeLine();
            QStandardItemModel* model    = new QStandardItemModel(timeline);
            timeline->show();
            timeline->setModel(model);
            window.addTab(timeline, QString("Thread #%1").arg(thd.c_str()));
            QColor thread_color = QColor::fromRgb(QRandomGenerator::global()->generate());
            prof::apply_for_data(thd, [ &thread_color, &time, &model ](const prof::frame& data) -> bool {
                QStandardItem* layer   = new QStandardItem("Layer");
                QStandardItem* section = new QStandardItem("Section");
                layer->setData(thread_color, Qt::DecorationRole);
                layer->setData(QString("%1,%2").arg(data.name().c_str()).arg(data.depth()), Qt::ToolTipRole);
                auto  diff      = data.start() - time;
                float diffFloat = std::chrono::duration_cast<std::chrono::duration<float>>(diff).count();
                section->setData(diffFloat + 5, Qt::UserRole + 1);
                diffFloat = std::chrono::duration_cast<std::chrono::duration<float>>(data.end() - data.start()).count();
                section->setData(diffFloat, Qt::UserRole + 2);
                layer->appendColumn({ section });
                model->appendRow(layer);
                return true;
            });
        }

    return app.exec();
}
