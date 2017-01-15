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
** Definition of the SettingDialog class
** File name: illustrationdialog.h
**
****************************************************************/

#ifndef ILLUSTRATIONDIALOG_H
#define ILLUSTRATIONDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QString>
#include <QTextBrowser>

//! The IllustrationDialog class provides a program settings widget.
/*!
    The IllustrationDialog class shows three program settings. Setting the
    layout of the virtual keyboard, setting the ticker speed and font
    and doing a reset of the user tables.

    @author Tom Thielicke, s712715
    @version 0.0.3
    @date 18.06.2006
*/
class IllustrationDialog : public QDialog {
    Q_OBJECT

    public:

        //! Constructor, creates an illustration widget.
        /*!

            @param parent The parent QWidget
        */
        IllustrationDialog(QWidget *parent = 0);

    public slots:

    private slots:

        //! Start button pressed
        void clickStart();

    private:

        //! Creates a cancel and a save button.
        void createButtons();

        //! Creates content widget
        void createContent();

        //! Creates the layout of the complete class.
        void createLayout();

        //! Writes user settings
        void writeSettings();

        //! Reads user settings
        void readSettings();

        QString languageGui;

        QPushButton *buttonStart;
        QCheckBox *showDialogCheck;

        QTextBrowser *illustrationContent;
};

#endif //ILLUSTRATIONDIALOG_H
