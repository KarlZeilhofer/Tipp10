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
** File name: companylogo.cpp
**
****************************************************************/

#include <QString>
#include <QPainter>

#include "companylogo.h"
#include "def/errordefines.h"
#include "def/defines.h"
#include "errormessage.h"

CompanyLogo::CompanyLogo(QWidget *parent, bool white) : QWidget(parent) {
	// Load the company logo
	loadLogo(white);
	// Set widget size to image size (fixed)
    setFixedSize(logo.width(), logo.height());
}

void CompanyLogo::paintEvent(QPaintEvent *revent) {
	QPainter painter(this);
	// Draw the image in the left top corner of the widget
	painter.drawImage(0, 0, logo);
}

void CompanyLogo::loadLogo(bool white) {
	QString logoColor;
	// Set the color as part of the file name
	if (white) {
		logoColor = "white";
	} else {
		logoColor = "orange";
	}
	// Load the image
	if (!logo.load(":/img/logo_" + logoColor + ".png")) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_LOGO_PIC, TYPE_WARNING,
			CANCEL_OPERATION);
	}
}
