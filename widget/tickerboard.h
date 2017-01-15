/*
Copyright (c) 2006-2009, Tom Thielicke IT Solutions

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.
*/

/****************************************************************
**
** Definition of the TickerBoard class
** File name: tickerboard.h
**
****************************************************************/

#ifndef TICKERBOARD_H
#define TICKERBOARD_H

#include <QWidget>
#include <QChar>
#include <QString>
#include <QShowEvent>
#include <QHideEvent>
#include <QPaintEvent>
#include <QTimer>
#include <QFont>
#include <QColor>
#include <QPixmap>
#include <QImage>
#include <QStringList>
#include <QKeyEvent>

//! The TickerBoard class provides a text ticker widget.
/*!
	The TickerBoard class provides a text ticker widget. The text can be set
	and update. It counts the tokens a requests new text if nessesary.

	@author Tom Thielicke, s712715
	@version 0.2.4
	@date 01.07.2006
*/
class TickerBoard : public QWidget {
    Q_OBJECT

	public:

		//! Constructor, creates a status bar with three text parts.
		/*!
			In this contructor the following variables are initialized:
				- txtCurrentLesson = "";
				- txtCompleteLesson = "";
				- counterCurrentLesson = 0;
				- counterCompleteLesson = 0;
				- counterRow = 0;
				- lengthCurrentLesson = 0;
				- lengthCompleteLesson = 0;
				- txtPause = "";
				- lessonOffset = 0;
				- scrollOffset = 0;
				- widthSelection = 0;
				- widthCurrentLesson = 0;
				- startFlag = false;

			In addition, the image "tickerbg.png" is loaded to variable
			background, settings are read and a timer signal is connected
			to the slot progress()

			@param parent The parent QWidget
			@see readSettings(), txtCurrentLesson, txtCompleteLesson,
				counterCurrentLesson, counterCompleteLesson, counterRow,
				lengthCurrentLesson, lengthCompleteLesson, txtPause,
				lessonOffset, scrollOffset, widthSelection, widthCurrentLesson,
				startFlag, colorSelection, tickerFont, background
		*/
		TickerBoard(QWidget *parent = 0);

		//! Sets the ticker speed (timer intervall).
		/*!
			@param speed The ticker speed
			@see tickerSpeed, timer
		*/
		void setSpeed(int speed);

	signals:

		//! Signal, emits a signal if current char has changed.
		/*!
			@param newChar The new char
			@see getNewChar()
		*/
		void charChanged(QChar newChar);

		//! Signal, emits a signal if new text is necessary.
		void updateRequired();

		//! Signal, emits a signal if lesson is ready.
		void isReady();

		//! Signal, emits the char of the current pressed key
		/*!
			After a key was pressed, this signal is emitted with the unicode
			char of the pressed key. The signal is used in the function
			keyPressEvent()
		*/
		void keyPressed(QChar key);

	public slots:

		//! Slot, refreshes the variables and starts the ticker.
		/*!
			@see charChanged(), lengthCompleteLesson, txtCompleteLesson,
				tickerFont, widthSelection, txtCurrentLesson,
				counterCurrentLesson, widthCurrentLesson, newChar,
				startFlag
		*/
        void startTicker(bool wasPaused = false);

		//! Slot, pauses the ticker and shows an pause test.
		/*!
			@param txt Pause text
			@see startFlag, txtPause
		*/
		void pauseTicker(QString txt = tr("Leertaste setzt das "
			"Diktat fort"));

		//! Slot, sets the text of the ticker.
		/*!
			@param txt Ticker text
			@see splitLesson(), checkUpdateRequired(), txtCompleteLesson
		*/
		void setTicker(QString txt = "");

		//! Slot, extends the text of the ticker.
		/*!
			@param txt Ticker extend text
			@param seperator Text seperator
			@see splitLesson(), checkUpdateRequired(), txtCompleteLesson
		*/
		void extendTicker(QString txt = "", QString seperator = "");

		//! Slot, changes the current char of the text.
		/*!
			@see changeChar(), startFlag, colorSelection
		*/
		void getNewChar();

		bool getOldChar();

		void insertErrorChar(QChar ch);
		void removeErrorChar();

	private slots:

		//! Slot, moves the ticker.
		/*!
			This slot moves the ticker 1 pix to the left. If difference
			of lessonOffset and scrollOffset is bigger 160 pixel the
			ticker scrolls faster.

			@see scrollOffset, lessonOffset
		*/
		void progress();

	protected:

		//! Starts the timer.
		/*!
			This function is called at the beginning and starts the timer
			with a timer intervall tickerSpeed.

			@see tickerSpeed
		*/
		void showEvent(QShowEvent*) { timer.start(40); }

		//! Stops the timer.
		/*!
			This function is called in the end and stops the timer.
		*/
		void hideEvent(QHideEvent*) { timer.stop(); }

		//! Draws the ticker and the current text with a char selection.
		/*!
			This function is called in the end and stops the timer.

			@param event The paint event
			@see startFlag, tickerFont, colorSelection, scrollOffset,
				lessonOffset, widthCurrentLesson, txtCurrentLesson,
				txtPause, background
		*/
		void paintEvent(QPaintEvent *event);

		//! Keyevent, reacts on every key press and emits the unicode char.
		/*!
			The keyevent detect a pressed key, the function then reads the full
			QString of the pressed key. It represents exactly the value which
			the user type in, thus inclusive the possibly pressed modifier.
			To know if there was only pressed a modifier we have to check if
			the QString is empty.
			After that only a QChar (the first index of the QString) is emitted
			over the function keyPressed().
			It is necessary to do that not only if the keyboard is started
			because we must also register a pressed space key to start the
			keyboard i.e. over a pressed space key.

			@param event Event of keyboard
			@see keyPressed()
		*/
		void keyPressEvent(QKeyEvent *event);

	private:

		//! Chances the current char selection to the next char.
		/*!
			This function changes the current char selection and
			emits the slot charChanged. After that it checks
			whether a new text is required.

			@see isReady(), charChanged(), checkUpdateRequired(),
				counterCurrentLesson, counterCompleteLesson, txtPause,
				startFlag, lengthCompleteLesson, lengthCurrentLesson,
				lessonOffset, widthSelection, newChar
		*/
		void nextChar();

		/*!
		 * \brief Goes to previous char in currentLession
		 * \return true on success
		 */
		bool prevChar();

		void updateSelection();

		//! Splits the text into rows over a fix token.
		/*!
			@see counterCurrentLesson, counterCompleteLesson,
				lengthCompleteLesson, lengthCurrentLesson
		*/
		void splitLesson();

		//! Checks whether a new text is required.
		/*!
			@see isReady(), charChanged(), checkUpdateRequired(),
				counterCurrentLesson, counterCompleteLesson, txtPause,
				startFlag, lengthCompleteLesson, lengthCurrentLesson,
				lessonOffset, widthSelection, newChar
		*/
		void checkUpdateRequired();

		//! Reads all user settings.
		void readSettings();

		//! Flag ticker board is started
		bool startFlag;

		//! Text of the current row of the ticker
		QString txtCurrentLesson;

		//! Complete Text of the ticker
		QString txtCompleteLesson;

		//! Counter of chars of the current text
		int counterCurrentLesson;

		int uncorrectedErrors;

		//! Counter of chars of the complete text
		int counterCompleteLesson;

		//! Number of chars of the current text
		int lengthCurrentLesson;

		//! Number of chars of the complete text
		int lengthCompleteLesson;

		//! Counter of the rows
		int counterRow;

		//! Width of until the current char in pix
		int widthCurrentLesson;

		//! Width of current char selection in pix
		int widthSelection;

		int widthErrorSelection;

		//! Offset of the lesson in pix
		int lessonOffset;

		//! Offset of the scroll in pix
		int scrollOffset;

		int scrollCounter;

		//! List of all text rows
		QStringList txtLessonSplited;

		//! Pause text
		QString txtPause;

		//! Speed of the ticker
		int tickerSpeed;

		//! Current speed of the ticker
		int tickerCurrentSpeed;

		//! Current char (with char selection)
		QChar newChar;

		//! Timer for the ticker move
		QTimer timer;

		//! Ticker font
		QFont tickerFont;

        //! Color of the char selection
        QColor colorFont;
        QColor colorBg;
        QColor colorCursor;
        QColor colorSelection;

		//! Background image
        QPixmap background;

};

#endif // TICKERBOARD_H
