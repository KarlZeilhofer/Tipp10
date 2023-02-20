/*
Copyright (c) 2006-2011, Tom Thielicke IT Solutions

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
** File name: main.cpp
**
****************************************************************/

#include <QApplication>
//#include <QPlastiqueStyle>
#include <QStyleFactory>
#include <QString>
#include <QSettings>
#include <QCoreApplication>
#include <QTranslator>

#include "def/defines.h"
#include "sql/connection.h"
#include "widget/mainwindow.h"
#include "widget/licensedialog.h"
#include "widget/illustrationdialog.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("plastiquestyle"));

    // Set application name and domain
    // (this saves having to repeat the information
    // each time a QSettings object is created)
	app.setOrganizationName(APP_ORGANIZATION);
 	app.setOrganizationDomain(APP_URL);
    app.setApplicationName(APP_NAME_INTERN);

	// Settings to get and set general settings
	#if APP_PORTABLE
	/*QSettings settings(QSettings::IniFormat, QSettings::UserScope,
		"portable", "settings");
    settings.setPath(QSettings::IniFormat, QSettings::UserScope,
    	QCoreApplication::applicationDirPath());*/
    QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
	#else
	QSettings settings;
	#endif

	// Read/write language, license key and show illustration flag
	settings.beginGroup("general");
	QString languageGui = settings.value("language_gui",
		QLocale::system().name()).toString();

	QStringList languagesGui =
		QString(APP_EXISTING_LANGUAGES_GUI).split(";", QString::SkipEmptyParts);

	QString checkDelims = "_";
	bool checkSuccessful = true;

	for (;;) {

		if (languagesGui.contains(languageGui)) {
			break;
		}

		int rightmost = 0;
		for (int i = 0; i < (int)checkDelims.length(); i++) {
			int k = languageGui.lastIndexOf(checkDelims[i]);
			if (k > rightmost)
				rightmost = k;
		}

		// no truncations? fail
		if (rightmost == 0)  {
			checkSuccessful = false;
			break;
		}

		languageGui.truncate(rightmost);
    }
    if (!checkSuccessful) {
		languageGui = APP_STD_LANGUAGE_GUI;
	}

	QString languageLayout = settings.value("language_layout",
		"").toString();

	QString languageLesson = settings.value("language_lesson",
		"").toString();

	QString licenseKey = settings.value("licensekey", "").toString();
	bool showIllustration = settings.value("check_illustration", true).toBool();
	bool useNativeStyle = settings.value("check_native_style", false).toBool();
    settings.endGroup(); // general

	// Convert old keyboard layout settings to new (since v2.1.0)
	if (languageLayout == "") {
		if (languageGui != "de") {

			languageLayout = "us_qwerty";
			if (APP_MAC) {
				languageLayout.append("_mac");
			} else {
				languageLayout.append("_win");
			}
			// Enable when English lessons available!!!!!!!!
            languageLesson = "en_us_qwerty";

		} else {
			// Check user's keyboard layout (win or mac)
			settings.beginGroup("settings");

			//! Used operation system ("win" or "mac").
			QString opSystem;

			//! Used keyboard layout ("de", "ch", "neo", etc.).
			QString opLayout;

			//! Used swiss layout ("sg" or "sf").
			QString chLayout;

			//! Used neo layout ("1.0" or "2.0").
			QString neoLayout;

			opSystem = "win";
			opLayout = "";
			chLayout = "";
			neoLayout = "";
			switch (settings.value("combo_layout_ch", 0).toInt()) {
				case 0:
					chLayout = "_sg";
					break;
				case 1:
					chLayout = "_sf";
					break;
			}
			switch (settings.value("combo_layout",
				KEYBOARD_LAYOUT_STANDARD).toInt()) {
				case 0:
					opLayout = "de_qwertz";
					break;
				case 1:
					opLayout = "ch_qwertz";
					break;
				case 2:
					opLayout = "de_neo";
					break;
				case 3:
					opLayout = "de_dvorak_v1";
					break;
				case 4:
					opLayout = "de_ristome";
					break;
				case 5:
					opLayout = "us_qwerty";
					break;
			}
			switch (settings.value("combo_layout_neo",
				0).toInt()) {
				case 0:
					neoLayout = "_v1";
					break;
				case 1:
					neoLayout = "_v2";
					break;
			}
			switch (settings.value("combo_layout_os",
				KEYBOARD_LAYOUT_MAC).toInt()) {
				case 0:
					opSystem = "win";
					break;
				case 1:
					opSystem = "mac";
					break;
			}

			languageLayout = opLayout;

			if (opLayout == "ch_qwertz") {
				languageLayout.append(chLayout);
			}

			if (opLayout == "de_neo") {
				languageLayout.append(neoLayout);
				languageLesson = "de_de_qwertz"; //"de_de_neo_v2";
			} else {
				languageLesson = "de_de_qwertz";
			}

			languageLayout.append("_" + opSystem);
			settings.endGroup();

		}
	}

	settings.beginGroup("general");
	settings.setValue("language_gui", languageGui);
	settings.setValue("language_layout", languageLayout);
	settings.setValue("language_lesson", languageLesson);
	settings.endGroup();

    // Set windows style
	if (!useNativeStyle) {
	    app.setStyle("plastique");
    }

	// Translation
	// Common qt widgets
	QTranslator translatorQt;
	translatorQt.load("qt_" + languageGui, ":/languages/");
	app.installTranslator(&translatorQt);

	// Content (own translation files)
	QTranslator translatorContent;
	translatorContent.load("tipp10_" + languageGui, ":/languages/");
	app.installTranslator(&translatorContent);

	// Set path to the db if arg parameter is set
	if (argc > 1) {
		settings.beginGroup("database");
		settings.setValue("pathpro", QString::fromLatin1(argv[1]) + "/" +
			APP_USER_DB);
		settings.endGroup();
	}

	// Create database connection
    if (!createConnection()) {
		// Cannot find or open database
		// -> exit program
		return 1;
	}


	// Show illustration widget at the beginning if not disabled by the user
    if (showIllustration) {
        IllustrationDialog illustrationDialog(0);
		illustrationDialog.exec();
	}

	// Create main window object
    MainWindow window;
    window.show();

    // Start the event loop
    return app.exec();
}
