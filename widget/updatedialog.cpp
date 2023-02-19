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
** Implementation of the UpdateDialog class
** File name: updatedialog.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSettings>
#include <QTextStream>
#include <QString>
#include <QtNetwork>
#include <QSizePolicy>
#include <QApplication>
#include <QCoreApplication>
#include <QDesktopServices>

#include "updatedialog.h"
#include "def/defines.h"
#include "def/errordefines.h"
#include "sql/startsql.h"
#include "errormessage.h"

#ifdef ONLINE

// Constructor
UpdateDialog::UpdateDialog(QWidget *parent) : QDialog(parent) {

	setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

	// Init flag default value: no new version available
    newVersion = false;

    // QHttp object provides interface to HTTP
    http = new QHttp(this);


	// Create dialog content
	createProgressinfo();
	createButtons();
	createLayout();
	createConnections();

	// Proxy settings
	readSettings();

	// Dialog defaults
    setWindowTitle(tr("Aktualisierung"));
	setWindowIcon(QIcon(":/img/" + QString(ICON_FILENAME)));
    buttonUpdate->setFocus();
}

void UpdateDialog::showHelp() {
	helpBrowser = new HelpBrowser("update.html", this);
	helpBrowser->show();
}

void UpdateDialog::createProgressinfo() {
    labelStatus = new QLabel(tr("Hier koennen Sie eine Aktualisierung "
    	"der Lektionen durchfuehren.\nFuer die Aktualisierung ist eine "
    	"Internetverbindung erforderlich."));
    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
}

void UpdateDialog::createButtons() {
	//Buttons
    buttonClose = new QPushButton(tr("Schliessen"));
    buttonUpdate = new QPushButton(tr("Aktualisierung starten"));
	buttonHelp = new QPushButton(tr("&Hilfe"));
    checkProxy = new QCheckBox(tr("Ueber einen Proxyserver verbinden"));
    txtProxyServer = new QLineEdit();
    txtProxyServer->setShown(false);
    txtProxyPort = new QLineEdit();
    txtProxyPort->setShown(false);
    labelProxyServer = new QLabel(tr("Server:"));
    labelProxyServer->setShown(false);
    labelProxyPort = new QLabel(tr("Port:"));
    labelProxyPort->setShown(false);
    buttonUpdate->setDefault(true);
}

void UpdateDialog::createLayout() {
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(buttonClose);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(buttonHelp);
    buttonLayout->addWidget(buttonUpdate);

    QHBoxLayout *proxyLayout = new QHBoxLayout;
    proxyLayout->addWidget(labelProxyServer);
    proxyLayout->addWidget(txtProxyServer);
    proxyLayout->addWidget(labelProxyPort);
    proxyLayout->addWidget(txtProxyPort);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(labelStatus);
    mainLayout->addWidget(progressBar);
    mainLayout->addWidget(checkProxy);
    mainLayout->addLayout(proxyLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);
}

void UpdateDialog::createConnections() {
    connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
    	this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
    connect(http, SIGNAL(done(bool)), this, SLOT(httpDownloadFinished(bool)));
    connect(http, SIGNAL(dataReadProgress(int, int)), this,
    	SLOT(updateDataReadProgress(int, int)));
    connect(buttonUpdate, SIGNAL(clicked()), this,
    	SLOT(downloadVersionFile()));
    connect(buttonClose, SIGNAL(clicked()), this, SLOT(writeSettings()));
    connect(buttonClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(checkProxy, SIGNAL(toggled(bool)), txtProxyServer, SLOT(setShown(bool)));
    connect(checkProxy, SIGNAL(toggled(bool)), txtProxyPort, SLOT(setShown(bool)));
    connect(checkProxy, SIGNAL(toggled(bool)), labelProxyServer, SLOT(setShown(bool)));
    connect(checkProxy, SIGNAL(toggled(bool)), labelProxyPort, SLOT(setShown(bool)));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(showHelp()));
}

void UpdateDialog::downloadVersionFile() {
	// Save proxy settings
	writeSettings();

	labelStatus->setText(
		tr("Versionsinformationen herunterladen..."));
	labelStatus->update();
	progressBar->setVisible(true);
	qApp->processEvents();

    tempVersionFile = new QTemporaryFile;
    if (!tempVersionFile->open()) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_TEMP_FILE_CREATION, TYPE_CRITICAL,
			CANCEL_UPDATE);
        delete tempVersionFile;
        progressBar->setVisible(false);
        buttonUpdate->setEnabled(true);
        return;
    }

    buttonUpdate->setEnabled(false);

    checkProxy->setVisible(false);
    labelProxyServer->setVisible(false);
    txtProxyServer->setVisible(false);
    labelProxyPort->setVisible(false);
    txtProxyPort->setVisible(false);

    http->setHost(UPDATE_URL);

    // Proxy server?
    if (checkProxy->isChecked()) {
    	http->setProxy(txtProxyServer->text(), txtProxyPort->text().toInt());
	}

    http->get(UPDATE_URL_VERSION, tempVersionFile);
}

void UpdateDialog::downloadSqlFile() {
	http->abort();
	progressBar->setVisible(true);
	labelStatus->setText(tr("SQL-Datei herunterladen..."));
	labelStatus->update();
	qApp->processEvents();
	tempSqlFile = new QTemporaryFile;
    if (!tempSqlFile->open()) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_TEMP_FILE_CREATION, TYPE_CRITICAL,
			CANCEL_UPDATE);
        delete tempSqlFile;
		close();
        return;
    }
    buttonUpdate->setEnabled(false);

	http->setHost(UPDATE_URL);

    // Proxy server?
    if (checkProxy->isChecked()) {
    	http->setProxy(txtProxyServer->text(), txtProxyPort->text().toInt());
	}

	http->get(UPDATE_URL_SQL, tempSqlFile);
}

void UpdateDialog::updateDataReadProgress(int bytesRead, int totalBytes) {
    progressBar->setMaximum(totalBytes);
    progressBar->setValue(bytesRead);
}

void UpdateDialog::readResponseHeader(const QHttpResponseHeader
		&responseHeader) {
    if (responseHeader.statusCode() != 200) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_UPDATE_EXECUTION, TYPE_CRITICAL,
			CANCEL_UPDATE);
        http->abort();
        return;
    }
}

void UpdateDialog::httpDownloadFinished(bool error) {
	// Download finished
	if (error) {
		// Error message + additional error information
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_UPDATE_EXECUTION, TYPE_CRITICAL,
			CANCEL_UPDATE, http->errorString());
		close();
		return;
    }

	if (!newVersion) {
		// First check the database version
		labelStatus->setText(tr("Version ueberpruefen..."));
		labelStatus->update();
		qApp->processEvents();
		if (checkVersionFile()) {
			// DB Version is new
			// -> download sql file
			delete tempVersionFile;
			newVersion = true;
			downloadSqlFile();
		}
	} else {
		// Execute sql file and analyze current text in DB
		labelStatus->setText(APP_NAME + tr(" Datenbank "
			"aktualisieren..."));
		labelStatus->update();
		qApp->processEvents();
		if (executeSqlFile()) {
			StartSql *lessonSql = new StartSql();
			if (lessonSql->analyzeLessons("lesson")) {
				if (lessonSql->analyzeLessons("open")) {
					labelStatus->setText(APP_NAME + tr(" wurde "
						"erfolgreich aktualisiert!"));
					buttonClose->setFocus();
				} else {
					close();
				}
			} else {
				close();
			}
		} else {
			close();
		}
	}
}

bool UpdateDialog::checkVersionFile() {
	// Go to the beginning of the version file
    tempVersionFile->seek(0);

	QSqlQuery query;
	QTextStream in(tempVersionFile);
	// Read only the first line (server DB version)
	QString updateVersion = in.readLine();
	if (updateVersion.isNull()) {
		// Can't read line
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_ONLINE_VERSION_READABLE, TYPE_CRITICAL,
			CANCEL_UPDATE);
        close();
        return false;
	} else {
		// Check DB version of software
		if (!query.exec("SELECT * FROM db_version ORDER BY version DESC;")) {
			// Error message
			ErrorMessage *errorMessage = new ErrorMessage(this);
			errorMessage->showMessage(ERR_DB_VERSION_READABLE,
				TYPE_CRITICAL, CANCEL_UPDATE);
			close();
			return false;
		} else {
			if (!query.first()) {
				// Error message
				ErrorMessage *errorMessage = new ErrorMessage(this);
				errorMessage->showMessage(ERR_DB_VERSION_READABLE,
					TYPE_CRITICAL, CANCEL_UPDATE);
				close();
				return false;
			} else {
				// Server DB version is 0
				// -> software is too old to update
				QString softwareVersion = query.value(0).toString();
				if (updateVersion.trimmed() == "0") {
					if (QMessageBox::information(this, APP_NAME,
						QString(tr("Ihre Version der Software ist "
						"veraltet und nicht mehr aktualisierungsfaehig.\nDie "
						"neue Version erhalten Sie im Internet unter %1\n\n"
						"Moechten Sie die neue Version jetzt herunterladen?"
						)).arg(APP_URL), QMessageBox::Yes | QMessageBox::No) ==
						QMessageBox::Yes) {

						QDesktopServices::openUrl(QString(APP_URL));
					}
					close();
					return false;
				}
				// Check whether ther is a new DB version on the server
				if (softwareVersion.trimmed() != updateVersion.trimmed()) {
					labelStatus->setText(QString(tr("Es stehen "
						"Updates fuer %1 zur Verfuegung...")).arg(APP_NAME));
					return true;
				}
			}
		}
	}
	labelStatus->setText(APP_NAME + tr(" befindet sich bereits auf "
		"dem aktuellsten Stand.\nEs stehen derzeit keine Aktualisierungen zur "
		"Verfuegung."));
	return false;

}

bool UpdateDialog::executeSqlFile() {
    QSqlQuery query;
	QString line = "";
	int bytesRead = 0;
	int totalBytes = tempSqlFile->size();

	// Go to the beginning of the version file
    tempSqlFile->seek(0);

	QTextStream in(tempSqlFile);

	// Execute all sql command of the downloaded file
	while (!in.atEnd()) {
		line = in.readLine();
		line = line.trimmed();
		bytesRead += line.size();
		// Exclude comments and empty lines
		if (line != "" && !line.startsWith("//", Qt::CaseSensitive)) {
			// Without error handling, because DROP-Statements are allowed to
			// be invalid (there exist also a IF EXISTS statement in the SQLite
			// library which suppresses an error, but it didn't work when I try it)
			if (!query.exec(line) && !line.startsWith("drop", Qt::CaseInsensitive)) {
				// Error message + failed sql string
				ErrorMessage *errorMessage = new ErrorMessage(this);
				errorMessage->showMessage(ERR_UPDATE_SQL_EXECUTION, TYPE_CRITICAL,
					CANCEL_UPDATE, line);
				return false;
			}
		}
		updateDataReadProgress(bytesRead, totalBytes);
	}
	delete tempSqlFile;
	updateDataReadProgress(totalBytes, totalBytes);
	return true;
}

void UpdateDialog::readSettings() {
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	settings.beginGroup("proxy");
	checkProxy->setChecked(settings.value("check_proxy", false).toBool());
	txtProxyServer->setText(settings.value("proxy_server", "").toString());
	txtProxyPort->setText(settings.value("proxy_port", "").toString());
	settings.endGroup();
	// Show proxy settings if proxy is checked
    if (checkProxy->isChecked()) {
	    labelProxyServer->setVisible(true);
	    txtProxyServer->setVisible(true);
	    labelProxyPort->setVisible(true);
   		txtProxyPort->setVisible(true);
	}
}

void UpdateDialog::writeSettings() {
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	settings.beginGroup("proxy");
	settings.setValue("check_proxy", checkProxy->isChecked());
	settings.setValue("proxy_server", txtProxyServer->text());
	settings.setValue("proxy_port", txtProxyPort->text());
	settings.endGroup();
}

#endif
