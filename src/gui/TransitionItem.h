/**
 * @file TransitionItem.h
 * @brief Visual representation of a transition between FSM states.
 * 
 * This class renders a curved or looped transition in the FSM editor, including
 * optional arrows and labels. It can be highlighted during execution and updated
 * dynamically as connections are drawn.
 * 
 * Authors: xmarina00, xnovakf00, xlesigm00
 */

#pragma once

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QPen>
#include "IActivable.h"

/**
 * @class TransitionItem
 * @brief Represents a graphical transition between two FSM states.
 * 
 * Draws a curved line or self-loop, displays a label, and supports runtime highlighting.
 * Used inside the FSMCraft scene to connect StateItems visually.
 */
class TransitionItem : public QGraphicsPathItem, public IActivable {
public:
    /**
     * @brief Constructs a transition between two points.
     * @param start Starting point of the transition.
     * @param end Ending point of the transition.
     * @param parent Optional parent graphics item.
     * @param id Unique ID of the transition (default is 9999).
     */
    TransitionItem(QPointF start, QPointF end, QGraphicsItem* parent = nullptr, int id = 9999);

    /**
     * @brief Paints the transition, including the line and optional arrow.
     * @param painter QPainter used to draw the transition.
     * @param option Style options for the item.
     * @param widget Unused. The widget being painted on.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    /**
     * @brief Updates the visual curve between the two endpoints.
     * @param start New starting point.
     * @param end New ending point.
     */
    void updateLine(QPointF start, QPointF end);

    /**
     * @brief Sets the label text shown on the transition.
     * @param text Text to display.
     */
    void setLabel(const QString& text);

    /**
     * @brief Marks the transition as confirmed and final (e.g., after creation).
     */
    void markConfirmed();

    /**
     * @brief Sets whether the transition is currently highlighted (active).
     * @param active True to highlight the transition; false to disable.
     */
    void setActive(bool active) override;

    /**
     * @brief Checks whether the transition is currently active (highlighted).
     * @return True if active; false otherwise.
     */
    bool isActive() const override;

    /**
     * @brief Returns the current label text on the transition.
     * @return Label as QString.
     */
    QString labelText() const;

    /**
     * @brief Returns the unique transition ID.
     * @return Transition ID as integer.
     */
    int getId() const;

private:
    QGraphicsTextItem* label = nullptr; ///< Text label for input/condition
    bool active = false;                ///< Whether the transition is visually active
    bool confirmed = false;             ///< Whether the transition has been finalized
    QPointF start;                      ///< Starting point of the curve
    QPointF end;                        ///< Ending point of the curve
    int id;                             ///< Identifier for backend matching

    /**
     * @brief Repositions the label according to the current curve.
     */
    void updateLabelPosition();
};
