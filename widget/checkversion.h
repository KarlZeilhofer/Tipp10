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
** Definition of the CheckVersion class
** File name: checkversion.h
**
****************************************************************/

#ifndef CHECKVERSION_H
#define CHECKVERSION_H

#include <QHttp>
#include <QHttpResponseHeader>
#include <QTemporaryFile>

//! The CheckVersion class checks for new software version.
/*!
	@author Tom Thielicke, s712715
    @version 0.0.1
    @date 02.01.2011
*/
class CheckVersion : public QObject {
    Q_OBJECT

    public:

        CheckVersion();

        //! Slot, download the database version from server.
        void checkVersion();

    signals:

        void newVersionAvailable();

    private slots:

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

	private:

		//! Checks wether the version is new.
        bool checkVersionFile();

        QHttp *http;
        QTemporaryFile *tempVersionFile;
};

#endif // CHECKVERSION_H
