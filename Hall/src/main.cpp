//****************************************************************************
//Copyright (C) 2005-2006 Beijing BlueDJ Technology Co.,Ltd. All rights reserved.

//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program (in the file LICENSE.GPL); if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

//Please visit http://www.bluedj.com for more infomation about us.
//Contact us at ggwizard@gmail.com or darkdong@gmail.com.
//****************************************************************************/

#include <QtGui>

#include "DJHallController.h"
#include "DJStyle.h"
#include "DJMainWindow.h"
#include "DJHallGlobal.h"
#include "Utility.h"
#include "HallUtility.h"

#ifdef Q_WS_X11
#include "DJApplication.h"
#endif

#if defined(Q_WS_WIN)
#include <windows.h>
HANDLE	gMutexHandle	= 0;
#endif

#ifdef DJ_DEBUG_OUTPUT
void myMessageOutput( QtMsgType type, const char *msg )
{
	static char *logFileName = "log.txt";
	static FILE	*logFile = NULL;

	if ( NULL == logFile ) {
		if (logFile = fopen(logFileName,"w") ) {
			fprintf(logFile,"%s\n",msg);
			fclose(logFile);
		}
	} else{
		if (logFile = fopen(logFileName,"a") ) {
			fprintf(logFile,"%s\n",msg);
			fclose(logFile);
		}	
	}
}
#endif

extern LocaleInfo supportedLocaleInfos[MAX_SUPPORTED_LOCALE];

int main(int argc, char *argv[])
{	
#ifdef DJ_DEBUG_OUTPUT	
	qInstallMsgHandler( myMessageOutput );
#endif

#if defined( Q_WS_WIN ) && defined( QT_NO_DEBUG ) && !defined( DJ_DEBUG_OUTPUT )	
	gMutexHandle = CreateMutex(0,true,(LPCTSTR)QString(APPLICATION_NAME).utf16());
	if ( ERROR_ALREADY_EXISTS == GetLastError() ) {
		exit(0);
	}
#endif

#ifdef Q_WS_X11	
	DJApplication	app(argc,argv);
#else
	QApplication	app(argc,argv);
#endif

	QCoreApplication::setOrganizationName( ORGANIZATION_NAME );
    QCoreApplication::setOrganizationDomain( ORGANIZATION_DOMAIN );
    QCoreApplication::setApplicationName( APPLICATION_NAME );

	QTranslator	hallTranslator;
	QString localeName	= FindSupportedLocaleName();
	djDebug()<<"localeName"<<localeName;
    hallTranslator.load( QString(":/HallRes/lang/Hall_") + localeName );
    QCoreApplication::installTranslator(&hallTranslator);
    
    QTranslator	qtTranslator;
    qtTranslator.load( QString(":/HallRes/lang/qt_") + localeName );
    QCoreApplication::installTranslator(&qtTranslator);
    
	QApplication::setStyle( new DJStyle(":/HallRes/image/style/") );
	QFont font	= GetSettingApplicationFont();
	QApplication::setFont( font );
	
	/*
	QTranslator translator;
    bool res	= translator.load( "LanguageNames");
    QString lang	= translator.translate( "LanguageNames", "zh_CN" );
    djDebug()<<"test translator"<<res<<lang;
    QByteArray buf	= lang.toUtf8();
    DebugBuffer( buf );
    
    const char* jtzw	= "\xe7\xae\x80\xe4\xbd\x93\xe4\xb8\xad\xe6\x96\x87";
    lang	= QString::fromUtf8( jtzw);
    	
	QFontDatabase base;
	QStringList list	= base.families( QFontDatabase::SimplifiedChinese );
	djDebug() << list;
	QLocale l(QLocale::English);
	djDebug()<<"localname="<<l.name()<<"lang string="<<l.languageToString(QLocale::Chinese);
	*/
		
	DJMainWindow	mainWindow;
#ifdef Q_WS_MAC
	mainWindow.setAttribute( Qt::WA_MacMetalStyle );
#endif
    DJHallController hallController( &mainWindow );
    mainWindow.showMaximized();
#ifdef Q_WS_X11	
	app.setMainWindow( &mainWindow );
#endif

	app.exec();
}
