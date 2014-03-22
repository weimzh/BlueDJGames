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

#include <QtCore>
#include "DJUpdateHandler.h"
#include "DJGlobal.h"

#ifdef DJ_DEBUG_OUTPUT
void myMessageOutput( QtMsgType type, const char *msg )
{
	static char *logFileName = "UpdateHall.txt";
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

int main( int argc, char ** argv )
{
#ifdef DJ_DEBUG_OUTPUT	
	qInstallMsgHandler( myMessageOutput );
#endif

	QStringList arguments;
	djDebug("argc=%d",argc);
	for (int i = 1;i< argc;i++) {
		arguments << QString(argv[i]);
		djDebug() << QString(argv[i]);
	}
	
	QCoreApplication	app(argc,argv);
	
	DJUpdateHandler	handler( arguments );
	return app.exec();
}
