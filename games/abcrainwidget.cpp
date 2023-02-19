
#include <QIcon>
#include <QTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QTime>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <math.h>

#include "abcrainwidget.h"
#include "def/errordefines.h"
#include "widget/errormessage.h"
#include "def/defines.h"

static const double Pi = 3.14159265358979323846264338327950288419717;
static const int AreaWidth = 580;
static const int AreaHeight = 360;
static const int AreaX = 0;
static const int AreaY = 0;
static const int GroundWidth = 580;
static const int GroundHeight = 10;
static const int GroundX = 0;
static const int GroundY = AreaHeight - GroundHeight;
static const int BallWidth = 30;

#if APP_MAC
static const int FontSizeH1 = 12;
static const int FontSizeH2 = 14;
static const int FontSizeH3 = 26;
static const int FontSizeH4 = 60;
#else
static const int FontSizeH1 = 10;
static const int FontSizeH2 = 11;
static const int FontSizeH3 = 20;
static const int FontSizeH4 = 50;
#endif

AbcRainWidget::AbcRainWidget(QWidget *parent) : QWidget(parent) {

	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	gamePaused = 0;
	gameStarted = 0;
	gameOvered = 0;

	// Set the logo
	companyLogo = new CompanyLogo(this, true);

	stepsLevel        << 1   << 2   << 3   << 4   << 5   << 6   << 7   << 7   << 8   << 9   << 9   << 10  << 10  << 11  << 11  << 12  << 12  << 13  << 13  << 14  << 14  << 15  << 15  << 16  << 16  << 17  << 17  << 18  << 18;
	stepsNumBalls     << 2   << 2   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1   << 1;
	stepsTimerSpeed   << 80  << 70  << 70  << 70  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60  << 60;
	stepsTimerNewBall << 24  << 24  << 20  << 20  << 20  << 16  << 10  << 10  << 5   << 10  << 10  << 10  << 10  << 14  << 10   << 10  << 10  << 10  << 10  << 10  << 10  << 10  << 10  << 10  << 10  << 5   << 5   << 5   << 3;
	stepsOffset       << 1   << 2   << 2   << 3   << 3   << 3   << 3   << 3   << 3   << 4   << 4   << 5   << 5   << 3   << 3   << 4   << 4   << 5   << 5   << 6   << 6   << 7   << 7   << 8   << 8   << 8   << 8   << 10  << 14;

    scene = new QGraphicsScene;
    scene->setSceneRect(AreaX, AreaY, AreaWidth, AreaHeight);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    for (int i = 0; i < 26; ++i) {
		abclist.append(QChar(97 + i));
    }

	ground = scene->addRect(GroundX, GroundY, GroundWidth, GroundHeight, QPen(QColor(200, 0, 0)), QBrush(QColor(200, 0, 0)));

    levelText = new QGraphicsTextItem(tr("Leertaste startet das Spiel"));
	levelText->setFont(QFont("Arial", FontSizeH3, 100));
	levelText->setDefaultTextColor(QColor(0, 0, 0, 250));
	levelText->setPos((AreaWidth / 2) - (levelText->boundingRect().width() / 2),
		(AreaHeight / 2) - (levelText->boundingRect().height() / 2) - 10);
	scene->addItem(levelText);

	levelSubText = new QGraphicsTextItem("");
	levelSubText->setFont(QFont("Arial", FontSizeH1, 100));
	levelSubText->setDefaultTextColor(QColor(0, 0, 0, 250));
	levelSubText->setPos(0, 0);

	scene->addItem(levelSubText);

    view = new QGraphicsView;
	view->setScene(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setBackgroundBrush(QPixmap(":/games/img/pattern_" + QString::number(qrand() % 16) + ".png"));
    view->setCacheMode(QGraphicsView::CacheBackground);
    //view->setDragMode(QGraphicsView::ScrollHandDrag);
    //view->setSceneRect(AreaX, AreaY, AreaWidth + 200, AreaHeight + 200);
    //view->resize(AreaWidth + 200, AreaHeight + 200);

	buttonHelp = new QPushButton(tr("&Hilfe"));
	buttonHelp->setFocusPolicy(Qt::NoFocus);
	buttonPause = new QPushButton(tr("&Pause"));
    buttonPause->setEnabled(false);
	buttonPause->setFocusPolicy(Qt::NoFocus);
	buttonReady = new QPushButton(tr("Spiel &beenden"));
	buttonReady->setFocusPolicy(Qt::NoFocus);

	// Button layout horizontal
	QHBoxLayout *buttonLayoutHorizontal = new QHBoxLayout;
    buttonLayoutHorizontal->addWidget(buttonReady);
    buttonLayoutHorizontal->addSpacing(10);
    buttonLayoutHorizontal->addWidget(buttonHelp);
    buttonLayoutHorizontal->addWidget(buttonPause);
	// Button layout vertical
    QVBoxLayout *buttonLayoutVertical = new QVBoxLayout;
    buttonLayoutVertical->addSpacing(20);
    buttonLayoutVertical->addLayout(buttonLayoutHorizontal);
	// Logo layout vertical
    QVBoxLayout *logoLayout = new QVBoxLayout;
    logoLayout->addSpacing(10);
    logoLayout->addWidget(companyLogo);
	// Logo layout + button layout horizontal
	QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addLayout(logoLayout);
    bottomLayout->addStretch(1);
    bottomLayout->addLayout(buttonLayoutVertical);
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(view);
    mainLayout->addSpacing(1);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));

	// Button connections
	connect(buttonPause, SIGNAL(clicked()), this, SLOT(pauseGame()));
	connect(buttonReady, SIGNAL(clicked()), this, SLOT(clickReady()));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(showHelp()));

	this->setFocus();
	setFocusPolicy(Qt::StrongFocus);

}

AbcRainWidget::~AbcRainWidget() {

}

void AbcRainWidget::startGame() {

    buttonPause->setEnabled(true);

	levelText->setFont(QFont("Arial", FontSizeH4, 100));
	levelSubText->setPlainText("");

	points = new QGraphicsTextItem();
	points->setFont(QFont("Arial", FontSizeH2, 100));
	points->setPos(0, 0);

	scene->addItem(points);

	timerCounter = 0;
	shotPoints = 0;
	ballCounter = 0;
	step = 0;
	gameStarted = 1;
	level = 0;

	timer->start(100);

	nextStep();

    for (int i = 0; i < numBalls; ++i) {
		setBall();
    }
	numBalls = 1;

}

void AbcRainWidget::nextStep() {

	if (gameStarted == 1 && gamePaused == 0) {
		if (level != stepsLevel[step]) {
			setLevel();
		}
		level = stepsLevel[step];
		numBalls = stepsNumBalls[step];
		timerSpeed = stepsTimerSpeed[step];
		timerNewBall = stepsTimerNewBall[step];
		offset = stepsOffset[step];

		timer->setInterval(timerSpeed);

		refreshStatus();

		if (step < stepsLevel.size() - 1) {
			step++;
		}
	}

}

void AbcRainWidget::setBall() {

	if (abclist.size() > 0) {
		// Zufaelligen Buchstaben aus der ABC-Liste entnehmen
		QChar characterTemp = abclist.takeAt((qrand() % abclist.size()));
		// Kugel erzeugen
		int position = 0;
		if ((ballCounter % 4) == 0) {
			// 1/4
			position = (qrand() % ((GroundWidth / 4) - (BallWidth / 2)) + 1) + (BallWidth / 2);
		}
		if ((ballCounter % 4) == 1) {
			// 3/4
			position = (qrand() % (GroundWidth / 4) + 1) + (GroundWidth / 2);
		}
		if ((ballCounter % 4) == 2) {
			// 2/4
			position = (qrand() % (GroundWidth / 4) + 1) + (GroundWidth / 4);
		}
		if ((ballCounter % 4) == 3) {
			// 4/4
			position = (qrand() % ((GroundWidth / 4) - (BallWidth / 2)) + 1) + (3 * GroundWidth / 4);
		}
		charballs.append(
			new CharBall(
				BallWidth,
				position,
				2,
				characterTemp)
			);
		// Schriftzeichen der aktuellen Zeichenliste hinzufuegen
		charlist.append(characterTemp);

		scene->addItem(charballs.last());
		charballs.last()->wind = (qrand() % 8) + 2;
		charballs.last()->rad = 0;

		chartext.append(new QGraphicsTextItem(QString(characterTemp),
            charballs.last()));
		chartext.last()->setFont(QFont("Courier", 16, 100));
		chartext.last()->setPos(-(chartext.last()->boundingRect().width() / 2), -(chartext.last()->boundingRect().height() / 2));

		ballCounter++;

 		if ((ballCounter % 10) == 0) {
			//setTimer(ballSpeed--);
			nextStep();
		}
	}

}

void AbcRainWidget::keyPressEvent(QKeyEvent *event) {
	// A key was pressed, read the typed text
    QString typed = event->text();
	if (typed == "") {
		// Return if no text was typed
		// i.e. if only a modifier was pressed
        return;
    }

	if (gameStarted == 1 && gamePaused == 0) {
		destroyBall(typed[0], 0);
	} else {
		if (typed == " ") {
			if (gameStarted == 0 && gameOvered == 0) {
				startGame();
			} else {
				gamePaused = 0;
                buttonPause->setEnabled(true);
				levelText->setPlainText("");
			}
		}
	}

	// Emit the pressed QChar
	//delete mouse;
	//emit keyPressed(typed[0]);
}

void AbcRainWidget::timerEvent() {

	if (gamePaused == 0) {

		timerCounter++;

		if (timerCounter > timerNewBall) {
			timerCounter = 0;

			//for (int i = 0; i < numBalls; ++i) {
				setBall();
			//}
			numBalls = 1;
		}

		if (showLevel > 20) {
			levelText->setPlainText("");
			scene->update();

			showLevel = 0;
		} else {
			if (showLevel > 0) {
				showLevel++;
				levelText->setDefaultTextColor(QColor(0, 0, 0, 250 - (showLevel * 10)));
				scene->update();
			}
		}

		int xTemp;

		foreach (CharBall *charball, charballs) {
			charball->rad += charball->wind * (qrand() % 2 + 1);
			xTemp = (int) ((qreal) charball->x() - cos((charball->rad / 180) * Pi) * 2);
			if (xTemp < (BallWidth / 2)) {
				xTemp = (BallWidth / 2);
				charball->rad = 90;
			}
			if (xTemp > AreaWidth - (BallWidth / 2)) {
				xTemp = AreaWidth - (BallWidth / 2);
			}
			if (charball->y() < AreaHeight - GroundHeight - (BallWidth / 2)) {
				charball->setPos(xTemp, charball->y() + offset);
			} else {
				charball->setPos(xTemp, AreaHeight - GroundHeight - (BallWidth / 2));
				destroyBall(charball->character(), 1);
				timer->stop();
				setGameOver();
				return;
			}
			//scene->update();
		}

	}

}

void AbcRainWidget::setGameOver() {

    buttonPause->setEnabled(false);

	gameStarted = 0;
	gameOvered = 1;

	QGraphicsTextItem *gameOverText = new QGraphicsTextItem(
		"GAME OVER"
		);
	gameOverText->setFont(QFont("Arial", FontSizeH4, 100));
	gameOverText->setPos((AreaWidth / 2) - (gameOverText->boundingRect().width() / 2),
		(AreaHeight / 2) - (gameOverText->boundingRect().height() / 2));

	QGraphicsTextItem *gameOverTextSub = new QGraphicsTextItem(
        tr("Erreichte Punktzahl: ") + QString::number(shotPoints)
		);
	gameOverTextSub->setFont(QFont("Arial", FontSizeH3, 100));
	gameOverTextSub->setPos((AreaWidth / 2) - (gameOverTextSub->boundingRect().width() / 2),
		(AreaHeight / 2) - (gameOverTextSub->boundingRect().height() / 2) +
		gameOverText->boundingRect().height() + 12);

	scene->addItem(gameOverText);
	scene->addItem(gameOverTextSub);
	scene->update();

}

void AbcRainWidget::setLevel() {
	showLevel = 1;
	//QGraphicsItem *gameOver = new QGraphicsItem(0);
    view->setBackgroundBrush(QPixmap(":/games/img/pattern_" + QString::number(qrand() % 16) + ".png"));

	levelText->setPlainText(
		"LEVEL " + QString::number(level + 1)
		);
	levelText->setDefaultTextColor(QColor(0, 0, 0, 250));
	levelText->setPos((AreaWidth / 2) - (levelText->boundingRect().width() / 2),
		(AreaHeight / 2) - (levelText->boundingRect().height() / 2) - 10);

	if (level == 10) {
		for (int i = 0; i < 26; ++i) {
			abclist.append(QChar(65 + i));
    	}
	}
	//scene->update();

}

 void AbcRainWidget::destroyBall(QChar character, int keep) {

	int i;
	if ((i = charlist.indexOf(character)) == -1) {
		if (shotPoints > 0) {
			shotPoints--;
			refreshStatus();
		}
		return;
	}

	charlist.removeAt(i);
	if (keep == 0) {
		charballs.at(i)->destroy();
		shotPoints++;
	}
	/*delete charballs.at(i);
	charballs.removeAt(i);
	delete chartext.at(i);
	chartext.removeAt(i);*/
	scene->removeItem(chartext.takeAt(i));
	scene->removeItem(charballs.takeAt(i));
	abclist.append(character);

	refreshStatus();

}

 void AbcRainWidget::refreshStatus() {

    points->setPlainText(tr("Level ") + QString::number(level) + "    " +
                         tr("Punkte: ") + QString::number(shotPoints));
	scene->update();

}

void AbcRainWidget::pauseGame() {
	if (gameOvered == 0 && gameStarted != 0) {
		gamePaused = 1;
        buttonPause->setEnabled(false);
		levelText->setFont(QFont("Arial", FontSizeH3, 100));
		levelText->setDefaultTextColor(QColor(0, 0, 0, 250));
        levelText->setPlainText(tr("Leertaste setzt das Spiel fort"));
		levelText->setPos((AreaWidth / 2) - (levelText->boundingRect().width() / 2),
			(AreaHeight / 2) - (levelText->boundingRect().height() / 2) - 10);
		scene->update();
	}
}

void AbcRainWidget::clickReady() {
	// User finished the evaluation
	// -> emit a ready signal
	emit readyClicked();
}

void AbcRainWidget::showHelp() {
	pauseGame();
    helpBrowser = new HelpBrowser("game.html", 0);
	helpBrowser->show();
}

