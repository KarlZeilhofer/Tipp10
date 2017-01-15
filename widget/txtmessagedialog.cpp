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
** Implementation of the TxtMessageDialog class
** File name: txtmessagedialog.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QMessageBox>
#include <QCoreApplication>

#include "txtmessagedialog.h"
#include "def/errordefines.h"
#include "errormessage.h"
#include "def/defines.h"

TxtMessageDialog::TxtMessageDialog(QString message, QString title,
	QString setting, QWidget *parent) : QDialog(parent) {

	setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

	settingName = setting;

	setWindowTitle(title);
	setWindowIcon(QIcon(":/img/" + QString(ICON_FILENAME)));
    // Set the image
	messageLabel = new QLabel(message);

    // Create buttons
    createButtons();

	// Set the layout of all widgets created above
	createLayout();

    buttonOk->setFocus();
    //setFixedWidth(illustrationImage->width() + 31);

    //Max window size for the bitmap
   //setFixedSize(width, height);
}

void TxtMessageDialog::createButtons() {
	//Buttons
	buttonOk = new QPushButton(this);
	buttonOk->setText(tr("&Ok"));
	buttonOk->setDefault(true);
	// Widget connections
    connect(buttonOk, SIGNAL(clicked()), this, SLOT(clickOk()));
	showDialogCheck = new QCheckBox(tr("Diese &Information nicht mehr anzeigen"));
}

void TxtMessageDialog::createLayout() {
	// Button layout horizontal
	QHBoxLayout *layoutHorizontal = new QHBoxLayout;
    layoutHorizontal->addSpacing(1);
    layoutHorizontal->addWidget(messageLabel);
    layoutHorizontal->addSpacing(1);
	// Button layout horizontal
	QHBoxLayout *buttonLayoutHorizontal = new QHBoxLayout;
    buttonLayoutHorizontal->addStretch(1);
    if (settingName != "") {
    	buttonLayoutHorizontal->addWidget(showDialogCheck);
	}
    buttonLayoutHorizontal->addSpacing(1);
    buttonLayoutHorizontal->addWidget(buttonOk);
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layoutHorizontal);
    mainLayout->addSpacing(1);
    mainLayout->addLayout(buttonLayoutHorizontal);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void TxtMessageDialog::clickOk() {
	writeSettings();
	accept();
}

void TxtMessageDialog::writeSettings() {
	// Saves settings of the startwiget
	// (uses the default constructor of QSettings, passing
	// the application and company name see main function)
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	if (settingName != "") {
		settings.beginGroup("general");
		settings.setValue(settingName, !showDialogCheck->isChecked());
		settings.endGroup();
	}
}
