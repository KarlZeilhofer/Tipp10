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
** Implementation of the KeyBoard class
** File name: keyboard.cpp
**
****************************************************************/

#include <QPainter>
#include <QSettings>
#include <QPen>
#include <QCoreApplication>

#include "keyboard.h"
#include "def/defines.h"
#include "def/errordefines.h"
#include "errormessage.h"

KeyBoard::KeyBoard(QWidget *parent) : QWidget(parent) {
	// Read settings (keyboard layout and key coloring)
	readSettings();

    // Load all images used in the keyboard
    loadKeyImages();

    // Start flag
    isStarted = false;
    // Pause flag
    isPaused = false;
    // Flag whether the key properties where found
    keyLayoutFound = false;

    showErrorImage = false;

	// Default values
    activeKey = '0';
    activeX = 0;
	activeY = 0;
    activeColor = 0;
    activeForm = 0;
    activeModifier = 0;
    activeFinger = 8;
    activeModifierX = 0;
	activeModifierY = 0;
    activeModifierColor = 0;
    activeModifierForm = 0;
    activeModifierFinger = 8;
    activeModifier2X = 0;
	activeModifier2Y = 0;
    activeModifier2Color = 0;
    activeModifier2Form = 0;
    activeModifier2Finger = 8;

    colorIndex[0] = 4; // yellow
    colorIndex[1] = 3; // green
    colorIndex[2] = 2; // blue
    colorIndex[3] = 1; // red
    colorIndex[4] = 0; // grey (no additional key)
    keyColors[0] = QColor(253,232,151);
    keyColors[1] = QColor(181,235,157);
    keyColors[2] = QColor(172,215,254);
    keyColors[3] = QColor(249,174,174);


    currentPen.setWidth(3);
    currentPen.setStyle(Qt::DotLine);

    // Sql class to get the key layout of a given char
    keyboardSql = new KeyboardSql(layout);
    // Fix the size of this class because of using fix sized images
    setFixedSize(610, 228);
    setAttribute(Qt::WA_NoSystemBackground);
}

void KeyBoard::startBoard() {
	// Turn start flag true
	isStarted = true;
	// Turn pause flag false
	isPaused = false;

    showErrorImage = false;
	// Call the paint event (refresh the keyboard)
	repaint();
}

void KeyBoard::pauseBoard() {
	// Turn start flag false
	isStarted = false;
	// Turn pause flag true
	isPaused = true;
	// Call the paint event (refresh the keyboard)
	update();
}

void KeyBoard::stopBoard() {
	// Turn start flag false
	isStarted = false;
	// Call the paint event (refresh the keyboard)
	update();
}

void KeyBoard::paintEvent(QPaintEvent *event) {
	// Draw empty grey keyboard
	cleanKeyboard();

	if (isStarted) {
		// Color current key and modifier
		colorKey();
	} else {
		if (keyColoringStart) {
			// Color user's start position
			colorStart();
		}
	}
	// Draw keyboard labeling
	drawLayout();
}

void KeyBoard::loadKeyImages() {
	// Load background
	if (!loadImage(background, ":/img/keyboardbg.png")) {
		return;
	}
	// Keys
	// -> four different key colors
	for (int x = 0; x < 5; x++) {
		// -> six different key forms
		for (int y = 0; y < 9; y++) {
			// Load key
			if (!loadImage(keyImage[x][y], ":/img/key" + QString::number(x)
				+ "_" + QString::number(y) + ".png")) {
				return;
			}
		}
	}
	// -> five different start key colors
	for (int y = 0; y < 5; y++) {
		// Load key
		if (!loadImage(keyImageStart[y], ":/img/key" + QString::number(y)
			+ ".png")) {
			return;
		}
	}
	// Load key labeling
	if (!loadImage(keyLayout, ":/img/" + layout + ".png")) {
		return;
	}
	// Load keyboard border
	if (!loadImage(keyBorder, ":/img/keyboard_border.png")) {
		return;
	}
}

bool KeyBoard::loadImage(QPixmap &img, QString filename) {
	// Check whether loading the image is correct
	if (!img.load(filename)) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_KEY_PIC, TYPE_WARNING, CANCEL_OPERATION);
		return false;
	}
	return true;
}

void KeyBoard::cleanKeyboard() {
	int i;
	QPainter painter(this);
	// Background image
	painter.drawPixmap(0, 0, background);

    // Keys first row
    for (i = 0; i < 13; i++) {
    	painter.drawPixmap(10 + (40 * i), 15, keyImage[0][0]);
	}
	painter.drawPixmap(530, 15, keyImage[0][5]);

    // Enter key QWERTZ; other QWERTY
   	if (layout.left(2) != "us") {
    	painter.drawPixmap(550, 55, keyImage[0][4]);
	} else {
    	painter.drawPixmap(550, 55, keyImage[0][1]);
	}

    // Keys second row
    painter.drawPixmap(10, 55, keyImage[0][6]);
    for (i = 0; i < 12; i++) {
    	painter.drawPixmap(70 + (40 * i), 55, keyImage[0][0]);
	}

    // Keys third row
    painter.drawPixmap(10, 95, keyImage[0][5]);
    if (keyColoringStart) {
		// Four keys left hand
		//COMAK-Release
		/*for (i = 4; i >= 0; i--) {
			if (i != 4 && i != activeFinger && i != activeModifierFinger) {
				painter.drawImage(80 + (40 * i), 95, keyImageStart[4 - i]);
			} else {
				if (i == 4 && (i - 1) != activeFinger && (i - 1) != activeModifierFinger) {
					painter.drawImage(80 + (40 * i), 95, keyImageStart[5 - i]);
				} else {
					painter.drawImage(80 + (40 * i), 95, keyImageStart[0]);
				}
			}
		}*/
		//TIPP10
		for (i = 3; i >= 0; i--) {
			if (i != activeFinger && i != activeModifierFinger && i != activeModifier2Finger) {
				painter.drawPixmap(80 + (40 * i), 95, keyImageStart[colorIndex[i]]);
			} else {
				painter.drawPixmap(80 + (40 * i), 95, keyImageStart[0]);
			}
		}
        painter.drawPixmap(80 + (40 * 4), 95, keyImage[0][0]);
        painter.drawPixmap(80 + (40 * 5), 95, keyImage[0][0]);
        for (i = 6; i < 10; i++) {
			if ((i - 2) != activeFinger && (i - 2) != activeModifierFinger && (i - 2) != activeModifier2Finger) {
				painter.drawPixmap(80 + (40 * i), 95, keyImageStart[colorIndex[9 - i]]);
			} else {
				painter.drawPixmap(80 + (40 * i), 95, keyImageStart[0]);
			}
        }
        painter.drawPixmap(80 + (40 * 10), 95, keyImage[0][0]);
    } else {
    	for (i = 0; i < 11; i++) {
    		painter.drawPixmap(80 + (40 * i), 95, keyImage[0][0]);
		}
	}
    // Enter key QWERTY; other QWERTZ
   	if (layout.left(2) != "us") {
		painter.drawPixmap(80 + (40 * 11), 95, keyImage[0][0]);
	} else {
		painter.drawPixmap(80 + (40 * 11), 95, keyImage[0][7]);
	}

    // Keys fourth row
   	if (layout.left(2) != "us") {
    	painter.drawPixmap(10, 135, keyImage[0][1]);
    	painter.drawPixmap(60, 135, keyImage[0][0]);
	} else {
    	painter.drawPixmap(10, 135, keyImage[0][8]);
	}
    for (i = 1; i < 11; i++) {
    	painter.drawPixmap(60 + (40 * i), 135, keyImage[0][0]);
	}
	painter.drawPixmap(500, 135, keyImage[0][2]);

	// Keys fifth row
    painter.drawPixmap(10, 175, keyImage[0][6]);
    painter.drawPixmap(70, 175, keyImage[0][0]);
	painter.drawPixmap(110, 175, keyImage[0][6]);
	painter.drawPixmap(170, 175, keyImage[0][3]);
	painter.drawPixmap(440, 175, keyImage[0][6]);
	painter.drawPixmap(500, 175, keyImage[0][0]);
	painter.drawPixmap(540, 175, keyImage[0][6]);
}

void KeyBoard::drawLayout() {
	QPainter painter(this);
	// Draw the labeling of the keyboard
	painter.drawPixmap(10, 15, keyLayout);
	// Draw the border of the keyboard
	if (keyDrawBorder) {
		painter.drawPixmap(270, 15, keyBorder);
    }
}

void KeyBoard::colorStart() {
	QPainter painter(this);

	if (keyColoring && !isPaused) {
		// User's startposition
        int i;
		// Four keys left hand
		for (i = 3; i >= 0; i--) {
			painter.drawPixmap(80 + (40 * i), 95, keyImageStart[colorIndex[i]]);
		}
		// Four keys right hand
		for (i = 6; i < 10; i++) {
			painter.drawPixmap(80 + (40 * i), 95, keyImageStart[colorIndex[9 - i]]);
		}


	}
	// Colored space key
	painter.drawPixmap(170, 175, keyImage[1][3]);
}

// Places the current colored keys
void KeyBoard::colorKey() {
    QPainter painter(this);

	if (keyLayoutFound) {

		if (keyDrawPath) {

			int activeFingerTemp = activeFinger;
			if (activeFingerTemp > 3) {
				activeFingerTemp += 2;
			}
			currentPen.setColor(keyColors[4 - colorIndex[4 - activeColor]]);
			painter.setPen(currentPen);
			if ((activeY != 95 || activeX != (activeFingerTemp * 40)) &&
				(activeY != 175 || activeX != 170)) {
				painter.drawLine(100 + (activeFingerTemp * 40), 115, activeX + 20,
					activeY + 20);
			}
			int activeModifierFingerTemp = activeModifierFinger;
			if (activeModifierFingerTemp > 3) {
				activeModifierFingerTemp += 2;
			}
			currentPen.setColor(keyColors[4 - colorIndex[4 - activeModifierColor]]);
			painter.setPen(currentPen);
			if (activeModifier != 0) {
				painter.drawLine(100 + (activeModifierFingerTemp * 40), 115,
					activeModifierX + 20, activeModifierY + 20);
			}
			int activeModifier2FingerTemp = activeModifier2Finger;
			if (activeModifier2FingerTemp > 3) {
				activeModifier2FingerTemp += 2;
			}
			currentPen.setColor(keyColors[4 - colorIndex[4 - activeModifier2Color]]);
			painter.setPen(currentPen);
			if (activeModifier2 != 0) {
				painter.drawLine(100 + (activeModifier2FingerTemp * 40), 115,
					activeModifier2X + 20, activeModifier2Y + 20);
			}
		}

		if (keyColoring) {
			// Color current key
			painter.drawPixmap(activeX, activeY, keyImage[colorIndex[4 - activeColor]][activeForm]);
			if (activeModifier != 0) {
				// Color current modifier
				painter.drawPixmap(activeModifierX, activeModifierY,
					keyImage[colorIndex[4 - activeModifierColor]][activeModifierForm]);
			}
			if (activeModifier2 != 0) {
				// Color current modifier
				painter.drawPixmap(activeModifier2X, activeModifier2Y,
					keyImage[colorIndex[4 - activeModifier2Color]][activeModifier2Form]);
			}
        }

    }
}

void KeyBoard::setErrorImage(QChar key) {
    if (errorImage.load(":/img/" + QString::number(key.unicode()) + ".png")) {
        /*errorImage = errorImage.scaledToHeight(40);
        for (int y = 0; y < errorImage.height(); ++y) {
            QRgb *row = (QRgb*)errorImage.scanLine(y);
            for (int x = 0; x < errorImage.width(); ++x) {
                ((unsigned char*)&row[x])[3] = 50;
            }
        }*/
        showErrorImage = true;
    }
}

void KeyBoard::setKey(QChar key) {
	QString statusText = "0";

    showErrorImage = false;

	// Check whether key is already colored and whether keyboard is started
	if (activeKey != key && isStarted) {
		activeModifierFinger = 8;
		activeModifier2Finger = 8;
		activeFinger = 8;
		// New current key
		activeKey = key;
		// Get the properties of the key
		if (keyboardSql->getKeyLayout(activeKey, &activeX, &activeY,
			&activeColor, &activeForm, &activeModifier, &activeModifier2,
				&activeFinger, &statusText)) {
			keyLayoutFound = true;
			if (activeModifier != 0) {
				// If there is also a modifier get the properties, too
				if (!keyboardSql->getModifierLayout(activeModifier,
					&activeModifierX, &activeModifierY, &activeModifierColor,
					&activeModifierForm, &activeModifierFinger, &statusText)) {
					activeModifier = 0;
				}
			}
			if (activeModifier2 != 0) {
				// If there is also a modifier get the properties, too
				if (!keyboardSql->getModifierLayout(activeModifier2,
					&activeModifier2X, &activeModifier2Y, &activeModifier2Color,
					&activeModifier2Form, &activeModifier2Finger, &statusText)) {
					activeModifier2 = 0;
				}
			}
		} else {
			// No properties were found
			keyLayoutFound = false;
		}
		// Emit the help text of current key(s)
		emit statusRefreshed(statusText);
    	// Call the paint event (refresh the keyboard)
    	if (keyColoring || keyColoringStart || keyDrawPath) {
			repaint();
		}
	}
}

void KeyBoard::readSettings() {
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif

	settings.beginGroup("general");
	layout = settings.value("language_layout", APP_STD_LANGUAGE_LAYOUT).toString();
	settings.endGroup();
	// Check if key coloring is turned on
	settings.beginGroup("support");
	keyColoring = settings.value("check_selection", true).toBool();
	keyColoringStart = settings.value("check_selection_start", true).toBool();
	keyDrawBorder = settings.value("check_border", true).toBool();
	keyDrawPath = settings.value("check_path", true).toBool();
    settings.endGroup();
}
