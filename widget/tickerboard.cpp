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
** Implementation of the TickerBoard class
** File name: tickerboard.h
**
****************************************************************/

#include <QPainter>
#include <QFontMetrics>
#include <QSettings>
#include <QRgb>
#include <QCoreApplication>

#include "tickerboard.h"
#include "def/errordefines.h"
#include "errormessage.h"
#include "def/defines.h"

TickerBoard::TickerBoard(QWidget *parent) : QWidget(parent) {
	txtCurrentLesson = "";
	txtCompleteLesson = "";
	counterCurrentLesson = 0;
	counterCompleteLesson = 0;
	counterRow = 0;
	lengthCurrentLesson = 0;
	lengthCompleteLesson = 0;
	txtPause = "";
	lessonOffset = 0;
	scrollOffset = 0;
	scrollCounter = 0;
	widthSelection = 0;
    widthCurrentLesson = 0;
    startFlag = false;
    colorSelection = colorCursor;
    tickerFont = QFont(FONT_STANDARD, FONT_SIZE_TICKER);
    tickerFont.setStyleStrategy(QFont::PreferAntialias);
	QFontMetrics fm(tickerFont);

	if (!background.load(":/img/tickerbg.png")) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_TICKER_PIC, TYPE_WARNING,
			CANCEL_OPERATION);
	}

    readSettings();

    connect(&timer, SIGNAL(timeout()), this, SLOT(progress()));

    setFixedSize(610, 65);

    setAttribute(Qt::WA_NoSystemBackground);

	// It is very important to set a strong focus on this widget
	// because it has always to receive the QKeyEvent event!!!
    setFocusPolicy(Qt::StrongFocus);
}

void TickerBoard::keyPressEvent(QKeyEvent *event) {
	// A key was pressed, read the typed text
    QString typed = event->text();
	if (typed == "") {
		// Return if no text was typed
		// i.e. if only a modifier was pressed
        return;
    }
	// Emit the pressed QChar
	emit keyPressed(typed[0]);
}

void TickerBoard::startTicker(bool wasPaused) {
	if ((lengthCompleteLesson = txtCompleteLesson.length()) > 0) {
		QFontMetrics fm(tickerFont);
		// Check current text and position
    	widthSelection = fm.charWidth(txtCurrentLesson, counterCurrentLesson);
    	widthCurrentLesson = fm.width(txtCurrentLesson);
		newChar = txtCurrentLesson[counterCurrentLesson];

		if (tickerSpeed == 50) {
			scrollOffset = 290;
			scroll(-290, 0, QRect::QRect(10, 15, 590, 35)); //contentsRect());
		}

		startFlag = true;
		repaint();
        if (!wasPaused) {
            emit charChanged(newChar);
        }
	}
}

void TickerBoard::pauseTicker(QString txt) {
	startFlag = false;
	txtPause = txt;
	update();
}

void TickerBoard::setTicker(QString txt) {
	txtCompleteLesson = txt;
	splitLesson();
	checkUpdateRequired();
}

void TickerBoard::extendTicker(QString txt, QString seperator) {
	txtCompleteLesson.append(seperator + txt);
	splitLesson();
	checkUpdateRequired();
}

void TickerBoard::getNewChar() {
    colorSelection = colorCursor;
	if (startFlag) {
		changeChar();
	}
}

void TickerBoard::changeChar() {
	counterCurrentLesson++;
	counterCompleteLesson++;
	if (counterCompleteLesson >= lengthCompleteLesson) {
		// No more text available
		txtPause = tr("Diktat beendet");
		startFlag = false;
		update();
		emit isReady();
		emit charChanged(' ');
		return;
	}
	if (counterCurrentLesson >= lengthCurrentLesson) {
		// Take a new text row
		counterCurrentLesson = 0;
		counterRow++;
		if (tickerSpeed != 50) {
			scrollOffset = 0;
		} else {
			scrollOffset = 290;
			scroll(-290, 0, QRect::QRect(10, 15, 590, 35)); //contentsRect());
		}
		splitLesson();
	}
	QFontMetrics fm(tickerFont);
    //txtSelectionWidth = fm.width(txtLesson[txtCounter]);
    lessonOffset = fm.width(txtCurrentLesson, counterCurrentLesson);
	widthSelection = fm.charWidth(txtCurrentLesson, counterCurrentLesson);
	newChar = txtCurrentLesson[counterCurrentLesson];
	repaint();
	emit charChanged(newChar);
	checkUpdateRequired();
}

void TickerBoard::checkUpdateRequired() {
	// Check whether a text update is required
	if ((lengthCompleteLesson - counterCompleteLesson)
		< NUM_TOKEN_UNTIL_REFRESH) {
		// Text update required
		emit updateRequired();
	}
}

void TickerBoard::setErrorSelection() {
	colorSelection = QColor(249, 126, 0);
	repaint();
}

void TickerBoard::clearErrorSelection() {
    colorSelection = colorCursor;
	repaint();
}

void TickerBoard::setSpeed(int speed) {
	//tickerSpeed = speed;
	if (timer.isActive() && tickerCurrentSpeed != speed) {
		tickerCurrentSpeed = speed;
		timer.setInterval(speed);
	}
}

void TickerBoard::splitLesson() {
	QFontMetrics fm(tickerFont);
	lengthCompleteLesson = txtCompleteLesson.length();
	// Split lection into sentences with line break sign at the end
	// (split case sensitive and skip empty parts)
	txtLessonSplited = txtCompleteLesson.split(QChar(TOKEN_NEW_LINE),
		QString::SkipEmptyParts, Qt::CaseSensitive);
	txtCurrentLesson = txtLessonSplited.at(counterRow) + QChar(TOKEN_NEW_LINE);
	lengthCurrentLesson = txtCurrentLesson.length();
	widthCurrentLesson = fm.width(txtCurrentLesson);
	repaint();
}

void TickerBoard::paintEvent(QPaintEvent *event) {
	int x;
    QPainter painter(this);
	painter.setPen(Qt::NoPen);
    painter.setBrush(colorBg);
    painter.drawRect(10, 15, 590, 35);
	if (startFlag) {
		// Draw current ticker position
		x = 290;
		painter.setFont(tickerFont);
		painter.setBrush(colorSelection);
		painter.drawRect(10 + x - scrollOffset + lessonOffset, 15,
			widthSelection, 35);
        painter.setPen(colorFont);
		painter.drawText(10 + x - scrollOffset, 15, widthCurrentLesson, 35,
			Qt::AlignVCenter, txtCurrentLesson);
	} else {
		// Draw pause or start text
		QFont tmpfont = QFont(FONT_STANDARD, FONT_SIZE_TICKER_PAUSE);
		tmpfont.setStyleStrategy(QFont::PreferAntialias);
		painter.setFont(tmpfont);
        painter.setPen(colorFont);
		painter.drawText(10, 15, 590, 35, Qt::AlignCenter | Qt::AlignVCenter,
			txtPause);
    }
	painter.drawPixmap(0, 0, background);
}

void TickerBoard::progress() {
	if (startFlag) {
		if (tickerSpeed != 50) {
			if (scrollOffset < lessonOffset) {

				// Move ticker 1 pixel to left
				scrollOffset++;
				scroll(-1, 0, QRect::QRect(10, 15, 590, 35)); //contentsRect());

				if ((lessonOffset - scrollOffset) <= 30) {
					setSpeed(tickerSpeed);
				}
				if ((lessonOffset - scrollOffset) > 30 && (lessonOffset - scrollOffset) <= 60) {
					setSpeed(tickerSpeed - (3 * (tickerSpeed / 10)));
				}
				if ((lessonOffset - scrollOffset) > 60 && (lessonOffset - scrollOffset) <= 90) {
					setSpeed(tickerSpeed - (5 * (tickerSpeed / 10)));
				}
				if ((lessonOffset - scrollOffset) > 90) { // && (lessonOffset - scrollOffset) <= 120) {
					setSpeed(tickerSpeed - (8 * (tickerSpeed / 10)));
				}
				/*if ((lessonOffset - scrollOffset) > 120) {
					setSpeed(1);
				}*/

			}
			// If the user types faster than the ticker, move ticker faster after
			// 160 pixels overage (because the user must see at least the next word)
			if ((lessonOffset - scrollOffset) > 200) {
				scrollOffset += (lessonOffset - scrollOffset) - 200;
				scroll(-((lessonOffset - scrollOffset) - 200), 0, QRect::QRect(10, 15, 590, 35)); //contentsRect());
			}
		} else {
			// If the user types faster than the ticker, move ticker faster after
			// 160 pixels overage (because the user must see at least the next word)
			if ((lessonOffset - scrollOffset) > 280) {
				scrollOffset += 570;
				scroll(-570, 0, QRect::QRect(10, 15, 590, 35)); //contentsRect());
			}

		}
	}
}

void TickerBoard::readSettings() {
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
    settings.beginGroup("settings");
    colorFont = QColor(settings.value("ticker_font_color",
                                      QString(TICKER_COLOR_FONT)).toString());
    colorBg = QColor(settings.value("ticker_bg_color",
                                    QString(TICKER_COLOR_BG)).toString());
    colorCursor = QColor(settings.value("ticker_cursor_color",
                                           QString(TICKER_COLOR_CURSOR)).toString());
    colorSelection = colorCursor;
	tickerFont.fromString(settings.value("ticker_font",
		tickerFont.toString()).toString());
	setSpeed(tickerSpeed = 50 - (settings.value("ticker_speed",
		TICKERSPEED_STANDARD).toInt() * 10));
	//tickerSpeed = 38 + (settings.value("ticker_speed",
	//	TICKERSPEED_STANDARD).toInt() * 10);
	//setSpeed(1);
	settings.endGroup();
}
