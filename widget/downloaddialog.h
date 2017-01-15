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
** Definition of the DownloadDialog class
** File name: downloaddialog.h
**
****************************************************************/

#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QHttp>
#include <QHttpResponseHeader>
#include <QTemporaryFile>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QString>
#include <QStringList>
#include <QProgressBar>

#include "helpbrowser.h"

//! The DownloadDialog class provides a license input widget.
/*!
	The DownloadDialog class shows a dialog to enter a license key.

	@author Tom Thielicke, s712715
	@version 0.0.1
	@date 09.09.2008
*/
class DownloadDialog : public QDialog {
	Q_OBJECT

	public:

		//! Constructor, creates two table objects and provide it in two tabs.
		DownloadDialog(QStringList *data, QWidget *parent = 0);

	public slots:

	private slots:

		//! Slot, download the text file from server.
		void downloadTxtFile();

		//! Slot, data read progress while downloading.
		/*!
			@param bytesRead Number of bytes read
			@param totalBytes Number total bytes
		*/
		void updateDataReadProgress(int bytesRead, int totalBytes);

		//! Slot, http response header read.
		/*!
			@param responseHeader http response header
		*/
		void readResponseHeader(const QHttpResponseHeader &responseHeader);

		//! Slot, download has finished.
		/*!
			@param error An error occured true/false
		*/
		void httpDownloadFinished(bool error);

		//! Slot, shows the help dialog.
		/*!
			This slot shows the help dialog. It creates an object of
			the QDialog class with an QTextbrowser.
		*/
		void showHelp();

	private:

		//! Creates the progress bar and status label.
		void createProgressinfo();

		void showProgressControls(bool show);

		bool checkTxtFile();

		//! Creates the controls.
		void createControls();

		//! Creates the connections between widgets an QHttp functions.
		void createConnections();

		//! Creates the layout of the complete class.
		void createLayout();

		//! Writes user settings
		void writeSettings();

		//! Reads user settings
		void readSettings();

		//! Object of the help browser dialog
		HelpBrowser *helpBrowser;

		QHttp *http;
		QTemporaryFile *tempTxtFile;

		QPushButton *buttonOk;
		QPushButton *buttonHelp;
		QPushButton *buttonCancel;
		QLabel *labelImportPath;
		QLineEdit *lineImportPath;
		QStringList *lessonData;

		QCheckBox *checkProxy;
		QLineEdit *txtProxyServer;
		QLabel *labelProxyServer;
		QLineEdit *txtProxyPort;
		QLabel *labelProxyPort;

		QProgressBar *progressBar;

		bool downloadCanceled;
};

#endif //DOWNLOADDIALOG_H
