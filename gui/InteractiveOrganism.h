#ifndef ECOSYSTEM_INTERACTIVEORGANISM_H
#define ECOSYSTEM_INTERACTIVEORGANISM_H


#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include "Organism.h"

class InteractiveOrganism : public QGraphicsPixmapItem {
public:
    InteractiveOrganism(const QPixmap &pixmap,
                        Organism* organism,
                        int width, int height,
                        QGraphicsItem* parent = nullptr);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    Organism* organism;
};

#endif //ECOSYSTEM_INTERACTIVEORGANISM_H
