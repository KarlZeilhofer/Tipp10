
#include "charball.h"

#include <math.h>

//static const double Pi = 3.14159265358979323846264338327950288419717;
//static double TwoPi = 2.0 * Pi;

CharBall::CharBall(int size, int position, int speed, QChar character)
    : color(qrand() % 128 + 128, qrand() % 128 + 128, qrand() % 128 + 128) {

	currentSpeed = speed;
	currentRadius = size/2;
	currentY = - 2 - size;
	currentX = position;
	currentCharacter = character;
	setPos(currentX, currentY);

	destroyed = 0;
    //startTimer(1000 / 33);

}

QRectF CharBall::boundingRect() const {

    qreal adjust = 0.0;
    return QRectF(-currentRadius - adjust, -currentRadius - adjust,
                  (2 * currentRadius) + adjust, (2 * currentRadius) + adjust);

}

QPainterPath CharBall::shape() const {

    QPainterPath path;
    path.addRect(-currentRadius, -currentRadius, 2 * currentRadius, 2 *currentRadius);
    return path;

}

void CharBall::paint(QPainter *painter,
	const QStyleOptionGraphicsItem *, QWidget *) {

    // Body
    painter->setBrush(color);
    painter->drawEllipse(-currentRadius, -currentRadius, 2 * currentRadius, 2* currentRadius);
    //painter->setBrush(QColor(0, 0, 0));

	if (destroyed != 0) {
		destroying();
	}

}

QChar CharBall::character() {

    return currentCharacter;

}

void CharBall::destroy() {
	destroyed = 56;
	//delete this;
	//return;
	update();

}

void CharBall::destroying() {

	color = QColor(0, 0, 0);
	destroyed--;
	//currentRadius = currentRadius - (28 + (destroyed / 4));
	if (destroyed < 2) {
		destroyed = 0;
		this->setVisible(false); //delete this;
		return;
	} else {
		update();
	}

}
