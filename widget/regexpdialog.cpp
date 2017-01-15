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
** Implementation of the RegExpDialog class
** File name: regexpdialog.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QMessageBox>
#include <QString>
#include <QFont>
#include <QSettings>
#include <QCoreApplication>

#include "regexpdialog.h"
#include "sql/trainingsql.h"
#include "def/errordefines.h"
#include "errormessage.h"
#include "def/defines.h"

RegExpDialog::RegExpDialog(QString layout, QWidget *parent) : QDialog(parent) {

	setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    currentLayout = layout;

    // Create buttons
    createButtons();

    // Create controls
    createControls();

	// Set the layout of all widgets created above
	createLayout();

    readSettings();

	// Widget connections
    connect(buttonSave, SIGNAL(clicked()), this, SLOT(clickSave()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(buttonDefault, SIGNAL(clicked()), this, SLOT(getDefault()));

    setWindowIcon(QIcon(":/img/" + QString(ICON_FILENAME)));

    setMinimumSize(420, 300);

    buttonSave->setFocus();
}

void RegExpDialog::createButtons() {
	// Buttons
	buttonCancel = new QPushButton(tr("&Abbrechen"));
	buttonSave = new QPushButton(tr("&Speichern"));
    buttonDefault = new QPushButton(tr("S&tandard"));
	buttonSave->setDefault(true);
}

void RegExpDialog::createControls()
{
    QFont h2;
    #if APP_MAC
    h2.setPointSize(11);
    #else
    h2.setPointSize(7);
    #endif
    // Labels
    labelRegExp = new QLabel(tr("Zeichenbegrenzung"));
    labelRegExpHelp = new QLabel(tr("Es sollen Schriftzeichen vermieden werden, die das "
                                    "aktuelle Tastaturlayout nicht unterstuetzt. "
                                    "Daher wird auf alle Uebungstexte vor dem Training ein "
                                    "Filter in Form eines regulaeren Ausdrucks "
                                    "angewendet. Sie sollten Aenderungen nur "
                                    "durchfuehren, wenn Sie sich mit regulaeren "
                                    "Ausdruecken auskennen."));
    labelRegExpHelp->setFont(h2);
    labelRegExpHelp->setWordWrap(true);
    labelReplace = new QLabel(tr("Ersetzungsfilter"));
    labelReplaceHelp = new QLabel(tr("Durch das Filtern auf zugelassene Schriftzeichen "
                                     "kann sinnloser Text entstehen (z.B. durch "
                                     "das Entfernen von Umlauten). Sie koennen hier "
                                     "Ersetzungen definieren, die vor der Zeichenbegrenzung auf "
                                     "den Text angewendet werden. Verwenden Sie dazu "
                                     "eine Form wie in nachfolgendem Beispiel, das alle "
                                     "deutschen Umlaute und Sz ersetzt:") +
                                  "\n" + tr("ae=ae,oe=oe,ue=ue,Ae=Ae,Oe=Oe,Ue=Ue,ss=ss"));
    labelReplaceHelp->setFont(h2);
    labelReplaceHelp->setWordWrap(true);
	// Lines (text input)
    lineRegExp = new QLineEdit();
    lineReplace = new QLineEdit();

    setWindowTitle(tr("Filter fuer das Tastaturlayout"));

}

void RegExpDialog::createLayout() {
	// Button layout horizontal
	QHBoxLayout *buttonLayoutHorizontal = new QHBoxLayout;
    buttonLayoutHorizontal->addWidget(buttonDefault);
    buttonLayoutHorizontal->addStretch(1);
    buttonLayoutHorizontal->addWidget(buttonCancel);
    buttonLayoutHorizontal->addSpacing(10);
    buttonLayoutHorizontal->addWidget(buttonSave);
    // Group layout vertical
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(labelRegExp);
    mainLayout->addWidget(lineRegExp);
    mainLayout->addWidget(labelRegExpHelp);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(labelReplace);
    mainLayout->addWidget(lineReplace);
    mainLayout->addWidget(labelReplaceHelp);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(buttonLayoutHorizontal);
    mainLayout->setMargin(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void RegExpDialog::clickSave() {
    writeSettings();
	this->accept();
}

void RegExpDialog::getDefault() {
    TrainingSql *trainingSql = new TrainingSql();
    lineRegExp->setText(trainingSql->getKeyboardLayoutRegexpRoutine(currentLayout));
    lineReplace->setText(trainingSql->getKeyboardLayoutReplaceRoutine(currentLayout));
}

void RegExpDialog::readSettings()
{
    #if APP_PORTABLE
    QSettings settings(QCoreApplication::applicationDirPath() +
        "/portable/settings.ini", QSettings::IniFormat);
    #else
    QSettings settings;
    #endif
    settings.beginGroup("general");
    QString regexp = settings.value("layout_regexp", "NULL").toString();
    QString replace = settings.value("layout_replace", "NULL").toString();
    settings.endGroup();
    TrainingSql *trainingSql = new TrainingSql();

    if (regexp == "NULL") {
        regexp = trainingSql->getKeyboardLayoutRegexpRoutine(currentLayout);
    }
    if (replace == "NULL") {
        replace = trainingSql->getKeyboardLayoutReplaceRoutine(currentLayout);
    }
    lineRegExp->setText(regexp);
    lineReplace->setText(replace);
}

void RegExpDialog::writeSettings()
{
    #if APP_PORTABLE
    QSettings settings(QCoreApplication::applicationDirPath() +
        "/portable/settings.ini", QSettings::IniFormat);
    #else
    QSettings settings;
    #endif
    settings.beginGroup("general");
    settings.setValue("layout_replace", lineReplace->text());
    settings.setValue("layout_regexp", lineRegExp->text());
    settings.endGroup();
}
