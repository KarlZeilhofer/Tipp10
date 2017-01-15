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
** Implementation of the LessonPrintDialog class
** File name: lessonprintdialog.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "lessonprintdialog.h"
#include "def/defines.h"

LessonPrintDialog::LessonPrintDialog(QString *enteredName, QWidget *parent) :
	QDialog(parent) {

	setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

	setWindowTitle(tr("Lektion drucken"));
	setWindowIcon(QIcon(":/img/" + QString(ICON_FILENAME)));

	userName = enteredName;

	// Create texbox
    createLineEdit();

	// Create buttons
    createButtons();

	// Set the layout of all widgets created above
	createLayout();

	lineName->setFocus();
}

void LessonPrintDialog::createButtons() {
	//Buttons
	buttonOk = new QPushButton(this);
	buttonCancel = new QPushButton(this);

	buttonOk->setText(tr("&Drucken"));
	buttonCancel->setText(tr("&Abbrechen"));

	buttonOk->setDefault(true);
	// Widget connections
    connect(buttonOk, SIGNAL(clicked()), this, SLOT(clickOk()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

void LessonPrintDialog::createLineEdit() {

	lineName = new QLineEdit();

	labelName = new QLabel(tr("Bitte geben Sie Ihren Namen ein:"));

	labelName->setWordWrap(true);
}

void LessonPrintDialog::createLayout() {
	// Button layout horizontal
	QHBoxLayout *buttonLayoutHorizontal = new QHBoxLayout;
    buttonLayoutHorizontal->addStretch(1);
    buttonLayoutHorizontal->addWidget(buttonCancel);
    buttonLayoutHorizontal->addWidget(buttonOk);
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(1);
    mainLayout->addWidget(labelName);
    mainLayout->addSpacing(1);
    mainLayout->addWidget(lineName);
    mainLayout->addSpacing(1);
    mainLayout->addLayout(buttonLayoutHorizontal);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void LessonPrintDialog::clickOk() {

	// Return entered name
	*userName = lineName->text();
	accept();
}
