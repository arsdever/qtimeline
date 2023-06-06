#include <QApplication>
#include <QPainter>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QStandardItemModel>
#include <QString>
#include <QStyleFactory>
#include <QTimer>

#include "prof/data.hpp"
#include "prof/profiler.hpp"
#include "prof/profiler_scope_keeper.hpp"
#include "qtimeline_view.hpp"

std::chrono::high_resolution_clock::time_point last_frame_time;

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

class unit
{
public:
    unit() { }

    void slow_motion()
    {
        auto       lock = prof::profile(__func__);
        static int a[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        while (std::ranges::next_permutation(a).found) { }
    }

    void long_running_task(int n = 10)
    {
        auto lock = prof::profile(__func__);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (n)
            long_running_task(n - 1);
    }

    long long_running_task_2branch(int n = 10)
    {
        auto lock = prof::profile(__func__);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (!n || n == 1)
            {
                return 1;
            }

        return long_running_task_2branch(n - 1) + long_running_task_2branch(n - 2);
    }
};

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

    last_frame_time = std::chrono::high_resolution_clock::now();
    unit u {};
    u.slow_motion();

    std::thread t1 { [ &u ]() { u.long_running_task_2branch(); } };
    std::thread t2 { [ &u ]() { u.long_running_task_2branch(); } };
    std::thread t3 { [ &u ]() { u.long_running_task_2branch(); } };
    t1.join();
    t2.join();
    t3.join();

    setupDarkThemePalette();

    QTabWidget window;
    window.show();
    QPushButton button("Update");
    button.show();

    // HeavyDrawnWidget widget; widget.show();

    std::unordered_map<std::string, QTimeLineView*> timelines;
    QColor thread_color = QColor::fromRgb(QRandomGenerator::global()->generate());

    // QTimer timer;
    // timer.setInterval(1000);
    // QObject::connect(&timer, &QTimer::timeout, [ & ]() {
    button.connect(&button, &QPushButton::clicked, [ & ]() {
        auto threads = prof::known_threads();

        for (const auto& thd : threads)
            {
                auto                it       = timelines.find(thd);
                QTimeLineView*      timeline = nullptr;
                QStandardItemModel* model    = nullptr;

                if (it == timelines.end())
                    {
                        timeline = new QTimeLineView();
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
                int                         n = 1;

                prof::apply_for_data(
                    thd, [ &thread_color, &depths, &n, &model ](const prof::data_sample& data) -> bool {
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

                        QStandardItem* section = new QStandardItem(data.name().c_str());
                        layer->setData(thread_color, Qt::DecorationRole);
                        layer->setData(QString("Depth %1").arg(data.depth()), Qt::ToolTipRole);
                        auto  diff      = data.start() - last_frame_time;
                        float diffFloat = std::chrono::duration_cast<std::chrono::duration<float>>(diff).count();
                        section->setData(data.name().c_str(), Qt::ToolTipRole);
                        section->setData(diffFloat, Qt::UserRole + 1);
                        diffFloat =
                            std::chrono::duration_cast<std::chrono::duration<float>>(data.end() - data.start()).count();
                        section->setData(diffFloat, Qt::UserRole + 2);
                        model->setItem(layer->row(), n++, section);
                        return true;
                    });
            }
    });
    // timer.start();

    return app.exec();
}
