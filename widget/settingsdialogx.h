/****************************************************************
**
** Definition of the SettingDialog class
** File name: settingsdialog.h
**
****************************************************************/

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QFont>
#include <QString>

//! The SettingsDialog class provides a program settings widget.
/*!
	The SettingsDialog class shows three program settings. Setting the
	layout of the virtual keyboard, setting the ticker speed and font
	and doing a reset of the user tables.

	@author Tom Thielicke, s712715
	@version 0.0.3
	@date 18.06.2006
*/
class SettingsDialog : public QDialog {
	Q_OBJECT

	public:

		//! Constructor, creates two table objects and provide it in two tabs.
		/*!
			In this contructor three groups are created over functions
			createGroupKeyboardLayout(), createGroupUserReset() and
			createGroupTickerFont(). In addition, standard settings are read,
			standard font is set and connections are set.

			@param parent The parent QWidget
			@see createGroupKeyboardLayout(), createGroupUserReset(),
				createGroupTickerFont(), readSettings(),
				tickerFont
		*/
		SettingsDialog(QWidget *parent = 0);

	public slots:

	private slots:

		//! Slot, shows the font setting window.
		/*!
			After the user clicked the font button, this function creates
			a QFontDialog and sets the new font if user has choosen one.
		*/
		void setFont();

		//! Slot, saves the setting data.
		/*!
			After the user clicked the save button, this function saves the
			settings over function writeSettings() and closes the current
			dialog.

			@see writeSettings()
		*/
		void clickSave();

		//! Slot, deletes the content of the user tables.
		/*!
			After the user clicked the reset user data button, this function
			deletes the content data of the user table over
			StartSql class.

			@see StartSql
		*/
		void deleteUserData();

		//! Slot, opens and cheks the database path file dialog.
		/*!
			After the user clicked the browse button, this function
			shows a file dialog. After that, the exist of the file
			is checked.
		*/
		void setDatabasePath();

	private:

		//! Creates a radiobutton with windows and mac keyboard layout items.
		void createGroupKeyboardLayout();

    	//! Creates a ticker font button and the ticker speed spin box.
		void createGroupTickerFont();

    	//! Creates two user data reset buttons.
		void createGroupUserReset();

    	//! Creates a database path control.
		void createGroupDatabase();

    	//! Creates a show dialog on-off check boxes.
		void createGroupDialogCheck();

		//! Creates a cancel and a save button.
		void createButtons();

		//! Creates the layout of the complete class.
		void createLayout();

		//! Sets the name of the font button.
		void setFontButtonLabel();

		//! Writes all user settings to hard disk.
		void writeSettings();

		//! Reads all user settings.
		void readSettings();

		QLabel *labelTickerFont;
		QLabel *labelTickerSpeed;
		QLabel *labelDatabasePath;
		QLabel *labelDatabaseExplain;
		QLineEdit *lineDatabasePath;
		QSpinBox *spinTickerSpeed;
		QPushButton *buttonSave;
		QPushButton *buttonCancel;
		QGroupBox *groupKeyboardLayout;
		QGroupBox *groupTickerFont;
		QGroupBox *groupUserReset;
		QGroupBox *groupDatabase;
		QGroupBox *groupDialogCheck;
		QRadioButton *radioLayoutWin;
		QRadioButton *radioLayoutMac;
		QRadioButton *radioLayoutNeo;
		QRadioButton *radioLayoutDvorak;
		QRadioButton *radioLayoutRistome;
		QPushButton *buttonLessonsReset;
		QPushButton *buttonCharsReset;
		QPushButton *buttonSetFont;
		QPushButton *buttonDatabasePath;
		QCheckBox *checkIllustration;
		QFont tickerFont;
		QString currentDatabasePath;
};

#endif //SETTINGSDIALOG_H
