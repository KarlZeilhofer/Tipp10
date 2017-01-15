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
** Definition of the EvaluationWidget class
** File name: evaluationwidget.h
**
****************************************************************/

#ifndef EVALUATIONWIDGET_H
#define EVALUATIONWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QTabWidget>
#include <QTextBrowser>

#include "companylogo.h"
#include "def/defines.h"
#include "progressionwidget.h"
#include "fingerwidget.h"
#include "sql/chartablesql.h"
#include "sql/lessontablesql.h"
#include "lessonresult.h"
#include "helpbrowser.h"

//! The EvaluationWidget class provides a user evaluation widget.
/*!
	The EvaluationWidget class has two tabs. The first tab shows the
	evaluation of the lessons the user completed. It includes an object of the
	class LessonTable (which provides a table of all lessons).
	The second tab shows the evaluation of all chars the user typed. It
	includes an object of the class CharTable (which provides a table of
	all chars and their errors).
	A button in the right bottom corner enables the user to close the widget.

	@author Tom Thielicke, s712715
	@version 0.0.2
	@date 10.06.2006
*/
class EvaluationWidget : public QWidget {
	// Necessary to create own signals, slots and connections
    Q_OBJECT

	public:

		//! Constructor, creates two table objects and provide it in two tabs.
		/*!
			In this contructor several objects are created. First the company
			logo in orange (companyLogo), a tab widget (tabEvaluation) and two
			evaluation tables (lessonTable and charTable). Everyone of these
			tables are inserted in an own tab.
			At the end a button is created with function createButtons() and
			a layout is defined with function createLayout(). The button will
			be connected to slot clickReady().

			@param row The row id of current lesson
			@param parent The parent QWidget
			@see createButtons(), createLayout(), clickReady(), companyLogo,
				tabEvaluation, lessonTable, charTable, buttonReady
		*/
		EvaluationWidget(int row, int type, QList<QChar> charList,
			QList<int> mistakeList, QWidget *parent = 0);

	signals:

		//! Signal, emits that the user has finished the evaluation.
		/*!
			After the user clicked the ready button, the function clickReady()
			emits this signal to inform the parent class about user finished.
		*/
		void readyClicked();

	private slots:

		//! Slot, connected with the clicked() function of the ready button.
		/*!
			After the user pushed the ready button, this function emits the
			signal readyClicked() to inform the parent class about user
			finished.
		*/
		void clickReady();

		//! Slot, shows the help dialog.
		/*!
			This slot shows the help dialog. It creates an object of
			the QDialog class with an QTextbrowser.
		*/
		void showHelp();

	private:

		//! Creates a ready button that the user can exit from this widget.
		void createButtons();

		//! Creates the layout of the complete class.
		void createLayout();

        void createComparisonTable();

		//! Ready button.
		QPushButton *buttonReady;
		QPushButton *buttonHelp;

		//! Tab widget
		QTabWidget *tabEvaluation;

		//! Object of the company logo widget
		CompanyLogo *companyLogo;

		//! Object of the table with user's lessons
		LessonTableSql *lessonTableSql;

		//! Object of the table with user's chars
		CharTableSql *charTableSql;

		//! Object of the table with user's chars
        LessonResult *lessonResult;

        //! Object of the widget with the progression chart
        ProgressionWidget *progressionWidget;

        //! Object of the widget with the finger error rate chart
        FingerWidget *fingerWidget;

        //! Object of the widget with the comparison table
        QTextBrowser *comparisonWidget;

		//! Object of the help browser dialog
		HelpBrowser *helpBrowser;

		int currentRow;
};

#endif // EVALUATIONWIDGET_H
