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
** Definition of the CompanyLogo class
** File name: illustrationimage.h
**
****************************************************************/

#ifndef ILLUSTRATIONIMAGE_H
#define ILLUSTRATIONIMAGE_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QString>

//! The IllustrationImage class provides a company logo widget.
/*!
	The IllustrationImage class loads a company image and provide it in a widget.
	By default a orange logo is load. If parameter white in contructor is true
	the logo is white (used not be an eye catcher while practice a lesson).

	@author Tom Thielicke, s712715
	@version 0.0.4
	@date 02.06.2006
*/
class IllustrationImage : public QWidget {

	public:

		//! Constructor: loads the company logo and resizes the widget.
		/*!
			In this contructor the function loadLogo is called. If parameter
			white is set true a white logo will be loaded.
			At the end the widget size is set to the fixed size of the image.

			@param parent The parent QWidget
			@param white Flag to load a white or orange image
			@see loadLogo
		*/
		IllustrationImage(QString filename, QString version = "", QWidget *parent = 0);

	protected:

		//! Paintevent, draws the company logo.
		/*!
			The paintevent is called every time when the window changes.
			It draws the logo (that was loaded in variable logo) in the left
			top corner of the widget.

			@param event Event of the paint function
			@see logo
		*/
		void paintEvent(QPaintEvent *event);

	private:

		//! Loads the company image in a QImage object logo.
		/*!
			This function defines the file name of the logo and load it in the
			QImage object logo. If white is true the file name is "logo_white.png",
			if white is false the file name is "logo_orange.png". The files are
			located in the directory "img/".

			@param white Flag to load a white or orange image
			@return Bool value
			@see logo
		*/
		bool loadImage(QString filename);

		//! Object of the company logo.
		QImage image;
		QString imageName;
		QString versionLabel;
};

#endif // ILLUSTRATIONIMAGE_H
