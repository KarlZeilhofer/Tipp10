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
** Implementation of the CharTableSql class
** File name: chartablesql.cpp
**
****************************************************************/

#include <QTableView>
#include <QObject>
#include <QItemSelectionModel>
#include <QAbstractItemView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColor>
#include <QFont>
#include <QMessageBox>

#include "chartablesql.h"
#include "def/defines.h"
#include "def/errordefines.h"
#include "widget/errormessage.h"
#include "startsql.h"

CharSqlModel::CharSqlModel(QWidget *parent) : QSqlQueryModel(parent) {
	// Remember the parent widget
	parentWidget = parent;
}

QVariant CharSqlModel::data(const QModelIndex &index, int role) const {
	QVariant value = QSqlQueryModel::data(index, role);
	int unicode;
    QString unicodeToChar;
    QString errorRatioString;
    double errorRatio;
	if (value.isValid() && role == Qt::DisplayRole) {
		// First column
		if (index.column() == 0) {
			// Read the unicode value
			unicode = value.toInt();
			// Convert unicode to a char
			unicodeToChar = QString(QChar(unicode)); //"\'" + QString(QChar(unicode)) + "\'";
			return unicodeToChar;
		} else {
			// Last column (error weight)
			if (index.column() == 4) {
				// If there is no error ratio (this happens if the denominator
				// of error weight calculation is zero
                if ((errorRatioString = value.toString()) == "") {
                    errorRatioString = "0";
                }
                errorRatio = errorRatioString.toDouble();
                errorRatioString.sprintf("%.0f", errorRatio);
                // Append a percent sign
                return errorRatioString.append(tr(" %"));
			}
		}
	}
	// Change font of first column to bold
	if (role == Qt::FontRole && index.column() == 0) {
		QFont font;
		font = parentWidget->font();
		font.setFamily("Courier New");
		font.setPointSize(font.pointSize() + 2);
		font.setBold(true);
		return qVariantFromValue(font);
	}
	// Change font of first column to bold
	if (role == Qt::FontRole && index.column() == 4) {
		QFont font;
		font = parentWidget->font();
		font.setBold(true);
		return qVariantFromValue(font);
	}
	return value;
}

CharTableSql::CharTableSql(QWidget *parent) : QWidget(parent) {
	previousColumnIndex = -1;
	// Create QModel object
	model = new CharSqlModel(this);

	// Column headers (see sql query)
	model->setHeaderData(0, Qt::Horizontal, tr("Schriftzeichen"));
	model->setHeaderData(1, Qt::Horizontal, tr("Soll-Fehler"));
	model->setHeaderData(2, Qt::Horizontal, tr("Ist-Fehler"));
	model->setHeaderData(3, Qt::Horizontal, tr("Vorkommen"));
	model->setHeaderData(4, Qt::Horizontal, tr("Fehlerquote"));

	model->setHeaderData(0, Qt::Horizontal,
		tr("Diese Spalte zeigt alle bislang\n"
			"eingegebenen Schriftzeichen"), Qt::ToolTipRole);
	model->setHeaderData(1, Qt::Horizontal,
		tr("Ein \"Soll-Fehler\" entsteht, wenn ein anderes\n"
		"Schriftzeichen eingegeben wurde als das hier\n"
		"vorgegebene"), Qt::ToolTipRole);
	model->setHeaderData(2, Qt::Horizontal,
		tr("Ein \"Ist-Fehler\" entsteht, wenn das Schriftzeichen\n"
		"trotz anderer Vorgabe eingegeben wurde"), Qt::ToolTipRole);
	model->setHeaderData(3, Qt::Horizontal,
		tr("Diese Spalte gibt an, wie oft das Schriftzeichen\n"
		"ingesamt diktiert wurde"), Qt::ToolTipRole);
	model->setHeaderData(4, Qt::Horizontal,
		tr("Die Fehlerquote zeigt, welche Schriftzeichen\n"
		"Ihnen am meisten Probleme bereiten.\n"
		"Die Fehlerquote errechnet sich aus dem\n"
		"Wert \"Soll-Fehler\" und dem Wert\n"
		"\"Vorkommen\"."), Qt::ToolTipRole);

	// Create the table view
	QTableView *view = new QTableView;
	view->setModel(model);
	// User should not be able to select a row
	view->setSelectionMode(QAbstractItemView::NoSelection);

	headerview = view->horizontalHeader();

	// Set the sql query (every unicode, it's errors and an error weight)
	sortColumn(4);

	headerview->setStretchLastSection(true);
    headerview->setSectionResizeMode(QHeaderView::Interactive);
	headerview->setSortIndicatorShown(true);

	// Resize the columns
	view->resizeColumnsToContents();


    buttonReset = new QPushButton(tr("Schriftzeichen zuruecksetzen"));
    buttonReset->setFixedHeight(20);
    if (model->rowCount() == 0) {
        buttonReset->setEnabled(false);
    }

    // Set a horizonal layout
    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addStretch(1);
    filterLayout->addWidget(buttonReset);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(view);
    // Pass layout to parent widget (this)
    this->setLayout(mainLayout);

    connect(headerview, SIGNAL(sectionClicked(int)), this, SLOT(sortColumn(int)));
    connect(buttonReset, SIGNAL(clicked()), this, SLOT(deleteUserChars()));
}

void CharTableSql::sortColumn(int columnindex) {
	// Select columnname from columnindex
	QString columnName;
	int isDesc;
	switch (columnindex) {
		case 0:
			columnName = "user_char_unicode";
			break;
		case 1:
			columnName = "user_char_target_errornum";
			break;
		case 2:
			columnName = "user_char_mistake_errornum";
			break;
		case 3:
			columnName = "user_char_occur_num";
			break;
		case 4:
		default:
			columnName = "(user_char_target_errornum * 100.0) / user_char_occur_num";
			break;
	}
	if (previousColumnIndex != columnindex) {
		isDesc = 1;
		headerview->setSortIndicator(columnindex, Qt::DescendingOrder);
	} else {
		if (headerview->sortIndicatorOrder() == Qt::AscendingOrder) {
			isDesc = 0;
		} else {
			isDesc = 1;
		}
	}
	previousColumnIndex = columnindex;
	// Call new query
	setQueryOrder(columnName, isDesc);
}

void CharTableSql::setQueryOrder(QString columnname, int isdesc)
{
    if (columnname != "") {
        QString descText;
        if (isdesc == 1) {
            descText = " DESC";
        } else {
            descText = " ASC";
        }
        sql = "SELECT user_char_unicode, user_char_target_errornum, "
              "user_char_mistake_errornum, user_char_occur_num, "
              "((user_char_target_errornum * 100.0) / user_char_occur_num) "
              "AS user_char_weighted FROM user_chars ORDER BY "
              + columnname + descText + ";";
    }
	// Set the sql query (every unicode, it's errors and an error weight)
    model->setQuery(sql);

}

void CharTableSql::deleteUserChars() {
    switch (QMessageBox::question(this, APP_NAME,
        tr("Die aufgezeichneten Fehlerquoten beeinflussen die Intelligenzfunktion "
           "und damit auch die Auswahl der zu diktierenden Texte. "
           "Wenn die Fehlerquote eines bestimmten Zeichens uebermaessig hoch ist, "
           "kann es unter Umstaenden sinnvoll sein, die Liste zurueckzusetzen.\n\n"
           "Es werden nun alle aufgezeichneten Schriftzeichen geloescht.\n\n"
           "Wollen Sie den Vorgang wirklich "
           "fortsetzen?\n"), tr("&Ja"), tr("&Abbrechen"),
           0, 1)) {
        case 0:
            StartSql *userSql = new StartSql();
            if (!userSql->deleteUserChars()) {
                // Error message
                ErrorMessage *errorMessage = new ErrorMessage(this);
                errorMessage->showMessage(ERR_USER_ERRORS_FLUSH,
                    TYPE_WARNING, CANCEL_OPERATION);
                return;
            }
            setQueryOrder("", 0);
            buttonReset->setEnabled(false);
            break;
    }
}
