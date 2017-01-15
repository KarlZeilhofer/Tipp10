#ifndef CHARBALL_H
#define CHARBALL_H

#include <QGraphicsItem>
#include <QObject>
#include <QPainterPath>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QTimerEvent>
#include <QColor>
#include <QRectF>

class CharBall : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    CharBall(int size, int position, int speed, QChar character);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
	void destroy();
	QChar character();
	double wind;
	double rad;


private:
	void destroying();

    int currentSpeed;
	int currentX;
	int currentY;
	int currentRadius;
	int destroyed;
    QColor color;
	QChar currentCharacter;
};

#endif // CHARBALL_H
