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
** Implementation of the HelpBrowser class
** File name: helpbrowser.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QSettings>
#include <QPrinter>
#include <QPrintDialog>

#include "helpbrowser.h"
#include "def/defines.h"

HelpBrowser::HelpBrowser(QString link, QWidget *parent) : QDialog(parent) {

	setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

	setWindowTitle(tr("Hilfe"));
	setWindowIcon(QIcon(":/img/" + QString(ICON_FILENAME)));
	//setModal(false);
	
	readSettings();
	
	// Create buttons
    createButtons();

	textBrowser = new QTextBrowser(this);

    textBrowser->setOpenExternalLinks(true);
	
    textBrowser->setSource(QString("file:///") + 
    	QCoreApplication::applicationDirPath() + 
    	QString("/help/") + language + QString("/index.html"));
    	
    if (link != "") {
    
		textBrowser->setSource(QString("file:///") + 
			QCoreApplication::applicationDirPath() + 
			QString("/help/") + language + QString("/content/") + link);
			
	}

	// Set the layout of all widgets created above
	createLayout();

	// Widget connections
    connect(buttonClose, SIGNAL(clicked()), this, SLOT(clickClose()));
	connect(buttonBack, SIGNAL(clicked()), textBrowser, SLOT(backward()));
	connect(buttonHome, SIGNAL(clicked()), textBrowser, SLOT(home()));
    connect(buttonPrint, SIGNAL(clicked()), this, SLOT(clickPrint()));
	connect(textBrowser, SIGNAL(sourceChanged(QUrl)), this, SLOT(changePage(QUrl)));
	connect(textBrowser, SIGNAL(backwardAvailable(bool)), buttonBack,
		SLOT(setEnabled(bool)));

	setWindowTitle(tr("Hilfe"));
	setWindowIcon(QIcon(":/img/" + QString(ICON_FILENAME)));

    buttonClose->setFocus();

    resize(790, 570);
}

void HelpBrowser::createButtons() {
	//Buttons
	buttonBack = new QPushButton(QIcon(":/img/help_arrow_left.png"), tr(" Zurueck"));
	buttonBack->setEnabled(false);
	buttonHome = new QPushButton(QIcon(":/img/help_home.png"), tr(" Inhaltsverzeichnis"));
	buttonClose = new QPushButton(tr("&Schliessen"));
	buttonClose->setDefault(true);
    buttonPrint = new QPushButton(QIcon(":/img/help_print.png"), tr(" Seite &drucken"));
}

void HelpBrowser::createLayout() {
	// Button layout horizontal
	QHBoxLayout *buttonLayoutTop = new QHBoxLayout;
    buttonLayoutTop->addWidget(buttonBack);
    buttonLayoutTop->addWidget(buttonHome);
    buttonLayoutTop->addStretch(1);
    buttonLayoutTop->addWidget(buttonPrint);
	// Center layout horizontal
	QHBoxLayout *layoutHorizontal = new QHBoxLayout;
    layoutHorizontal->addSpacing(1);
    layoutHorizontal->addWidget(textBrowser);
    layoutHorizontal->addSpacing(1);
	// Button layout horizontal
	QHBoxLayout *buttonLayoutBottom = new QHBoxLayout;
    buttonLayoutBottom->addStretch(1);
    buttonLayoutBottom->addWidget(buttonClose);
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(buttonLayoutTop);
    mainLayout->addLayout(layoutHorizontal);
    mainLayout->addSpacing(1);
    mainLayout->addLayout(buttonLayoutBottom);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void HelpBrowser::clickClose() {
	accept();
}

void HelpBrowser::clickPrint() {

    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Seite drucken"));

    if (dialog->exec() != QDialog::Accepted)
        return;

    textBrowser->print(&printer);
}

void HelpBrowser::changePage(QUrl url) {
	//this->setWindowTitle(url.toString());
}

void HelpBrowser::readSettings() {
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	settings.beginGroup("general");
	language = settings.value("language_gui",
		 APP_STD_LANGUAGE_GUI).toString();
	settings.endGroup();
}
