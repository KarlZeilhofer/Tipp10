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
** Implementation of the LessonTableSql class
** File name: lessontablesql.h
**
****************************************************************/

#include <QObject>
#include <QItemSelectionModel>
#include <QAbstractItemView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QColor>
#include <QFont>
#include <QDateTime>
#include <QSettings>
#include <QTextEdit>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextCursor>
#include <QTextFrame>
#include <QTextTable>
#include <QTextFrameFormat>
#include <QTextCharFormat>
#include <QTextTableFormat>
#include <QCoreApplication>
#include <QString>
#include <QList>
#include <QChar>
#include <QPen>
#include <QColor>
#include <math.h>

#include "lessontablesql.h"
#include "widget/lessonprintdialog.h"
#include "def/defines.h"

LessonSqlModel::LessonSqlModel(int row, int type, QWidget *parent) : QSqlQueryModel(parent) {
	parentWidget = parent;
	lastIdInserted = row;
	lastTypeInserted = type;
	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
		"/portable/settings.ini", QSettings::IniFormat);
	#else
	QSettings settings;
	#endif
	settings.beginGroup("general");
	language = settings.value("language_gui", "en").toString();
	settings.endGroup();

}

QVariant LessonSqlModel::data(const QModelIndex &index, int role) const {
	QVariant value = QSqlQueryModel::data(index, role);
	QDateTime timeStamp;
	double lessonGrade;
    QString lessonGradeString;
	int timeSec;
    double timeMin;
    double lessonCpm;
    QString lessonCpmString;
    double lessonRate;
    QString lessonRateString;
	static int coloredRow = -1;
	QString lessonName;
	if (value.isValid() && role == Qt::DisplayRole) {
		if (index.column() == 0) {
			// Expand lesson name
			lessonName = value.toString();
			return lessonName; //.prepend(tr("Uebungslektion "));
		}
		if (index.column() == 1) {
			// Convert time stamp into a readable format
			timeStamp = QDateTime::fromString(value.toString(), "yyyyMMddhhmmss");
            return timeStamp.toString((language == "de" ? "dd.MM.yyyy hh:mm" : "MMM d, yyyy hh:mm ap")) +
				(language == "de" ? tr(" Uhr") : "");
		}
		if (index.column() == 2) {
			// Show time length in seconds or minutes
			if ((timeSec = value.toInt()) < 60) {
				return QString::number(timeSec) + tr(" sek.");
			} else {
				timeMin = floor((timeSec / 60.0) / 0.1 + 0.5) * 0.1;
				return QString::number(timeMin) + tr(" min.");
			}
        }
        if (index.column() == 5) {
            // Rate
            lessonRate = value.toDouble();

            lessonRateString.sprintf("%.0f", lessonRate);
            return lessonRateString + " %";;
        }
        if (index.column() == 6) {
            // There is never grade smaller than zero
            lessonCpm = value.toDouble();

            lessonCpmString.sprintf("%.0f", lessonCpm);
            return lessonCpmString;
        }
        if (index.column() == 7) {
            // There is never grade smaller than zero
            if ((lessonGrade = value.toDouble()) < 0) {
                lessonGrade = 0;
            }
            lessonGradeString.sprintf("%.0f", lessonGrade);
            return lessonGradeString +
                (lessonGradeString == "1" ? tr(" Punkt") : tr(" Punkte"));
		}
        if (index.column() == 8 && value.toInt() == lastIdInserted) {
			// Current row has to be colored
			coloredRow = index.row();
			return QString::number(coloredRow);
		}
		if (lastIdInserted == 0) {
			// No row is colored
			coloredRow = -1;
		}
	}
    if (role == Qt::FontRole && (index.column() == 0 || index.column() == 7)) {
		// Show the lesson number bold
		QFont font;
		font = parentWidget->font();
		font.setBold(true);
		return qVariantFromValue(font);
	}
	/*if (role == Qt::BackgroundColorRole) {
		if (index.row() == coloredRow) {
			// Show the row with background color
			return qVariantFromValue(QColor(249, 126, 50));
		}
	}*/
	//if (role == Qt::TextColorRole
	return value;
}

LessonTableSql::LessonTableSql(int row, int type, QList<QChar> charlist,
	QList<int> mistakelist, QWidget *parent) : QWidget(parent) {



	mistakeList = mistakelist;
	charList = charlist;
	lessonRow = row;

	previousColumnIndex = -1;
	whereClausel = "";

	#if APP_PORTABLE
	QSettings settings(QCoreApplication::applicationDirPath() +
    	"/portable/settings.ini", QSettings::IniFormat);
    #else
	QSettings settings;
	#endif
	printVisible = false;

	// Create filter headline
	labelFilter = new QLabel(tr("Zeige: "));
	comboFilter = new QComboBox();
	comboFilter->insertItem(0, tr("Alle Lektionen"));
	comboFilter->insertItem(1, tr("Uebungslektionen"));
	comboFilter->insertItem(2, tr("Freie Lektionen"));
	comboFilter->insertItem(3, tr("Eigene Lektionen"));

	// Create QModel object
	model = new LessonSqlModel(row, type, this);

    setModelHeader();
	//model->setSort(0, Qt::AscendingOrder);

	// Create the table view
	view = new QTableView;

	view->setModel(model);
	// User should not be able to select a row
	view->setSelectionMode(QAbstractItemView::NoSelection);
	// Hide the lesson id column

	headerview = view->horizontalHeader();

	// Set the sql query (every lesson, it's properties and rating)
	sortColumn(-1);

	headerview->setStretchLastSection(true);
	headerview->setResizeMode(QHeaderView::Interactive);
	headerview->setSortIndicatorShown(true);

	// Resize the columns
	view->resizeColumnsToContents();
    view->setColumnHidden(8, true);
	view->resizeColumnsToContents();
	//view->setColumnHidden(8, true);

	// Set a horizonal layout
	QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addStretch(1);
	filterLayout->addWidget(labelFilter);
	filterLayout->addWidget(comboFilter);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(filterLayout);
	mainLayout->addWidget(view);
	// Pass layout to parent widget (this)
	this->setLayout(mainLayout);

	connect(headerview, SIGNAL(sectionClicked(int)), this, SLOT(sortColumn(int)));
	connect(comboFilter, SIGNAL(activated(int)), this, SLOT(changeFilter(int)));
}

void LessonTableSql::sortColumn(int columnindex) {
	// Select columnname from columnindex
	switch (columnindex) {
		case 0:
			columnName = "user_lesson_lesson";
			break;
		case 1:
		default:
			columnName = "user_lesson_timestamp";
			columnindex = 1;
			break;
		case 2:
			columnName = "user_lesson_timelen";
			break;
		case 3:
			columnName = "user_lesson_tokenlen";
			break;
		case 4:
			columnName = "user_lesson_errornum";
            break;
        case 5:
            columnName = "user_lesson_rate";
            break;
        case 6:
            columnName = "user_lesson_cpm";
            break;
        case 7:
			columnName = "user_lesson_grade";
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
	if (columnindex != -1) {
		model->lastIdInserted = 0;
	}
	// Call new query
	setQueryOrder(columnName, isDesc);
}

void LessonTableSql::changeFilter(int rowindex) {
	// Select columnname from columnindex
	switch (rowindex) {
		case 0:
			whereClausel = "";
			break;
		case 1:
		default:
			whereClausel = "WHERE user_lesson_type = 0 ";
			break;
		case 2:
			whereClausel = "WHERE user_lesson_type = 1 ";
			break;
		case 3:
			whereClausel = "WHERE user_lesson_type = 2 ";
			break;
	}
	model->lastIdInserted = 0;
	// Call new query
	setQueryOrder(columnName, isDesc);
}

void LessonTableSql::setQueryOrder(QString columnname, int isdesc) {

	QString descText;
	if (isdesc == 1) {
		descText = " DESC";
	} else {
		descText = " ASC";
	}
    model->clear();

	// Set the sql query (every lesson, it's properties and rating)
	model->setQuery("SELECT user_lesson_name, user_lesson_timestamp, "
		"user_lesson_timelen, user_lesson_tokenlen, "
		"user_lesson_errornum, "
        "((user_lesson_errornum * 100.0) / "
        " user_lesson_strokesnum) AS user_lesson_rate, "
        "(user_lesson_strokesnum / "
        "(user_lesson_timelen / 60.0)) AS user_lesson_cpm, "
        "(((user_lesson_strokesnum - (20.0 * user_lesson_errornum)) / "
        "(user_lesson_timelen / 60.0)) * 0.4) AS user_lesson_grade, "
		"user_lesson_id FROM user_lesson_list " +
		whereClausel + "ORDER BY " + columnname + descText + ";");

    setModelHeader();
}

void LessonTableSql::setModelHeader()
{
    // Column headers (see sql query)
    model->setHeaderData(0, Qt::Horizontal, tr("Lektion"));
    model->setHeaderData(1, Qt::Horizontal, tr("Zeitpunkt"));
    model->setHeaderData(2, Qt::Horizontal, tr("Dauer"));
    model->setHeaderData(3, Qt::Horizontal, tr("Zeichen"));
    model->setHeaderData(4, Qt::Horizontal, tr("Fehler"));
    model->setHeaderData(5, Qt::Horizontal, tr("Quote"));
    model->setHeaderData(6, Qt::Horizontal, tr("A/min"));
    model->setHeaderData(7, Qt::Horizontal, tr("Bewertung"));

    model->setHeaderData(0, Qt::Horizontal,
        tr("Diese Spalte zeigt die Namen\n"
            "der absolvierten Lektionen"), Qt::ToolTipRole);
    model->setHeaderData(1, Qt::Horizontal,
        tr("Startzeitpunkt der Lektion"), Qt::ToolTipRole);
    model->setHeaderData(2, Qt::Horizontal,
        tr("Dauer der Lektion insgesamt"), Qt::ToolTipRole);
    model->setHeaderData(3, Qt::Horizontal,
        tr("Anzahl der Schriftzeichen, die\n"
        "ingesamt diktiert wurden"), Qt::ToolTipRole);
    model->setHeaderData(4, Qt::Horizontal,
        tr("Anzahl der Tippfehler, die in\n"
        "der Lektion gemacht wurden"), Qt::ToolTipRole);
    model->setHeaderData(5, Qt::Horizontal,
        tr("Tippfehler in Abhaengigkeit von der\n"
           "Diktatlaenge (Tippfehler / Zeichen)\n"
           "Umso geringer die Fehlerquote, desto besser!"), Qt::ToolTipRole);
    model->setHeaderData(6, Qt::Horizontal,
        tr("\"A/min\" besagt, wie viele Schriftzeichen (Anschlaege)\n"
        "durchschnittlich pro Minute eingegeben wurden"), Qt::ToolTipRole);
    model->setHeaderData(7, Qt::Horizontal,
        tr("Die Bewertung der Leistung errechnet sich wie folgt:\n"
        "((Zeichen - (20 x Fehler)) / Dauer in Minuten) x 0.4\n \n"
        "Beachten Sie, dass langsames Tippen ohne Fehler\n"
        "eine bessere Bewertung hervorruft, als schnelles\n"
        "Tippen mit vielen Fehlern!"), Qt::ToolTipRole);
}
