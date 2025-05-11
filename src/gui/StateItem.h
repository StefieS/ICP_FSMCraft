/**
 * @file StateItem.h
 * @brief Represents a state in the FSM editor as a circular graphics object.
 * @author xmarina00
 * @date 11.05.2025
 *
 * This class is responsible for rendering individual states (nodes) in the FSM graph.
 * Each state has a name, can be marked as initial or final, and can respond to hover events which are handled in MainWindow.cpp.
 * 
 */

#pragma once

#include <QGraphicsObject>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include "IActivable.h"

/**
 * @class StateItem
 * @brief Graphical representation of a single FSM state.
 *
 * This class manages the drawing and interaction logic of an FSM state circle,
 * including its name label, highlighting, and initial/final status.
 */
class StateItem : public QGraphicsObject, public IActivable {
    Q_OBJECT

public:
    /**
     * @brief Constructs a new StateItem.
     * @param position The initial position of the state in the scene.
     * @param name The displayed name of the state.
     */
    StateItem(const QPointF& position, const QString& name);

    /**
     * @brief Marks the state as initial or not.
     * @param initial True if the state should be initial.
     */
    void setInitial(bool initial);  

    /**
     * @brief Checks if the state is marked as initial.
     * @return True if this is the initial state.
     */
    bool isInitial() const;

    /**
     * @brief Marks the state as final or not.
     * @param final True if the state should be final.
     */
    void setFinal(bool final);

    /**
     * @brief Checks if the state is marked as final.
     * @return True if this is a final state.
     */
    bool isFinal() const;

    /**
     * @brief Checks whether a given scene point is inside this state circle.
     * @param pt The scene coordinate to test.
     * @return True if the point lies within this state.
     */
    bool containsScenePoint(const QPointF& pt) const;

    /**
     * @brief Updates the pen color and thickness based on current state status.
     *
     * This is called when the state is marked as active, initial, final, etc.
     */
    void updatePen();

    /**
     * @brief Returns the name of the state.
     * @return Name of the state as a QString.
     */
    QString getName() const;

    /**
     * @brief Computes the center point of the state in scene coordinates.
     * @return The center QPointF of the circle.
     */
    QPointF sceneCenter() const;

    /**
     * @brief Sets the active status of the state (e.g. for highlighting).
     * @param active Whether the state should be active.
     */
    void setActive(bool active) override;

    /**
     * @brief Checks whether the state is currently active (highlighted).
     * @return True if active.
     */
    bool isActive() const override;

protected:
    /**
     * @brief Handles when the mouse hovers over this state.
     * @param event The hover enter event.
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

    /**
     * @brief Handles when the mouse stops hovering over this state.
     * @param event The hover leave event.
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    /**
     * @brief Returns the bounding rectangle of this state for Qt's layout engine.
     * @return QRectF covering the shape.
     */
    QRectF boundingRect() const override;

    /**
     * @brief Custom painting logic for the state. Unused since we rely on QGraphicsEllipseItem.
     */
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;

private:
    QGraphicsEllipseItem* circle;  ///< The circular shape representing the state.
    QGraphicsTextItem* label;      ///< The text label displaying the state name.
    bool initial = false;          ///< True if this is the initial state.
    bool final = false;            ///< True if this is a final (accepting) state.
    bool active = false;           ///< True if this state is currently highlighted.
};
