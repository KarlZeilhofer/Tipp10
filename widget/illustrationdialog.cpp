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
** Implementation of the IllustrationDialog class
** File name: illustrationdialog.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QCoreApplication>
#include <QPalette>
#include <QIcon>

#include "illustrationdialog.h"
#include "def/errordefines.h"
#include "errormessage.h"
#include "def/defines.h"

IllustrationDialog::IllustrationDialog(QWidget *parent) : QDialog(parent) {

    setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    setWindowTitle(tr("Einfuehrung"));
    setWindowIcon(QIcon(":/img/" + QString(ICON_FILENAME)));

    readSettings();

    createContent();

    createButtons();

    // Set the layout of all widgets created above
    createLayout();

    buttonStart->setFocus();

    resize(570, 400);
}

void IllustrationDialog::createContent() {

    QString content;

    content = ""
        "<div style=\"margin-top:12px;margin-left:12px;margin-bottom:12px;margin-right:12px;font-size:12px;\">"
            "<div style=\"margin-top:16px;\">"
                "<center>"
                    "&nbsp;<br><img src=\":/img/logo_orange.png\">"
                "</center>"
            "</div>"
            "<div style=\"margin-top:20px;\">"
                "<center>"
                    "<b>" + tr("Willkommen zu TIPP10") + "</b>"
                "</center>"
            "</div>"
            "<div style=\"margin-top:16px;\">" +
            tr("TIPP10 ist ein kostenloser 10-Finger-Schreibtrainer fuer Windows, Mac OS "
            "und Linux. Die Software arbeitet intelligent - Schriftzeichen, die haeufig "
            "falsch getippt werden, werden auch sofort haeufiger diktiert. "
            "So laesst sich das Zehnfingersystem schnell und effizient erlernen.") +
            "</div>"
            "<div style=\"margin-top:17px;\">"
                "<b>" + tr("Tipps zur Anwendung des Zehnfingersystems") + "</b>"
            "</div>"
            "<div style=\"margin-top:10px;\">" +
                tr("1. Die Finger nehmen zunaechst die Grundstellung ein "
                   "(sie wird auch zu Beginn jeder Lektion angezeigt). "
                   "Nach jedem Tippen einer Taste kehren die Finger in die Grundstellung "
                   "zurueck.") +
            "</div>"
            "<div style=\"margin-top:16px;\">"
                "<img src=\":/img/" + languageGui + "_illustration.png\">"
            "</div>"
            "<div style=\"margin-top:16px;\">" +
                tr("2. Nehmen Sie eine aufrechte Haltung ein und vermeiden Sie es in jedem "
                   "Fall auf die Tastatur zu sehen. Ihr Blick sollte stets auf den "
                   "Bildschirm gerichtet sein.") +
            "</div>"
            "<div style=\"margin-top:10px;\">" +
                tr("3. Legen Sie die Oberarme am Koerper an und lassen Sie die Schultern "
                   "haengen. Die Unterarme bilden einen rechten Winkel zu den Oberarmen. "
                   "Legen Sie die Handgelenke nicht ab und lassen Sie sie nicht durchhaengen.") +
            "</div>"
            "<div style=\"margin-top:10px;\">" +
                tr("4. Bleiben Sie waehrend des Schreibtrainings entspannt.") +
            "</div>"
            "<div style=\"margin-top:10px;\">" +
                tr("5. Versuchen Sie moeglichst fehlerfrei zu tippen. Es ist deutlich ineffizienter "
                   "schnell zu tippen, wenn Sie dabei viele Fehler machen.") +
            "</div>"
            "<div style=\"margin-top:10px;\">" +
                tr("6. Einmal mit dem Zehnfingersystem angefangen, sollten Sie es tunlichst "
                   "vermeiden wieder zu Ihrem \"alten System\" zurueck zu wechseln (auch "
                   "wenn es mal schnell gehen muss).") +
            "</div>"
            "<div style=\"margin-top:26px;\">" +
                tr("Bei Fragen rund um die Bedienung der Software rufen Sie bitte die "
                   "Hilfe auf oder besuchen Sie die Internetseite") +
                " <a href=\"http://www.tipp10.de\">http://www.tipp10.de</a>. " +
                tr("Dort finden Sie auch praktische Tipps und Hintergrundinformationen.") +
            "</div>"
            "<div style=\"margin-top:34px;\">"
                "<img src=\":/img/tt_logo.png\">"
            "</div>"
            "<div style=\"margin-top:10px;\">"
            "&copy; Tom Thielicke IT Solutions<br>"
            "<a href=\"http://www.thielicke.org\">http://www.thielicke.org</a><br><br>" +
            tr("Alle Rechte vorbehalten.") +
                "<br>&nbsp;"
            "</div>"
        "</div>";


    illustrationContent = new QTextBrowser();
    illustrationContent->setHtml(content);
    illustrationContent->setOpenExternalLinks(true);
}

void IllustrationDialog::createButtons() {
    //Buttons
    buttonStart = new QPushButton(this);
    buttonStart->setText(tr("TIPP10 &starten"));
    buttonStart->setDefault(true);
    // Widget connections
    connect(buttonStart, SIGNAL(clicked()), this, SLOT(clickStart()));
    showDialogCheck = new QCheckBox(tr("Dieses &Fenster nicht mehr anzeigen"));
}

void IllustrationDialog::createLayout() {
    // Button layout horizontal
    QHBoxLayout *layoutHorizontal = new QHBoxLayout;
    layoutHorizontal->addSpacing(1);
    layoutHorizontal->addWidget(illustrationContent, 1);
    layoutHorizontal->addSpacing(1);
    // Button layout horizontal
    QHBoxLayout *buttonLayoutHorizontal = new QHBoxLayout;
    buttonLayoutHorizontal->addStretch(1);
    buttonLayoutHorizontal->addWidget(showDialogCheck);
    buttonLayoutHorizontal->addSpacing(8);
    buttonLayoutHorizontal->addWidget(buttonStart);
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

void IllustrationDialog::clickStart() {
    writeSettings();
    accept();
}

void IllustrationDialog::writeSettings() {
    // Saves settings of the widget
    // (uses the default constructor of QSettings, passing
    // the application and company name see main function)
    #if APP_PORTABLE
    QSettings settings(QCoreApplication::applicationDirPath() +
        "/portable/settings.ini", QSettings::IniFormat);
    #else
    QSettings settings;
    #endif
    settings.beginGroup("general");
    settings.setValue("check_illustration", !showDialogCheck->isChecked());
    settings.endGroup();
}

void IllustrationDialog::readSettings()
{
    #if APP_PORTABLE
    QSettings settings(QCoreApplication::applicationDirPath() +
        "/portable/settings.ini", QSettings::IniFormat);
    #else
    QSettings settings;
    #endif
    settings.beginGroup("general");
    languageGui = settings.value("language_gui",
        APP_STD_LANGUAGE_GUI).toString();
    settings.endGroup();
}
