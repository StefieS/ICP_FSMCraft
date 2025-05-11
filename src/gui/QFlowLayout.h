/**
 * @file QFlowLayout.h
 * @brief Custom layout class that arranges widgets in a flow layout, wrapping them as needed.
 * @author xmarina00
 * @date 11.05.2025
 *
 * Similar to how text flows in a paragraph, QFlowLayout automatically arranges
 * child widgets left-to-right and wraps them onto the next line when there's not enough space.
 *
 */

#pragma once

#include <QLayout>
#include <QRect>
#include <QStyle>

/**
 * @class QFlowLayout
 * @brief A custom layout that places widgets horizontally with automatic line wrapping.
 *
 * This layout is used in FSMCraft to arrange internal variable widgets neatly inside a container,
 * allowing dynamic resizing and consistent spacing across rows.
 */
class QFlowLayout : public QLayout {
public:
    /**
     * @brief Constructs a new QFlowLayout.
     * 
     * @param parent Parent widget (optional).
     * @param margin Margin around layout contents (default -1 for style-based).
     * @param hSpacing Horizontal spacing between items (default -1 for style-based).
     * @param vSpacing Vertical spacing between rows (default -1 for style-based).
     */
    explicit QFlowLayout(QWidget *parent = nullptr, int margin = -1, int hSpacing = -1, int vSpacing = -1);

    /**
     * @brief Destroys the layout and cleans up its child items.
     */
    ~QFlowLayout();

    /**
     * @brief Adds a new layout item to the flow.
     * 
     * @param item The layout item to be added.
     */
    void addItem(QLayoutItem *item) override;

    /**
     * @brief Returns the horizontal spacing between items.
     * 
     * @return Pixel value of horizontal spacing.
     */
    int horizontalSpacing() const;

    /**
     * @brief Returns the vertical spacing between rows.
     * 
     * @return Pixel value of vertical spacing.
     */
    int verticalSpacing() const;

    /**
     * @brief Indicates whether the layout supports height-for-width.
     * 
     * @return Always true for flow layout.
     */
    bool hasHeightForWidth() const override;

    /**
     * @brief Computes the height needed for a given width.
     * 
     * @param width The total available width.
     * @return The total height required to arrange items within this width.
     */
    int heightForWidth(int width) const override;

    /**
     * @brief Returns the number of items managed by this layout.
     * 
     * @return Total number of layout items.
     */
    int count() const override;

    /**
     * @brief Returns the item at the given index.
     * 
     * @param index Index of the desired layout item.
     * @return Pointer to the item or nullptr if index is invalid.
     */
    QLayoutItem *itemAt(int index) const override;

    /**
     * @brief Removes and returns the layout item at the given index.
     * 
     * @param index Index of the item to remove.
     * @return Pointer to the removed layout item, or nullptr if index is invalid.
     */
    QLayoutItem *takeAt(int index) override;

    /**
     * @brief Returns the minimum size required for the layout.
     * 
     * @return Minimum size to contain all layout items.
     */
    QSize minimumSize() const override;

    /**
     * @brief Arranges all items in the layout within the given rectangle.
     * 
     * @param rect The bounding rectangle to use for layouting.
     */
    void setGeometry(const QRect &rect) override;

    /**
     * @brief Returns the recommended size for this layout.
     * 
     * @return Suggested size hint based on current items.
     */
    QSize sizeHint() const override;

private:
    /**
     * @brief Internal function to arrange layout items.
     * 
     * @param rect Rectangle within which to arrange.
     * @param testOnly If true, only calculates height without changing item positions.
     * @return Total vertical space consumed.
     */
    int doLayout(const QRect &rect, bool testOnly) const;

    /**
     * @brief Computes spacing value based on style metrics.
     * 
     * @param pm The pixel metric to query.
     * @return Pixel spacing.
     */
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> itemList;  ///< List of items managed by the layout.
    int m_hSpace;                   ///< Horizontal spacing (px).
    int m_vSpace;                   ///< Vertical spacing (px).
};
