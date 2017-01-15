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
** Implementation of the LicenseDialog class
** File name: licensedialog.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSettings>
#include <QCoreApplication>

#include "licensedialog.h"
#include "def/defines.h"

LicenseDialog::LicenseDialog(QWidget *parent) : QDialog(parent) {

	setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

	setWindowTitle(tr("Lizenznummer"));
	setWindowIcon(QIcon(":/img/" + QString(ICON_FILENAME)));

	// Create texbox
    createLineEdit();

	// Create buttons
    createButtons();

	// Set the layout of all widgets created above
	createLayout();

	lineLicensing->setFocus();
}

void LicenseDialog::createButtons() {
	//Buttons
	buttonOk = new QPushButton(this);
	buttonDemo = new QPushButton(this);

	buttonOk->setText(tr("&Ok"));
	buttonDemo->setText(tr("&Demo starten"));
	buttonDemo->setToolTip(tr("Im Demo-Modus koennen pro Lektion nur\n"
		"10 Schriftzeichen eingegeben werden"));

	buttonOk->setDefault(true);
	// Widget connections
    connect(buttonOk, SIGNAL(clicked()), this, SLOT(clickOk()));
    connect(buttonDemo, SIGNAL(clicked()), this, SLOT(clickDemo()));
}

void LicenseDialog::createLineEdit() {

	lineLicensing = new QLineEdit();
	lineLicensing->setInputMask(">NNNNNNNNNNNNNN");

	labelLicensing = new QLabel(tr("Bitte geben Sie Ihre Lizenznummer "
		"(ohne Leerzeichen) ein, "
		"die Sie im Arbeitsbuch (Schulbuch) auf Seite 3 finden:"));

	labelLicensing->setWordWrap(true);
}

void LicenseDialog::createLayout() {
	// Button layout horizontal
	QHBoxLayout *buttonLayoutHorizontal = new QHBoxLayout;
    buttonLayoutHorizontal->addStretch(1);
    buttonLayoutHorizontal->addWidget(buttonDemo);
    buttonLayoutHorizontal->addWidget(buttonOk);
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(1);
    mainLayout->addWidget(labelLicensing);
    mainLayout->addSpacing(1);
    mainLayout->addWidget(lineLicensing);
    mainLayout->addSpacing(1);
    mainLayout->addLayout(buttonLayoutHorizontal);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void LicenseDialog::clickOk() {

	// Check license key
	if (!checkLicenseKey(lineLicensing->text())) {

		// License key is wrong

		// Message to the user
		QMessageBox::information(0, APP_NAME,
			tr("Die eingegebene Lizenznummer ist leider nicht "
			"korrekt.\nBitte ueberpruefen Sie die Schreibweise."));

		lineLicensing->setFocus();

	} else {

		// License key is ok
		writeSettings();
		accept();
	}
}

void LicenseDialog::clickDemo() {
	accept();
}

bool LicenseDialog::checkLicenseKey(QString licenseKey) {
	if (licenseKey.size() == 14 &&
		licenseKey[0].isLetter() &&
		licenseKey[1].isLetter() &&
		(licenseKey.mid(2, 2) == "39" ||
		licenseKey.mid(2, 2) == "41" ||
		licenseKey.mid(2, 2) == "43" ||
		licenseKey.mid(2, 2) == "49" ||
		licenseKey.mid(2, 2) == "99") &&
		licenseKey[4].isDigit() &&
		licenseKey[5].isDigit() &&
		licenseKey[6].isDigit() &&
		licenseKey[7].isLetter() &&
		licenseKey[8].isDigit() &&
		licenseKey[9].isDigit() &&
		licenseKey[10].isDigit() &&
		licenseKey[11].isDigit() &&
		licenseKey[12].isLetter() &&
		licenseKey[13].isLetter()) {

		return true;
	}
	return false;
}

void LicenseDialog::writeSettings() {
	// Saves settings of the startwiget
	// (uses the default constructor of QSettings, passing
	// the application and company name see main function)
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif

	settings.beginGroup("general");
	settings.setValue("licensekey", lineLicensing->text());
	settings.endGroup();
}
