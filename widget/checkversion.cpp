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
** Implementation of the CheckVersion class
** File name: checkversion.cpp
**
****************************************************************/

#include <QTextStream>
#include <QString>
#include <QtNetwork>
#include <QSqlQuery>

#include "checkversion.h"
#include "def/defines.h"

// Constructor
CheckVersion::CheckVersion()
{
    // QHttp object provides interface to HTTP
    http = new QHttp();

    connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
        this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
    connect(http, SIGNAL(done(bool)), this, SLOT(httpDownloadFinished(bool)));
    connect(http, SIGNAL(dataReadProgress(int, int)), this,
        SLOT(updateDataReadProgress(int, int)));
}

void CheckVersion::checkVersion()
{
    tempVersionFile = new QTemporaryFile;
    if (!tempVersionFile->open()) {
        return;
    }

    http->setHost(UPDATE_URL);

    // Proxy server?
    //http->setProxy(txtProxyServer->text(), txtProxyPort->text().toInt());

    http->get(UPDATE_URL_VERSION, tempVersionFile);
}

void CheckVersion::updateDataReadProgress(int bytesRead, int totalBytes) {

}

void CheckVersion::readResponseHeader(const QHttpResponseHeader
        &responseHeader)
{
    if (responseHeader.statusCode() != 200) {
        http->abort();
        return;
    }
}

void CheckVersion::httpDownloadFinished(bool error)
{
	// Download finished
    if (error) {
		return;
    }

    if (checkVersionFile()) {
        // DB Version is new
        delete tempVersionFile;
        emit newVersionAvailable();
    }
}

bool CheckVersion::checkVersionFile()
{
	// Go to the beginning of the version file
    tempVersionFile->seek(0);

	QSqlQuery query;
	QTextStream in(tempVersionFile);
	// Read only the first line (server DB version)
	QString updateVersion = in.readLine();

    if (updateVersion.isNull()) {

        return false;

	} else {
		// Check DB version of software
        if (!query.exec("SELECT * FROM db_version ORDER BY version DESC;")) {

			return false;

		} else {

            if (!query.first()) {

				return false;

			} else {
                // Server DB version is 0 or bigger than current version
                QString softwareVersion = query.value(0).toString();

                if (updateVersion.trimmed() == "0" ||
                    softwareVersion.trimmed() != updateVersion.trimmed()) {

                    return true;
                }
			}
		}
    }

	return false;

}
