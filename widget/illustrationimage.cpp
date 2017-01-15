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
** Implementation of the CompanyLogo class
** File name: illustrationimage.cpp
**
****************************************************************/

#include <QPainter>

#include "illustrationimage.h"
#include "def/defines.h"
#include "def/errordefines.h"
#include "errormessage.h"

IllustrationImage::IllustrationImage(QString filename, QString version, QWidget *parent) : QWidget(parent) {
	imageName = filename;
	versionLabel = version;
	// Load the company logo
	if (!loadImage(filename)) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_LOGO_PIC, TYPE_WARNING,
			CANCEL_OPERATION);
	}
	// Set widget size to image size (fixed)
    setFixedSize(image.width(), image.height());
    setMinimumSize(image.width(), image.height());
}

void IllustrationImage::paintEvent(QPaintEvent *revent) {
	QPainter painter(this);
	// Draw the image in the left top corner of the widget
	painter.drawImage(0, 0, image);
	if (imageName.left(5) == "about" && versionLabel != "") {
		#if APP_WIN
		painter.drawText(360, 10, 150, 100, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
			versionLabel);
		#endif
		#if APP_MAC
		painter.drawText(340, 10, 150, 100, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
			versionLabel);
		#endif
		#if APP_X11
		painter.drawText(300, 10, 170, 100, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
			versionLabel);
		#endif
	}
}

bool IllustrationImage::loadImage(QString filename) {
	// Load the image
	if (!image.load(":/img/" + filename)) {
		return false;
	}
	return true;
}
