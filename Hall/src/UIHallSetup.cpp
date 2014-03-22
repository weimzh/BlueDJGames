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

#include "UIHallSetup.h"
#include "Utility.h"
#include "HallUtility.h"
#include "DJGlobal.h"

extern LocaleInfo supportedLocaleInfos[MAX_SUPPORTED_LOCALE];

UIHallSetup::UIHallSetup(QWidget *parent) 
	:QDialog( parent )
{
	setupUi(this);
	
	cbMultiLogin->hide();
	
	QValidator *validator = new QIntValidator(0, 65535, this);
	lePort->setValidator( validator );
	
	connect(leServer, SIGNAL(editingFinished()), SLOT(serverFinished()));
	connect(lePort, SIGNAL(editingFinished()), SLOT(portFinished()));
	
	djDebug()<<"uihallsetup"<< supportedLocaleInfos[1].languageName;
	DebugBuffer( supportedLocaleInfos[1].languageName.toUtf8() );
	
    for ( uint i = 0; i < MAX_SUPPORTED_LOCALE; i++ ) {
    	QString languageName	= supportedLocaleInfos[i].languageName;
    	QString localeName	= supportedLocaleInfos[i].localeName;
		comboLang->addItem( languageName, localeName );
	}
    
    //get value from settings
    spinMaxSize->setValue( GetSettingMaxImageSize() );
    int min,max;
    GetSettingMinMaxSentences(min,max);
    spinMinSentences->setValue( min );
    spinMaxSentences->setValue( max );
    cbMultiLogin->setChecked( GetSettingMultiLogin() );
    cbSound->setChecked( GetSettingSound() );
    cbSaveChat->setChecked( GetSettingSaveChat() );
    cbPopupChat->setChecked( GetSettingPopupChat() );
    
	QString localeName	= GetSettingLocaleName();
	localeName 	= MostAppropriateSupportedLocaleName( localeName );
	djDebug()<<"MostAppropriateSupportedLocaleName"<<localeName;
	
	int index	= comboLang->findData(localeName);
	if ( -1 == index ) {
		index	= 0;
	}
	comboLang->setCurrentIndex(index);
	
	QString	username,password;
	GetSettingProxyAuth( username,password ); 
	leUsername->setText(username);
	lePassword->setText(password);
		
	comboProxyType->setItemData( 0, GetSettingProxy(0) );
	comboProxyType->setItemData( 1, GetSettingProxy(1) );
	comboProxyType->setItemData( 2, GetSettingProxy(2) );
	
	index	= GetSettingProxyType();
	djDebug() << "GetSettingProxyType" << index;
	if ( -1 == index )
		index	= 0;
	comboProxyType->setCurrentIndex( index );
	on_comboProxyType_activated( index );
	
	gbProxy->setChecked( GetSettingUseProxy() );
}
UIHallSetup::~UIHallSetup()
{
	djDebug() << "UIHallSetup destructor";
}
void UIHallSetup::on_pbFont_clicked()
{
	bool ok;
    QFont font = QFontDialog::getFont( &ok, QApplication::font(), this );
    if ( ok ) {
    	QApplication::setFont( font );
    	SetSettingApplicationFont( font );
    }
}
void UIHallSetup::on_okButton_clicked()
{
	djDebug() << "on_okButton_clicked";
	accept();
	
	SetSettingMaxImageSize( spinMaxSize->value() );
	SetSettingMinMaxSentences( spinMinSentences->value(), spinMaxSentences->value() );
	SetSettingMultiLogin( cbMultiLogin->isChecked() );
	SetSettingSound( cbSound->isChecked() );
	SetSettingSaveChat( cbSaveChat->isChecked() );
	SetSettingPopupChat( cbPopupChat->isChecked() );
	SetSettingLanguage( comboLang->itemData(comboLang->currentIndex()) );
	SetSettingUseProxy( gbProxy->isChecked() );
	SetSettingProxyType( comboProxyType->currentIndex() );
	for ( int i = 0; i < 3; i++ ) {
		QVariant	data	= comboProxyType->itemData(i);
		SetSettingProxy( i, data );
	}
	SetSettingProxyAuth(leUsername->text(),lePassword->text());
}

void UIHallSetup::on_cancelButton_clicked()
{
	reject();
}
void UIHallSetup::on_comboProxyType_activated( int index )
{
	djDebug() << "on_comboProxyType_activated" << index;
	QList<QVariant>	data	= comboProxyType->itemData(index).toList();
	
	QString	server	= data.takeFirst().toString();
	QString	port	= data.takeFirst().toString();
	
	leServer->setText( server );
	lePort->setText( port );
	
	/*
	if ( 0 == index ) {
		leUsername->setEnabled(true);
		lePassword->setEnabled(true);
	}else {
		leUsername->setEnabled(false);
		lePassword->setEnabled(false);
	}
	*/
}
void UIHallSetup::on_spinMinSentences_editingFinished()
{
	int minValue	= spinMinSentences->value();
	int maxValue	= spinMaxSentences->value();
	
	if ( minValue > maxValue )
		spinMinSentences->setValue( maxValue );
}
void UIHallSetup::on_spinMaxSentences_editingFinished()
{
	int minValue	= spinMinSentences->value();
	int maxValue	= spinMaxSentences->value();
	
	if ( maxValue < minValue )
		spinMaxSentences->setValue( minValue );
}
void UIHallSetup::serverFinished()
{
	djDebug() << "serverFinished";
	int index	= comboProxyType->currentIndex();
	QList<QVariant> data	= comboProxyType->itemData(index).toList();
	data[0]	= leServer->text();
	comboProxyType->setItemData(index, data);
}
void UIHallSetup::portFinished()
{
	djDebug() << "portFinished";
	int index	= comboProxyType->currentIndex();
	QList<QVariant> data	= comboProxyType->itemData(index).toList();
	data[1]	= lePort->text();
	comboProxyType->setItemData(index, data);
}
