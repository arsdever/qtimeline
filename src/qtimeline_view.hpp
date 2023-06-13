#pragma once

#include <QListView>
#include <QPersistentModelIndex>
#include <QTableView>
#include <QTreeView>

class QTimeLineView : public QAbstractItemView
{
public:
    explicit QTimeLineView(QWidget* parent = nullptr);
    ~QTimeLineView() override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

#pragma region QAbstractItemView

public:
    QModelIndex indexAt(const QPoint& point) const override;
    void        scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible) override;
    QRect       visualRect(const QModelIndex& index) const override;

protected:
    int         horizontalOffset() const override;
    bool        isIndexHidden(const QModelIndex& index) const override;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    void        setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags flags) override;
    int         verticalOffset() const override;
    QRegion     visualRegionForSelection(const QItemSelection& selection) const override;
    bool        viewportEvent(QEvent* event) override;

#pragma endregion

#pragma region QAbstractScrollArea

public:
    void scrollContentsBy(int dx, int dy) override;

#pragma endregion

private:
    QRect                         itemRect(const QModelIndex& index) const;
    void                          updateScrollBars();
    double                        durationToPixels(std::chrono::duration<double> value) const;
    std::chrono::duration<double> pixelsToDuration(int value) const;

private:
    /**
     * @brief Scaling factor for the timeline.
     *
     * Indicates how many pixels correspond to one micro.
     */
    double                _scale                   = 1.0;
    int                   _timestampsSectionHeight = 30;
    double                _timestampsPer100Pixels  = 1.0;
    int                   _layerHeight             = 20;
    QPersistentModelIndex _hoverIndex;
    QPoint                _scrollOffset;
};
