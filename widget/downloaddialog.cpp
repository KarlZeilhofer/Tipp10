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
** Implementation of the DownloadDialog class
** File name: downloaddialog.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>
#include <QUrl>
#include <QtNetwork>
#include <QHash>
#include <QHashIterator>
#include <QSizePolicy>
#include <QApplication>
#include <QCoreApplication>

#include "downloaddialog.h"
#include "def/defines.h"
#include "def/errordefines.h"
#include "errormessage.h"

DownloadDialog::DownloadDialog(QStringList *data, QWidget *parent) :
	QDialog(parent) {

	setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    // QHttp object provides interface to HTTP
    http = new QHttp(this);

	setWindowTitle(tr("Lektion downloaden"));
	setWindowIcon(QIcon(":/img/" + QString(ICON_FILENAME)));

	lessonData = data;

	createProgressinfo();

	// Create dialog content
    createControls();
	createLayout();
	createConnections();

	readSettings();

	lineImportPath->setFocus();

    setMinimumSize(APP_WIDTH_STANDARD, APP_HEIGHT_STANDARD);
}

void DownloadDialog::showHelp() {
	helpBrowser = new HelpBrowser("lessons.html#ownlesson", this);
	helpBrowser->show();
}

void DownloadDialog::createProgressinfo() {

	labelImportPath = new QLabel(tr("Bitte geben Sie die Internetadresse zu einer "
		"Textdatei ein:"));
	labelImportPath->setWordWrap(true);

    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
}

void DownloadDialog::createControls() {

	lineImportPath = new QLineEdit();
	//Buttons
	buttonOk = new QPushButton(tr("&Download starten"));
	buttonCancel = new QPushButton(tr("&Abbrechen"));
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

	buttonOk->setDefault(true);
}

void DownloadDialog::createLayout() {
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(buttonCancel);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(buttonHelp);
    buttonLayout->addWidget(buttonOk);

    QHBoxLayout *proxyLayout = new QHBoxLayout;
    proxyLayout->addWidget(labelProxyServer);
    proxyLayout->addWidget(txtProxyServer);
    proxyLayout->addWidget(labelProxyPort);
    proxyLayout->addWidget(txtProxyPort);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(labelImportPath);
    mainLayout->addWidget(lineImportPath);
    mainLayout->addWidget(progressBar);
    mainLayout->addWidget(checkProxy);
    mainLayout->addLayout(proxyLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);
}

void DownloadDialog::createConnections() {
    connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
    	this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
    connect(http, SIGNAL(done(bool)), this, SLOT(httpDownloadFinished(bool)));
    connect(http, SIGNAL(dataReadProgress(int, int)), this,
    	SLOT(updateDataReadProgress(int, int)));
    connect(buttonOk, SIGNAL(clicked()), this, SLOT(downloadTxtFile()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(writeSettings()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(checkProxy, SIGNAL(toggled(bool)), txtProxyServer, SLOT(setShown(bool)));
    connect(checkProxy, SIGNAL(toggled(bool)), txtProxyPort, SLOT(setShown(bool)));
    connect(checkProxy, SIGNAL(toggled(bool)), labelProxyServer, SLOT(setShown(bool)));
    connect(checkProxy, SIGNAL(toggled(bool)), labelProxyPort, SLOT(setShown(bool)));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(showHelp()));
}

bool DownloadDialog::checkTxtFile() {

	lessonData->clear();

	lessonData->append(lineImportPath->text());

	int bytesRead = 0;
	int totalBytes = tempTxtFile->size();
	QString line;

	// Go to the beginning of the text file
    tempTxtFile->seek(0);

	QTextStream in(tempTxtFile);

	// Read all lines of the downloaded file
	while (!in.atEnd()) {

		line = in.readLine();
		lessonData->append(line);
		bytesRead += line.size();

		updateDataReadProgress(bytesRead, totalBytes);
	}
	delete tempTxtFile;
	updateDataReadProgress(totalBytes, totalBytes);

	if (lessonData->size() < 2) {
		return false;
	}

	return true;
}

void DownloadDialog::downloadTxtFile() {

	writeSettings();

	downloadCanceled = false;

	showProgressControls(true);

	labelImportPath->setText(tr("Textdatei herunterladen..."));
	labelImportPath->update();

	qApp->processEvents();

	tempTxtFile = new QTemporaryFile;
    if (!tempTxtFile->open()) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_TEMP_FILE_CREATION, TYPE_WARNING,
			CANCEL_OPERATION);

        delete tempTxtFile;
        showProgressControls(false);
		close();
        return;
    }

	QString userUrl = lineImportPath->text();

	// Check if user forgot the "http"
	if (userUrl.left(4) != "http") {
		userUrl = "http://" + userUrl;
	}
    QUrl url(userUrl);

    if (url.path() == "") {
		url.setPath("/");
	}

	http->setHost(url.host());

    // Proxy server?
    if (checkProxy->isChecked()) {
    	http->setProxy(txtProxyServer->text(), txtProxyPort->text().toInt());
	}

	http->get(url.path(), tempTxtFile);
}

void DownloadDialog::updateDataReadProgress(int bytesRead, int totalBytes) {

    progressBar->setMaximum(totalBytes);
    progressBar->setValue(bytesRead);
}

void DownloadDialog::showProgressControls(bool show) {

    if (show) {

		lineImportPath->setVisible(false);
		progressBar->setVisible(true);
		buttonOk->setEnabled(false);
	} else {

        lineImportPath->setVisible(true);
        progressBar->setVisible(false);
        buttonOk->setEnabled(true);
	}
}

void DownloadDialog::readResponseHeader(const QHttpResponseHeader
		&responseHeader) {

    if (responseHeader.statusCode() != 200) {
		// Error message
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_USER_DOWNLOAD_EXECUTION, TYPE_WARNING,
			CANCEL_OPERATION);

        downloadCanceled = true;

        return;
    }
}

void DownloadDialog::httpDownloadFinished(bool error) {
	// Download finished
	if (downloadCanceled) {

		showProgressControls(false);
        http->abort();
		return;
	}
	if (error) {
		// Error message + additional error information
		ErrorMessage *errorMessage = new ErrorMessage(this);
		errorMessage->showMessage(ERR_USER_DOWNLOAD_EXECUTION, TYPE_WARNING,
			CANCEL_OPERATION, http->errorString());
		showProgressControls(false);
        http->abort();
		return;
    }

	// Execute sql file and analyze current text in DB
	labelImportPath->setText(tr("Lektion "
		"ueberpruefen..."));
	labelImportPath->update();
	qApp->processEvents();
	if (checkTxtFile()) {
		this->accept();
	} else {
		close();
	}
}

void DownloadDialog::readSettings() {
	// Saves settings of the startwiget
	// (uses the default constructor of QSettings, passing
	// the application and company name see main function)
	#if APP_PORTABLE
    QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif

	settings.beginGroup("lesson");
	lineImportPath->setText(settings.value("downloadpath", LESSON_DOWNLOAD_URL).toString());
	settings.endGroup();

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

void DownloadDialog::writeSettings() {
	// Saves settings of the startwiget
	// (uses the default constructor of QSettings, passing
	// the application and company name see main function)
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif

	settings.beginGroup("lesson");
	settings.setValue("downloadpath", lineImportPath->text());
	settings.endGroup();

	settings.beginGroup("proxy");
	settings.setValue("check_proxy", checkProxy->isChecked());
	settings.setValue("proxy_server", txtProxyServer->text());
	settings.setValue("proxy_port", txtProxyPort->text());
	settings.endGroup();
}
