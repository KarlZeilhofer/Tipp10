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
** Definition of the KeyBoard class
** File name: keyboard.h
**
****************************************************************/

#ifndef NUMPAD_H
#define NUMPAD_H

#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QChar>
#include <QString>
#include <QColor>
#include <QPen>

#include "../sql/keyboardsql.h"

//! The KeyBoard class provides a virtuell keyboard widget.
/*!
	The KeyBoard class normally receives the current key over slot setKey().
	It then colors the current key if keyColoring is true. When a key is
	pressed the class receive the written char with a QKeyEvent and emits the
	char with signal keyPressed().
	Start and stop the keyboard class with function startBoard and stopBoard.
	When the keyboard is stopped it colors the startposition(the eight keys
	where users fingers musst place on start) and the space key.
	Pause it with function pauseBoard. When the keyboard is paused ist colors
	only the space key.
	The store the properties of the current key and modifier and to access it
	every time over the paint event, private variables are used.

	@author Tom Thielicke, s712715
	@version 0.1.6
	@date 16.06.2006
*/
class NumPad : public QWidget {
	// Necessary to create own signals, slots and connections
    Q_OBJECT

	public:

		//! Constructor: initializes variables, images, settings and sql class.
		/*!
			In this contructor the following variables are initialized:
				- isStarted = false
				- isPaused = false
				- keyLayoutFound = false
				- activeKey = '0'
				- activeX = 0
				- activeY = 0
				- activeColor = 0
				- activeForm = 0
				- activeModifier = 0
				- activeModifierX = 0
				- activeModifierY = 0
				- activeModifierColor = 0
				- activeModifierForm = 0

			In addition, the settings are read over function readSettings()
			and all png image files are loaded over function loadKeyImages().
			The widget is set to a fixed size because of using fix sized images.

			@param parent The parent QWidget
			@attention To ensure that the KeyEvent is acting it is very
				important that there is a strong focus on this widget
			@see readSettings(), loadKeyImages(), keyboardSql, isStarted,
				isPaused, keyLayoutFound, activeKey, activeX, activeY,
				activeColor, activeForm, activeModifier, activeModifierX,
				activeModifierY, activeModifierColor, activeModifierForm
		*/
		NumPad(QWidget *parent = 0);

	signals:

		//! Signal, emits the current text for the status bar.
		/*!
			This signal is used to transmit a help text for the status bar.
			It is used in the function setKey(). After a new key was
			successfully set, the functions getKeyLayout() and
			getModifierLayout() put a status text together which is then
			emitted over this signal.
		*/
		void statusRefreshed(QString statustext);

	public slots:

		//! Slot, sets the current char and refreshes the keyboard.
		/*!
			Only if parameter key is a different char than the current one and
			the flag isStarted is true, this function converts the key into
			properties and statustext of current key and modifier.
			After a successfully convertion update() is called to repaint the
			keyboard and the signal statusRefreshed() is emited.

			@pre Flag isStarted must be true
			@param key Unicode char of the current key
			@see activeKey, activeX, activeY, activeColor, activeForm,
				activeModifier, activeModifierX, activeModifierY,
				activeModifierColor, activeModifierForm
		*/
		void setKey(QChar key);

		//! Slot, starts the keyboard.
		/*!
			This slot turns the flag isStarted to true and the flag isPaused
			to false. After that update() is called to repaint the keyboard.

			@see isStarted, isPaused
		*/
		void startBoard();

		//! Slot, stops the keyboard.
		/*!
			This slot stops the keyboard by turning flag isStarted to false.
			After that update() is called to repaint the keyboard.

			@see isStarted
		*/
		void stopBoard();

		//! Slot, pauses the keyboard.
		/*!
			This slot turns the flag isStarted to false and the flag isPaused
			to true. After that update() is called to repaint the keyboard.

			@see isStarted, isPaused
		*/
		void pauseBoard();

	protected:

		//! Paintevent, draws current view of the keyboard with functions.
		/*!
			The paintevent is called every time when the window changes or
			the function update() is called.
			It first executes the function cleanKeyboard() to get a full
			grey board, after that it colors the current key and modifier
			over function colorKey() if flag isStarted is true. If not it
			calls instead the function colorStart() to show the startposition
			or a paused keyboard. At the end it excutes the function
			drawLayout() to give the keyboard its labeling.

			@param event Event of the paint function
			@see isStarted, cleanKeyboard(), colorKey(), colorStart(),
				drawLayout()
		*/
		void paintEvent(QPaintEvent *event);

	private:

		//! Puts the background and grey key images together to a keyboard.
		/*!
			This function places the preloaded background image and all grey
			key images to a full keyboard (without labels). This happens with
			fixed coordinates because the function is called always when the
			paintevent reacts. Loading the coordinates every time from the sql
			database would be very slow and it is not necessary because the
			keyboard layout is always fix (it doesn't make any difference
			wether the keyboard is an apple or windows board).
			The preloaded key images are located in the private QImage array
			keyImage[][] and the background image in the private QImage
			variable background.

			@see keyImage[][], background
		*/
		void cleanKeyboard();

		//! Colors the current key and possibly the current modifier.
		/*!
			If flag keyColoring is true and a key layout was found
			(keyLayoutFound is true) this function draws the current key and
			possibly a modifier with the current properties.

			@pre flag keyLayoutFound is true and flag isColoring is true
			@see activeKey, activeX, activeY, activeColor, activeForm,
				activeModifier, activeModifierX, activeModifierY,
				activeModifierColor, activeModifierForm, keyImage[][]
		*/
		void colorKey();

		//! Sets the keyboard labeling.
		/*!
			This function draws the image loaded in variable keyLayout. It is
			necessary to set the image not until the full keyboard is drawn
			because the labeling could be covered.

			@see keyLayout
		*/
		void drawLayout();

		//! Loads all images of the keyboard.
		/*!
			This function loads the background image, the key images (there
			are six different forms, every form has four different colors) and
			the labeling of the keyboard. To load the image the function
			loadImage() is used.
			The filenames of the keys are located in the directory "img/" and
			are labeled "key_x_y.png", where x (0-4) is the color and y (0-6)
			is the form of every key.
			The labeling of the full keyboard is defined in the file
			keylayout_win and keylayout_mac respectively.

			@see loadImage(), background, keyImage[][], keyLayout
		*/
		void loadKeyImages();

		//! Loads an image.
		/*!
			This function loads the image filename to the QImage img.

			@param img The QImage which gets the image
			@param filename The file name of the image
			@return Bool value true if funtion succeeded
		*/
		bool loadImage(QPixmap &img, QString filename);

		//! Reads the keyboard layout and the key coloring settings.
		/*!
			This function checks whether the user has selected a windows
			or macintosh keyboard layout and if key coloring is turned on
			or off.

			@see opSystem, keyColoring
		*/
		void readSettings();

		//! Holds the index (the order) of the key colors.
		int colorIndex[4];

		//! Holds the images of the keys.
		QColor keyColors[4];

		//! Holds the images of the keys.
		QPixmap keyImage[5][10];

		//! Holds the image of the keyboard layout.
		QPixmap keyLayout;

		//! Holds the image of the keyboard background.
		QPixmap background;

		//! Object of the corresponding sql class.
		KeyboardSql *keyboardSql;

		//! Flag, keyboard is started.
		bool isStarted;

		//! Flag, keyboard is paused.
		bool isPaused;

		//! Flag, the properties of the current key where found.
		bool keyLayoutFound;

		//! Flag, coloring of key is turned on or off.
		bool keyColoring;

		//! Unicode char of the current key.
		QChar activeKey;

		//! Unicode char of the current key.
		bool isMac;

		//! X coordinate of the current key.
		int activeX;

		//! Y coordinate of the current key.
		int activeY;

		//! Color of the current key (0-4).
		int activeColor;

		//! Form of the current key (0-6).
		int activeForm;

		//! Finger code of the current key (0-7).
		int activeFinger;

		//! Used keyboard layout.
		QString layout;

		QPen currentPen;
};

#endif // NUMPAD_H
