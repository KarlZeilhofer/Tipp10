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
** File name: numpad.cpp
**
****************************************************************/

#include <QPainter>
#include <QSettings>
#include <QPen>
#include <QCoreApplication>

#include "numpad.h"
#include "../def/defines.h"
#include "../def/errordefines.h"
#include "errormessage.h"

NumPad::NumPad(QWidget *parent) : QWidget(parent) {
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

	// Default values
    activeKey = '0';
    activeX = 0;
	activeY = 0;
    activeColor = 0;
    activeForm = 0;
    activeFinger = 8;

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
    setFixedSize(180, 228);
    setAttribute(Qt::WA_NoSystemBackground);
}

void NumPad::startBoard() {
	// Turn start flag true
	isStarted = true;
	// Turn pause flag false
	isPaused = false;
	// Call the paint event (refresh the keyboard)
	repaint();
}

void NumPad::pauseBoard() {
	// Turn start flag false
	isStarted = false;
	// Turn pause flag true
	isPaused = true;
	// Call the paint event (refresh the keyboard)
	update();
}

void NumPad::stopBoard() {
	// Turn start flag false
	isStarted = false;
	// Call the paint event (refresh the keyboard)
	update();
}

void NumPad::paintEvent(QPaintEvent *event) {
	// Draw empty grey keyboard
	cleanKeyboard();

	if (isStarted) {
		// Color current key and modifier
		colorKey();
	}
	// Draw keyboard labeling
	drawLayout();
}

void NumPad::loadKeyImages() {
	// Load background
	if (!loadImage(background, ":/img/numpadbg.png")) {
		return;
	}
	// Keys
	// -> four different key colors
	for (int x = 0; x < 5; x++) {
		// -> six different key forms
		for (int y = 0; y < 10; y++) {
			// Load key
			if (!loadImage(keyImage[x][y], ":/img/key" + QString::number(x)
				+ "_" + QString::number(y) + ".png")) {
				return;
			}
		}
	}
	// Load key labeling
	if (!loadImage(keyLayout, ":/img/" + layout.left(2) +
		"_numpad_" + layout.right(3) + ".png")) {

		return;
	}
}

bool NumPad::loadImage(QPixmap &img, QString filename) {
	// Check whether loading the image is correct
	if (!img.load(filename)) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_KEY_PIC, TYPE_WARNING,
			CANCEL_OPERATION, filename);
		return false;
	}
	return true;
}

void NumPad::cleanKeyboard() {
	int i;
	QPainter painter(this);
	// Background image
	painter.drawPixmap(0, 0, background);

    // Keys first row
    for (i = 0; i < 4; i++) {
    	painter.drawPixmap(10 + (40 * i), 15, keyImage[0][0]);
	}

    // Keys second row
    for (i = 0; i < 3; i++) {
    	painter.drawPixmap(10 + (40 * i), 55, keyImage[0][0]);
	}
   	if (!isMac) {
    	painter.drawPixmap(130, 55, keyImage[0][9]);
	} else {
    	painter.drawPixmap(130, 55, keyImage[0][0]);
	}

    // Keys third row
    for (i = 0; i < 3; i++) {
    	painter.drawPixmap(10 + (40 * i), 95, keyImage[0][0]);
	}
   	if (layout.right(3) == "mac") {
    	painter.drawPixmap(130, 95, keyImage[0][0]);
	}

    // Keys fourth row
    for (i = 0; i < 3; i++) {
    	painter.drawPixmap(10 + (40 * i), 135, keyImage[0][0]);
	}
    painter.drawPixmap(130, 135, keyImage[0][9]);

    // Keys fifth row
    painter.drawPixmap(10, 175, keyImage[0][7]);
    painter.drawPixmap(90, 175, keyImage[0][0]);
}

void NumPad::drawLayout() {
	QPainter painter(this);
	// Draw the labeling of the keyboard
	painter.drawPixmap(10, 15, keyLayout);
}

// Places the current colored keys
void NumPad::colorKey() {
	QPainter painter(this);

	if (keyLayoutFound) {

		if (keyColoring) {
			// Color current key
			painter.drawPixmap(activeX, activeY,
				keyImage[colorIndex[4 - activeColor]][activeForm]);
		}

	}
}

void NumPad::setKey(QChar key) {
	QString statusText = "0";

	// Check whether key is already colored and whether keyboard is started
	if (activeKey != key && isStarted) {
		activeFinger = 8;
		// New current key
		activeKey = key;
		// Get the properties of the key
		if (keyboardSql->getNumLayout(activeKey, &activeX, &activeY,
			&activeColor, &activeForm, &activeFinger, &statusText)) {
			keyLayoutFound = true;
		} else {
			// No properties were found
			keyLayoutFound = false;
		}
		// Emit the help text of current key(s)
		emit statusRefreshed(statusText);
    	// Call the paint event (refresh the keyboard)
    	if (keyColoring) {
			repaint();
		}
	}
}

void NumPad::readSettings() {
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	// Check user's keyboard layout
	settings.beginGroup("general");
	layout = settings.value("language_layout", APP_STD_LANGUAGE_LAYOUT).toString();
	settings.endGroup();
	// Check if key coloring is turned on
	settings.beginGroup("support");
	keyColoring = settings.value("check_selection", true).toBool();
	settings.endGroup();
}
