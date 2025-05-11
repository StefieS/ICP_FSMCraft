/**
 * @file QFlowLayout.h
 * @brief Custom layout class that arranges widgets in a flow layout, wrapping them as needed.
 *
 * Similar to how text flows in a paragraph, QFlowLayout automatically arranges
 * child widgets left-to-right and wraps them onto the next line when there's not enough space.
 * 
 *  * Authors: xmarina00
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
 * allowing dynamic resizing and consistent spacing.
 */
class QFlowLayout : public QLayout {
public:
    /**
     * @brief Constructs a flow layout.
     * @param parent Optional parent widget.
     * @param margin Outer margin in pixels. If -1, default style margin is used.
     * @param hSpacing Horizontal space between items. If -1, style defaults are used.
     * @param vSpacing Vertical space between rows. If -1, style defaults are used.
     */
    explicit QFlowLayout(QWidget *parent = nullptr, int margin = -1, int hSpacing = -1, int vSpacing = -1);

    /**
     * @brief Destructor. Cleans up all layout items.
     */
    ~QFlowLayout();

    /**
     * @brief Adds a new layout item to the flow.
     * @param item The layout item to add.
     */
    void addItem(QLayoutItem *item) override;

    /**
     * @brief Returns horizontal spacing between items.
     * @return Number of pixels between items horizontally.
     */
    int horizontalSpacing() const;

    /**
     * @brief Returns vertical spacing between rows.
     * @return Number of pixels between rows.
     */
    int verticalSpacing() const;

    /**
     * @brief Indicates that this layout supports height-for-width sizing.
     * @return Always true.
     */
    bool hasHeightForWidth() const override;

    /**
     * @brief Calculates the height required for the given width.
     * @param width The total available width.
     * @return The height needed to fit the layout.
     */
    int heightForWidth(int width) const override;

    /**
     * @brief Returns the number of layout items.
     * @return Number of items in the layout.
     */
    int count() const override;

    /**
     * @brief Returns the item at the given index.
     * @param index Index of the item.
     * @return Pointer to the QLayoutItem at the index or nullptr if out of bounds.
     */
    QLayoutItem *itemAt(int index) const override;

    /**
     * @brief Removes and returns the item at the given index.
     * @param index Index of the item to remove.
     * @return The removed layout item, or nullptr if index is invalid.
     */
    QLayoutItem *takeAt(int index) override;

    /**
     * @brief Returns the minimum size needed for the layout.
     * @return Minimum QSize that can fit all items.
     */
    QSize minimumSize() const override;

    /**
     * @brief Sets the geometry of the layout and arranges all items.
     * @param rect The bounding rectangle to layout items in.
     */
    void setGeometry(const QRect &rect) override;

    /**
     * @brief Returns the recommended size for the layout.
     * @return Suggested size based on current contents.
     */
    QSize sizeHint() const override;

private:
    /**
     * @brief Performs layouting of items within the given rectangle.
     * @param rect The bounding area to arrange within.
     * @param testOnly If true, just calculates size without modifying item geometry.
     * @return Total height used by the layout.
     */
    int doLayout(const QRect &rect, bool testOnly) const;

    /**
     * @brief Determines spacing based on the style and layout type.
     * @param pm Pixel metric type to use.
     * @return Spacing in pixels.
     */
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> itemList;  ///< List of items managed by this layout.
    int m_hSpace;                   ///< Horizontal spacing between widgets.
    int m_vSpace;                   ///< Vertical spacing between rows.
};
