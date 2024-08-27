#include "InteractiveOrganism.h"
#include <QBrush>
#include <QMessageBox>

InteractiveOrganism::InteractiveOrganism(const QPixmap &pixmap,
                                         Organism* organism,
                                         int width, int height,
                                         QGraphicsItem* parent)
        : QGraphicsPixmapItem(parent), organism(organism) {
    setPixmap(pixmap.scaled(width, height, Qt::KeepAspectRatio));  // Skalowanie obrazu
    setPos(organism->getX(), organism->getY()); // Ustawienie pozycji
}

void InteractiveOrganism::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    // Wyświetlenie szczegółów organizmu po kliknięciu
    QString details = QString("Energy: %1\nAge: %2").arg(organism->getEnergy()).arg(organism->getAge());
    QMessageBox::information(nullptr, "Organism Details", details);
}
