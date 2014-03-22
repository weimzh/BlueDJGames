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

#include "UISelectDownloadServer.h"
#include "HallUtility.h"
#include "DJGlobal.h"

UISelectDownloadServer::UISelectDownloadServer(QWidget *parent) 
	:QDialog( parent )
{
	setupUi(this);
	
	cbResume->setChecked( GetSettingResume() );
}
UISelectDownloadServer::~UISelectDownloadServer()
{
}
bool UISelectDownloadServer::selectDownloadServer( const QString& section )
{	
	QSettings updateSettings( LOCALFILE_UPDATE_INI, QSettings::IniFormat );
	
	updateSettings.beginGroup( section );
	
	int size = updateSettings.beginReadArray( UPDATE_INI_UPDATE );
	if ( 0 == size )
		return false;
    for ( int i = 0; i < size; ++i ) {
        updateSettings.setArrayIndex(i);
        QString http	= updateSettings.value(UPDATE_INI_UPDATE_URL).toString();
        QString name	= updateSettings.value(UPDATE_INI_UPDATE_SERVER).toString();
        comboServer->addItem( name, http );
    }
    updateSettings.endArray();
	updateSettings.endGroup();
	return true;
}
QString UISelectDownloadServer::url() const
{
	return comboServer->itemData(comboServer->currentIndex()).toString();
}
bool UISelectDownloadServer::resume() const
{
	return cbResume->isChecked();
}
void UISelectDownloadServer::on_okButton_clicked()
{
	djDebug() << "on_okButton_clicked";
	accept();
	
	SetSettingResume( cbResume->isChecked() );
}
