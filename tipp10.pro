######################################################################
# Project file for TIPP 10
# Copyright (c) 2006-2011, Tom Thielicke IT Solutions
######################################################################

TEMPLATE         = 	app
# MAC-Version:
#ICON           =	img/tipp10.icns
#QMAKE_MAC_SDK  = /Developer/SDKs/MacOSX10.5.sdk
#CONFIG        +=  x86 ppc
TRANSLATIONS 	 =	languages/tipp10_en.ts \
                    languages/tipp10_de.ts
QMAKE_LFLAGS    -=  -mthreads
DEPENDPATH      += 	.
INCLUDEPATH     += 	.
CONFIG          += 	qt
QT              += 	sql
QT              += 	network
RC_FILE         += 	tipp10.rc
RESOURCES       += 	tipp10.qrc
HEADERS         += 	def/defines.h \
                    def/errordefines.h \
                    widget/mainwindow.h \
                    widget/startwidget.h \
                    widget/illustrationdialog.h \
                    widget/illustrationimage.h \
                    widget/trainingwidget.h \
                    widget/tickerboard.h \
                    widget/keyboard.h \
                    widget/numpad.h \
                    widget/statusbar.h \
                    widget/evaluationwidget.h \
                    widget/progressionwidget.h \
                    widget/fingerwidget.h \
                    widget/settingsdialog.h \
                    widget/settingspages.h \
                    widget/lessondialog.h \
                    widget/regexpdialog.h \
                    widget/downloaddialog.h \
                    widget/lessonprintdialog.h \
                    widget/lessonresult.h \
                    widget/updatedialog.h \
                    widget/helpbrowser.h \
                    widget/companylogo.h \
                    widget/errormessage.h \
                    widget/licensedialog.h \
                    widget/txtmessagedialog.h \
                    widget/checkversion.h \
                    sql/connection.h \
                    sql/lessontablesql.h \
                    sql/chartablesql.h \
                    sql/trainingsql.h \
                    sql/keyboardsql.h \
                    sql/startsql.h \
                    games/abcrainwidget.h \
                    games/charball.h
SOURCES         += 	main.cpp \
                    widget/mainwindow.cpp \
                    widget/startwidget.cpp \
                    widget/illustrationdialog.cpp \
                    widget/illustrationimage.cpp \
                    widget/trainingwidget.cpp \
                    widget/tickerboard.cpp \
                    widget/keyboard.cpp \
                    widget/numpad.cpp \
                    widget/statusbar.cpp \
                    widget/evaluationwidget.cpp \
                    widget/progressionwidget.cpp \
                    widget/fingerwidget.cpp \
                    widget/settingsdialog.cpp \
                    widget/settingspages.cpp \
                    widget/lessondialog.cpp \
                    widget/regexpdialog.cpp \
                    widget/downloaddialog.cpp \
                    widget/lessonprintdialog.cpp \
                    widget/lessonresult.cpp \
                    widget/updatedialog.cpp \
                    widget/helpbrowser.cpp \
                    widget/companylogo.cpp \
                    widget/errormessage.cpp \
                    widget/licensedialog.cpp \
                    widget/txtmessagedialog.cpp \
                    widget/checkversion.cpp \
                    sql/lessontablesql.cpp \
                    sql/chartablesql.cpp \
                    sql/trainingsql.cpp \
                    sql/keyboardsql.cpp \
                    sql/startsql.cpp \
                    games/abcrainwidget.cpp \
                    games/charball.cpp
