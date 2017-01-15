

#ifndef ABCRAINWIDGET_H
#define ABCRAINWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QChar>
#include <QList>
#include <QTimer>
#include <QPushButton>

#include "charball.h"
#include "widget/helpbrowser.h"
#include "widget/companylogo.h"


//! The MainWindow class provides the main window of the application.
/*!
	The MainWindow class shows the main window. At startup it shows the widget
	StartWidget. Over functions the widget can be changed also to TrainingWidget
	or Evaluatonwidget. In addition it has an own menu bar.

	@author Tom Thielicke, s712715
	@version 0.1.9
	@date 01.07.2006
*/
class AbcRainWidget : public QWidget {
	Q_OBJECT

	public:

		//! Constructor, initializes the window and shows the start widget.
		/*!
			This contructor initializes the following variable:
				- trainingStarted = false

			In addition, it sets menu actions and the menu bar over functions
			createActions() and createMenu(). After that, it creates the start
			widget over function createStart() and reads the settings over
			function readSettings().
			At the end it sets the window title, icon and window size.

			@see trainingStarted, createActions(), createMenu(), createStart(),
				readSettings()
		*/
		AbcRainWidget(QWidget *parent = 0);

		//! Deconstructor, writes the current settings to hard disk.
		/*!
			This decontructor writes the settings over function
			writeSettings().

			@see writeSettings()
		*/
		~AbcRainWidget();

	private slots:
		void timerEvent();

		//! Slot, connected with the clicked() function of the ready button.
		/*!
			After the user pushed the ready button, this function emits the
			signal readyClicked() to inform the parent class about user
			finished.
		*/
		void clickReady();

		//! Slot, shows the help dialog.
		/*!
			This slot shows the help dialog. It creates an object of
			the QDialog class with an QTextbrowser.
		*/
		void showHelp();

		void pauseGame();

	protected:
		void keyPressEvent(QKeyEvent *event);

	signals:
		void keyPressed(QChar key);

		//! Signal, emits that the user has finished the evaluation.
		/*!
			After the user clicked the ready button, the function clickReady()
			emits this signal to inform the parent class about user finished.
		*/
		void readyClicked();

	private:
		void startGame();
		void refreshStatus();
		void setBall();
		void setGameOver();
		void setLevel();
		void nextStep();
		void destroyBall(QChar character, int keep = 0);
		QGraphicsScene *scene;
		QGraphicsView *view;
		//CharBall **charball;
		QGraphicsRectItem *ground;
		QGraphicsTextItem *points;
		QGraphicsTextItem *levelText;
		QGraphicsTextItem *levelSubText;
		QList<CharBall *> charballs;
		QList<QGraphicsTextItem *> chartext;
		QList<QChar> charlist;
		QList<QChar> abclist;
		QList<int> stepsLevel;
		QList<int> stepsNumBalls;
		QList<int> stepsTimerSpeed;
		QList<int> stepsTimerNewBall;
		QList<int> stepsOffset;
		QTimer *timer;
		QPushButton *buttonHelp;
		QPushButton *buttonReady;
		QPushButton *buttonPause;
     	int timerId;
     	int shotPoints;
		int ballCounter;
		int level;
		int numBalls;
		int timerSpeed;
		int timerCounter;
		int timerNewBall;
		int offset;
		int step;
		int gameStarted;
		int gamePaused;
		int gameOvered;
		int showLevel;

		//! Object of the company logo widget
		CompanyLogo *companyLogo;

		//! Object of the help browser dialog
		HelpBrowser *helpBrowser;
};

#endif //ABCRAINDIALOG_H
