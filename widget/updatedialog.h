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
** Definition of the UpdateDialog class
** File name: updatedialog.h
**
****************************************************************/

#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>
#include <QWidget>

#ifdef ONLINE
#include <QHttp>
#include <QHttpResponseHeader>


#include <QTemporaryFile>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>

#include "helpbrowser.h"

//! The UpdateDialog class provides an online update dialog.
/*!
	@author Tom Thielicke, s712715
	@version 0.1.1
	@date 19.06.2006
*/
class UpdateDialog : public QDialog {
    Q_OBJECT

	public:

		//! Constructor, creates a progress bar, a label and a start button.
		/*!
			In this contructor the following variables are initialized:
				- newVersion = false;

			In addition, it creates a progress bar, a status label and a start
			button.

			@param parent The parent QWidget
			@see createProgressinfo(), createButtons(), createLayout(),
				createConnections(), newVersion, buttonUpdate
		*/
		UpdateDialog(QWidget *parent = 0);

	private slots:

		//! Slot, download the database version from server.
		void downloadVersionFile();

		//! Slot, download the update sql file from server.
		void downloadSqlFile();

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

		//! Writes proxy settings to hard disk.
		void writeSettings();

		//! Slot, shows the help dialog.
		/*!
			This slot shows the help dialog. It creates an object of
			the QDialog class with an QTextbrowser.
		*/
		void showHelp();

	private:

		//! Creates the cancel and start update buttons.
		void createButtons();

		//! Creates the progress bar and status label.
		void createProgressinfo();

		//! Creates the full layout of the widget.
		void createLayout();

		//! Creates the connections between widgets an QHttp functions.
		void createConnections();

		//! Checks wether the version is new.
		bool checkVersionFile();

		//! Executes all sql command of the update file.
		bool executeSqlFile();

		//! Analyzes all text in database.
		bool analyzeLessons(QString lessonType);

		//! Reads proxy settings.
		void readSettings();

		//! Object of the help browser dialog
		HelpBrowser *helpBrowser;


        QHttp *http;
		bool newVersion;
		QTemporaryFile *tempVersionFile;
		QTemporaryFile *tempSqlFile;
		QLabel *labelStatus;
		QProgressBar *progressBar;
		QPushButton *buttonClose;
		QPushButton *buttonUpdate;
		QPushButton *buttonHelp;
		QCheckBox *checkProxy;
		QLineEdit *txtProxyServer;
		QLabel *labelProxyServer;
		QLineEdit *txtProxyPort;
		QLabel *labelProxyPort;
};
#endif // ONLINE
#endif // UPDATEDIALOG_H
