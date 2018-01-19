#include "ControlItem.h"

#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <iostream>

#include "editor/model/control/NodeControl.h"
#include "../ItemResizer.h"
#include "../schematic/SchematicCanvas.h"

using namespace AxiomGui;
using namespace AxiomModel;

ControlItem::ControlItem(NodeControl *control, SchematicCanvas *canvas) : control(control), canvas(canvas) {
    connect(control, &NodeControl::posChanged,
            this, &ControlItem::setPos);
    connect(control, &NodeControl::beforeSizeChanged,
            this, &ControlItem::triggerGeometryChange);
    connect(control, &NodeControl::sizeChanged,
            this, &ControlItem::setSize);
    connect(control, &NodeControl::removed,
            this, &ControlItem::remove);

    // create resize items
    ItemResizer::Direction directions[] = {
            ItemResizer::TOP, ItemResizer::RIGHT, ItemResizer::BOTTOM, ItemResizer::LEFT,
            ItemResizer::TOP_RIGHT, ItemResizer::TOP_LEFT, ItemResizer::BOTTOM_RIGHT, ItemResizer::BOTTOM_LEFT
    };
    for (auto i = 0; i < 8; i++) {
        auto resizer = new ItemResizer(directions[i], SchematicCanvas::controlGridSize);
        resizer->enablePainting();
        resizer->setVisible(false);

        // ensure corners are on top of edges
        resizer->setZValue(i > 3 ? 3 : 2);

        connect(this, &ControlItem::resizerPosChanged,
                resizer, &ItemResizer::setPos);
        connect(this, &ControlItem::resizerSizeChanged,
                resizer, &ItemResizer::setSize);

        connect(resizer, &ItemResizer::startDrag,
                this, &ControlItem::resizerStartDrag);
        connect(resizer, &ItemResizer::changed,
                this, &ControlItem::resizerChanged);

        connect(control, &NodeControl::selected,
                resizer, [this, resizer]() { resizer->setVisible(true); });
        connect(control, &NodeControl::deselected,
                resizer, [this, resizer]() { resizer->setVisible(false); });

        resizer->setParentItem(this);
    }

    // set initial state
    setPos(control->pos());
    setSize(control->size());
}

QRectF ControlItem::boundingRect() const {
    return {
            QPoint(0, 0),
            SchematicCanvas::controlRealSize(control->size())
    };
}

bool ControlItem::isEditable() const {
    return !control->isSelected();
}

AxiomModel::ConnectionSink& ControlItem::sink() {
    return control->sink;
}

void ControlItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        event->accept();

        isConnecting = true;
        canvas->startConnecting(this);
    } else if (!isEditable() && event->button() == Qt::LeftButton) {
        event->accept();

        isMoving = true;
        mouseStartPoint = event->screenPos();
        emit control->startedDragging();
    } else {
        event->ignore();
    }
}

void ControlItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (isConnecting) {
        event->accept();

        canvas->updateConnecting(event->scenePos());
    } else if (isMoving) {
        event->accept();

        auto mouseDelta = event->screenPos() - mouseStartPoint;
        emit control->draggedTo(QPoint(
                qRound((float) mouseDelta.x() / SchematicCanvas::controlGridSize.width()),
                qRound((float) mouseDelta.y() / SchematicCanvas::controlGridSize.height())
        ));
    } else {
        event->ignore();
    }
}

void ControlItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (isConnecting) {
        event->accept();

        isConnecting = false;
        canvas->endConnecting(event->scenePos());
    } else if (isMoving) {
        event->accept();

        isMoving = false;
        emit control->finishedDragging();
    } else {
        event->ignore();
    }
}

void ControlItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    control->select(true);
    mousePressEvent(event);
}

void ControlItem::setPos(QPoint newPos) {
    auto realPos = SchematicCanvas::controlRealPos(newPos);
    QGraphicsItem::setPos(realPos.x(), realPos.y());
    emit resizerPosChanged(realPos);
}

void ControlItem::setSize(QSize newSize) {
    emit resizerSizeChanged(SchematicCanvas::controlRealSize(newSize));
}

void ControlItem::remove() {
    scene()->removeItem(this);
}

void ControlItem::resizerChanged(QPointF topLeft, QPointF bottomRight) {
    control->setCorners(QPoint(
            qRound(topLeft.x() / SchematicCanvas::controlGridSize.width()),
            qRound(topLeft.y() / SchematicCanvas::controlGridSize.height())
    ), QPoint(
            qRound(bottomRight.x() / SchematicCanvas::controlGridSize.width()),
            qRound(bottomRight.y() / SchematicCanvas::controlGridSize.height())
    ));
}

void ControlItem::resizerStartDrag() {
    control->select(true);
}

void ControlItem::triggerGeometryChange() {
    prepareGeometryChange();
}

void ControlItem::triggerUpdate() {
    update();
}