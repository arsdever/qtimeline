#include <QPainter>
#include <QRandomGenerator>
#include <QStandardItemModel>
#include <QString>
#include <QTimer>
#include <qapplication.h>
#include <qscrollarea.h>
#include <qstylefactory.h>
#include <qtimeline.hpp>

#include "prof/data.hpp"
#include "prof/profiler.hpp"
#include "prof/profiler_scope_keeper.hpp"

std::chrono::high_resolution_clock::time_point last_frame_time;

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

class HeavyDrawnWidget : public QWidget
{
public:
    HeavyDrawnWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
    }

protected:
    void paintEvent(QPaintEvent* event) override
    {
        last_frame_time = std::chrono::high_resolution_clock::now();
        auto     lock   = prof::profile_frame(__func__);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        drawRects(painter);
        drawCircles(painter);
        drawLines(painter);
        drawPoints(painter);
        drawPie(painter);
    }

    void drawCircles(QPainter& painter)
    {
        auto lock = prof::profile(__func__);
        for (int i = 0; i < 100; ++i)
            {
                painter.setBrush(QColor::fromRgb(QRandomGenerator::global()->generate()));
                painter.drawEllipse(QPoint(QRandomGenerator::global()->generate() % width(),
                                           QRandomGenerator::global()->generate() % height()),
                                    QRandomGenerator::global()->generate() % 100,
                                    QRandomGenerator::global()->generate() % 100);
            }
    }

    void drawRects(QPainter& painter)
    {
        auto lock = prof::profile(__func__);
        for (int i = 0; i < 100; ++i)
            {
                painter.setBrush(QColor::fromRgb(QRandomGenerator::global()->generate()));
                painter.drawRect(QRect(
                    QPoint(QRandomGenerator::global()->generate() % width(),
                           QRandomGenerator::global()->generate() % height()),
                    QSize(QRandomGenerator::global()->generate() % 100, QRandomGenerator::global()->generate() % 100)));
            }
    }

    void drawLines(QPainter& painter)
    {
        auto lock = prof::profile(__func__);
        for (int i = 0; i < 100; ++i)
            {
                painter.setBrush(QColor::fromRgb(QRandomGenerator::global()->generate()));
                painter.drawLine(QPoint(QRandomGenerator::global()->generate() % width(),
                                        QRandomGenerator::global()->generate() % height()),
                                 QPoint(QRandomGenerator::global()->generate() % width(),
                                        QRandomGenerator::global()->generate() % height()));
            }
    }

    void drawPoints(QPainter& painter)
    {
        auto lock = prof::profile(__func__);
        for (int i = 0; i < 100; ++i)
            {
                painter.setBrush(QColor::fromRgb(QRandomGenerator::global()->generate()));
                painter.drawPoint(QPoint(QRandomGenerator::global()->generate() % width(),
                                         QRandomGenerator::global()->generate() % height()));
            }
    }

    void drawPie(QPainter& painter)
    {
        auto lock = prof::profile(__func__);
        for (int i = 0; i < 100; ++i)
            {
                painter.setBrush(QColor::fromRgb(QRandomGenerator::global()->generate()));
                painter.drawPie(QRect(QPoint(QRandomGenerator::global()->generate() % width(),
                                             QRandomGenerator::global()->generate() % height()),
                                      QSize(QRandomGenerator::global()->generate() % 100,
                                            QRandomGenerator::global()->generate() % 100)),
                                QRandomGenerator::global()->generate() % 100,
                                QRandomGenerator::global()->generate() % 100);
            }
    }
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    // unit u;
    // u.multithreaded_load();

    setupDarkThemePalette();

    QTabWidget window;
    window.show();

    HeavyDrawnWidget widget;
    widget.show();

    std::unordered_map<std::string, QTimeLine*> timelines;

    QTimer timer;
    timer.setInterval(10);
    QColor thread_color = QColor::fromRgb(QRandomGenerator::global()->generate());
    QObject::connect(&timer, &QTimer::timeout, [ & ]() {
        auto threads = prof::known_threads();

        for (const auto& thd : threads)
            {
                auto                it       = timelines.find(thd);
                QTimeLine*          timeline = nullptr;
                QStandardItemModel* model    = nullptr;

                if (it == timelines.end())
                    {
                        timeline = new QTimeLine();
                        model    = new QStandardItemModel(timeline);

                        timeline->show();
                        timeline->setModel(model);
                        timelines.emplace(thd, timeline);
                        window.addTab(timeline, QString("Thread #%1").arg(thd.c_str()));
                    }
                else
                    {
                        timeline = it->second;
                        model    = static_cast<QStandardItemModel*>(timeline->model());
                    }

                model->clear();

                std::vector<QStandardItem*> depths;

                prof::apply_for_data(thd, [ &thread_color, &depths, &model ](const prof::data_sample& data) -> bool {
                    QStandardItem* layer = nullptr;
                    if (data.depth() >= depths.size() || depths[ data.depth() ] == nullptr)
                        {
                            depths.resize(std::max(data.depth() + 1, depths.size()), nullptr);
                            layer                  = new QStandardItem(QString("Depth %1").arg(data.depth()));
                            depths[ data.depth() ] = layer;
                            model->appendRow(layer);
                        }
                    else
                        {
                            layer = depths[ data.depth() ];
                        }

                    QStandardItem* section = new QStandardItem("Section");
                    layer->setData(thread_color, Qt::DecorationRole);
                    layer->setData(QString("Depth %1").arg(data.depth()), Qt::ToolTipRole);
                    auto  diff      = data.start() - last_frame_time;
                    float diffFloat = std::chrono::duration_cast<std::chrono::duration<float>>(diff).count();
                    section->setData(data.name().c_str(), Qt::ToolTipRole);
                    section->setData(diffFloat, Qt::UserRole + 1);
                    diffFloat =
                        std::chrono::duration_cast<std::chrono::duration<float>>(data.end() - data.start()).count();
                    section->setData(diffFloat, Qt::UserRole + 2);
                    layer->appendColumn({ section });
                    return true;
                });
            }
    });
    timer.start();

    return app.exec();
}
