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
** Implementation of the SettingDialog class
** File name: settingsdialog.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>

#include "settingsdialog.h"
#include "sql/startsql.h"
#include "sql/connection.h"
#include "def/errordefines.h"
#include "errormessage.h"
#include "def/defines.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {

	setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

	contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setIconSize(QSize(96, 49));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMinimumWidth(120);
    contentsWidget->setMaximumWidth(120); //128
    contentsWidget->setMinimumHeight(308);
    contentsWidget->setSpacing(6);

	trainingPage = new TrainingPage;
	databasePage = new DatabasePage;
	otherPage = new OtherPage;
	languagePage = new LanguagePage;

    pagesWidget = new QStackedWidget;
    pagesWidget->addWidget(trainingPage);
    pagesWidget->addWidget(languagePage);
    pagesWidget->addWidget(databasePage);
    pagesWidget->addWidget(otherPage);

	//Buttons
	buttonCancel = new QPushButton(tr("&Abbrechen"));
	buttonSave = new QPushButton(tr("&Speichern"));
	buttonHelp = new QPushButton(tr("&Hilfe"));
	buttonSave->setDefault(true);

    createIcons();
    contentsWidget->setCurrentRow(0);

    connect(buttonSave, SIGNAL(clicked()), this, SLOT(clickSave()));
	connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(showHelp()));

	QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget, 1);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(buttonCancel);
    buttonsLayout->addSpacing(10);
    buttonsLayout->addWidget(buttonHelp);
    buttonsLayout->addWidget(buttonSave);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(6);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    readSettings();

    setWindowTitle(tr("Grundeinstellungen"));
	setWindowIcon(QIcon(":/img/" + QString(ICON_FILENAME)));

    buttonSave->setFocus();

}

void SettingsDialog::createIcons() {

    QListWidgetItem *buttonTraining = new QListWidgetItem(contentsWidget);
    buttonTraining->setIcon(QIcon(":/img/config_1.png"));
    buttonTraining->setText(tr("Schreibtraining"));
    buttonTraining->setTextAlignment(Qt::AlignHCenter);
    buttonTraining->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *buttonLanguage = new QListWidgetItem(contentsWidget);
    buttonLanguage->setIcon(QIcon(":/img/config_3.png"));
    buttonLanguage->setText(tr("Sprache"));
    buttonLanguage->setTextAlignment(Qt::AlignHCenter);
    buttonLanguage->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *buttonDatabase = new QListWidgetItem(contentsWidget);
    buttonDatabase->setIcon(QIcon(":/img/config_2.png"));
    buttonDatabase->setText(tr("Lernstatistik"));
    buttonDatabase->setTextAlignment(Qt::AlignHCenter);
    buttonDatabase->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *buttonOther = new QListWidgetItem(contentsWidget);
    buttonOther->setIcon(QIcon(":/img/config_4.png"));
    buttonOther->setText(tr("Sonstiges"));
    buttonOther->setTextAlignment(Qt::AlignHCenter);
    buttonOther->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
}

void SettingsDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous) {
    if (!current) {
        current = previous;
	}

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

void SettingsDialog::clickSave() {
	buttonSave->setText(tr("Bitte warten"));
    buttonSave->setEnabled(false);
    writeSettings();
	trainingPage->writeSettings();
	databasePage->writeSettings();
	bool requireRestartLanguage = languagePage->writeSettings();
	bool requireRestartOther = otherPage->writeSettings();
	if (requireRestartLanguage ||
		requireRestartOther) {
		QMessageBox::information(this, APP_NAME,
			tr("Einige der Einstellungen werden erst nach einem Neustart "
			"der Software wirksam.\n"));
	}
	if (createConnection()) {
		this->accept();
	}
}

void SettingsDialog::showHelp() {
	helpBrowser = new HelpBrowser("settings.html", this);
	helpBrowser->show();
}

void SettingsDialog::readSettings() {
    #if APP_PORTABLE
    QSettings settings(QCoreApplication::applicationDirPath() +
        "/portable/settings.ini", QSettings::IniFormat);
    #else
    QSettings settings;
    #endif
    settings.beginGroup("settings");
    contentsWidget->setCurrentRow(settings.value("current_settings_item", 0).toInt());
    pagesWidget->setCurrentIndex(contentsWidget->currentRow());
    settings.endGroup();
}

void SettingsDialog::writeSettings() {
    #if APP_PORTABLE
    QSettings settings(QCoreApplication::applicationDirPath() +
        "/portable/settings.ini", QSettings::IniFormat);
    #else
    QSettings settings;
    #endif
    settings.beginGroup("settings");
    settings.setValue("current_settings_item", contentsWidget->currentRow());
    settings.endGroup();
}
