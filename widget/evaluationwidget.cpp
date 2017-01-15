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
** Implementation of the EvaluationWidget class
** File name: evaluationwidget.cpp
**
****************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "evaluationwidget.h"

EvaluationWidget::EvaluationWidget(int row, int type, QList<QChar> charList,
	QList<int> mistakeList, QWidget *parent) : QWidget(parent) {

	currentRow = row;
	// Set the logo
	companyLogo = new CompanyLogo(this);
	// Create tab widget
	tabEvaluation = new QTabWidget(this);
	// Object of the user's lesson table
	lessonTableSql = new LessonTableSql(row, type, charList, mistakeList, this);
	// Object of the user's char table
    charTableSql = new CharTableSql(this);
    // Object of the progression chart widget
    progressionWidget = new ProgressionWidget(this);
    // Object of the finger error rate chart widget
    fingerWidget = new FingerWidget(this);
    // Object of the comparison chart widget
    QWidget *comparisonContainer = new QWidget();
    comparisonWidget  = new QTextBrowser(comparisonContainer);
    QVBoxLayout *comparisonLayout = new QVBoxLayout;
    comparisonLayout->addWidget(comparisonWidget);

    comparisonContainer->setLayout(comparisonLayout);

    createComparisonTable();

	if (row > 0) {
		lessonResult = new LessonResult(row, type, charList, mistakeList, this);
		tabEvaluation->addTab(lessonResult, tr("Bericht"));
	}

	// Add three tabs and insert the table objects
	tabEvaluation->addTab(lessonTableSql, tr("Lektionenuebersicht"));
    tabEvaluation->addTab(progressionWidget, tr("Lektionenverlauf"));
    tabEvaluation->addTab(charTableSql, tr("Schriftzeichen"));
    tabEvaluation->addTab(fingerWidget, tr("Finger"));
    tabEvaluation->addTab(comparisonContainer, tr("Vergleichstabelle"));

    // Create exit (ready) button
    createButtons();

	// Set the layout of all widgets above
	createLayout();

	// Button connection to private slot clickReady()
    connect(buttonReady, SIGNAL(clicked()), this, SLOT(clickReady()));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(showHelp()));

	// Set focus on button
    //buttonReady->setFocus();
}

void EvaluationWidget::createButtons() {
	// Default button "Ready"
	buttonHelp = new QPushButton(tr("&Hilfe"));
	buttonReady = new QPushButton(tr("&Fertig"));
	buttonReady->setDefault(true);
}

void EvaluationWidget::createLayout() {
	// Button layout horizontal
	QHBoxLayout *buttonLayoutHorizontal = new QHBoxLayout;
    buttonLayoutHorizontal->addWidget(buttonHelp);
    buttonLayoutHorizontal->addWidget(buttonReady);
	// Button layout vertical
    QVBoxLayout *buttonLayoutVertical = new QVBoxLayout;
    buttonLayoutVertical->addSpacing(20);
    buttonLayoutVertical->addLayout(buttonLayoutHorizontal);
	// Logo layout vertical
    QVBoxLayout *logoLayout = new QVBoxLayout;
    logoLayout->addSpacing(10);
    logoLayout->addWidget(companyLogo);
	// Logo layout + button layout horizontal
	QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addLayout(logoLayout);
    bottomLayout->addStretch(1);
    bottomLayout->addLayout(buttonLayoutVertical);
	// Full layout of all widgets vertical
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabEvaluation);
    mainLayout->addSpacing(1);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(15);
    // Pass layout to parent widget (this)
	this->setLayout(mainLayout);
}

void EvaluationWidget::clickReady() {
	// User finished the evaluation
	// -> emit a ready signal
	emit readyClicked();
}

void EvaluationWidget::showHelp() {
	QString ahref = "";
	int tab = tabEvaluation->currentIndex();
	if (currentRow <= 0) {
		tab++;
	}
	switch (tab) {
		case 0:
			ahref = "#registerreport";
			break;
		case 1:
			ahref = "#registerlessons";
			break;
		case 2:
			ahref = "#registerprogression";
			break;
		case 3:
			ahref = "#registerchars";
            break;
        case 4:
            ahref = "#fingerrates";
            break;
        case 5:
            ahref = "#registercomparison";
            break;
	}
    helpBrowser = new HelpBrowser("results.html" + ahref, 0);
	helpBrowser->show();
}

void EvaluationWidget::createComparisonTable() {

    QString content;

    content = ""
        "<div style=\"margin-top:12px;margin-left:16px;margin-bottom:12px;margin-right:16px;\">"
            "<div style=\"margin-top:16px;font-size:12px;\">"
                "&nbsp;<br><b>" + tr("Beispielbewertungen") + "</b>"
            "</div>"
            "<div style=\"margin-top:10px;font-size:11px;\">" +
                tr("Beachten Sie, dass langsames Tippen ohne Fehler bessere Bewertungen hervorruft, als schnelles Tippen mit vielen Fehlern!") +
            "</div>"
            "<div style=\"margin-top:8px;font-size:10px;\">"
            "<table width=\"100%\" border=\"0.5\"cellspacing=\"0\" cellpadding=\"1\" style=\"border-color:#444444;border-style:solid;\">"
                "<tr>"
                    "<td bgcolor=\"#DDDDDD\">"
                        "<b>" + tr("Bewertung") + "</b>"
                    "</td>"
                    "<td bgcolor=\"#DDDDDD\">"
                        "<b>" + tr("Entspricht zum Beispiel ...") + "</b>"
                    "</td>"
                    "<td bgcolor=\"#DDDDDD\">"
                        "<b>" + tr("Leistung") + "</b>"
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "0 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "20 " + tr("A/min und") + " 16 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\" rowspan=\"3\" valign=\"bottom\">"
                        "" + tr("Ohne Erfahrung im Zehnfingersystem") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "5 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "20 " + tr("A/min und") + " 2 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "8 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "20 " + tr("A/min und") + " 0 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "9 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "30 " + tr("A/min und") + " 2 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\" rowspan=\"3\" valign=\"bottom\">"
                        "" + tr("Erste Erfahrungen im Zehnfingersystem") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "13 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "40 " + tr("A/min und") + " 2 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "16 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "40 " + tr("A/min und") + " 0 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "17 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "50 " + tr("A/min und") + " 2 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\" rowspan=\"3\" valign=\"bottom\">"
                        "" + tr("Fortgeschritten") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "25 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "70 " + tr("A/min und") + " 2 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "32 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "80 " + tr("A/min und") + " 0 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "35 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "100 " + tr("A/min und") + " 3 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\" rowspan=\"3\" valign=\"bottom\">"
                        "" + tr("Brauchbare Leistung") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "37 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "100 " + tr("A/min und") + " 2 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "45 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "120 " + tr("A/min und") + " 2 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "48 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "120 " + tr("A/min und") + " 0 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\" rowspan=\"3\" valign=\"bottom\">"
                        "" + tr("Sehr gute Leistung") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "53 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "140 " + tr("A/min und") + " 2 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "69 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "180 " + tr("A/min und") + " 2 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "77 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "200 " + tr("A/min und") + " 2 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\" rowspan=\"3\" valign=\"bottom\">"
                        "" + tr("Hervorragende Leistung") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "86 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "200 " + tr("A/min und") + " 1 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "110 " + tr("Punkte") + ""
                    "</td>"
                    "<td bgcolor=\"#FFFFFF\">"
                        "220 " + tr("A/min und") + " 1 " + tr("Fehler in") + " 5 " + tr("Minuten") + ""
                    "</td>"
                "</tr>"
                "</table>"
                "<br>&nbsp;"
            "</div>"
        "</div>";

    comparisonWidget->viewport()->setAutoFillBackground(false);
    comparisonWidget->setHtml(content);
}
