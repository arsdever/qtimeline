#pragma once

#include <QTreeView>
#include <QListView>

class QTimeLine : public QListView
{
public:
    explicit QTimeLine(QWidget* parent = nullptr);
    ~QTimeLine() override;
};
