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

#include "UILogin.h"
#include "LaunchUrl.h"
#include "Utility.h"
#include "HallUtility.h"
#include "DJGlobal.h"

static	QString		WWW_HOST_REGISTER_URL	= QString(HTTP_WWW_HOST) + "register.php";

static const char*	KEY_LOGINS	= "Logins";

static const char*	KEY_USERNAME	= "UserName";
static const char*	KEY_PASSWORD	= "Password";

UILogin::UILogin(QWidget *parent) 
	:QDialog( parent )
{
	setupUi(this);
	vboxLayout->setSpacing(20);
	vboxLayout->setSpacing(20);
	
	labelName->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	labelPassword->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	
	QSettings loginsSettings;
	loginsSettings.beginGroup( KEY_HALL );
	
    int size = loginsSettings.beginReadArray( KEY_LOGINS );
    djDebug() << "UILogin constructor" << "size = " << size;
    for ( int i = 0; i < size; ++i ) {
        loginsSettings.setArrayIndex(i);
        QString	userName	= loginsSettings.value( KEY_USERNAME ).toString();
        QString password	= QString::fromUtf8( crypt(loginsSettings.value( KEY_PASSWORD ).toByteArray()) );
        comboName->addItem( userName, password );
    }
    loginsSettings.endArray();
    loginsSettings.endGroup();
    
    comboName->setCurrentIndex( 0 );
    
    //set attribute
    on_comboName_activated(0);
    //exclusive check
	on_cbSaveName_clicked( cbSaveName->isChecked() );
}
UILogin::~UILogin()
{
	djDebug() << "UILogin desstructor";
}
/*
void UILogin::on_btnRemove_clicked()
{
	//int index	= comboName->currentIndex();
	comboName->removeItem( comboName->currentIndex() );
	on_comboName_activated( comboName->currentIndex() );
}
*/
void UILogin::on_btnLogin_clicked()
{
	djDebug() << "login button clicked";
	
	m_useName	= comboName->currentText().trimmed();
	m_password	= lePassword->text();
	
	if ( m_useName.isEmpty() )
		reject();
	int index	= comboName->findText( m_useName );
	if ( -1 == index ) {
		//new user name
		if ( cbSaveName->isChecked() ) {
			//save username
			if ( cbSavePassword->isChecked() )
				comboName->insertItem( 0, m_useName, m_password );
			else {
				comboName->insertItem( 0, m_useName, QString() );
			}
		}else {
			//don't save username
			accept();
		}
	}else {
		//exsting user name
		if ( cbSaveName->isChecked() ) {
			//save username
			if ( cbSavePassword->isChecked() )
				comboName->setItemData( index, m_password );
			else {
				comboName->setItemData( index, QString() );
			}
		}else {
			//don't save username
			comboName->removeItem( index );
		}
	}
	
	QSettings loginsSettings;
	loginsSettings.beginGroup( KEY_HALL );
	
	loginsSettings.beginWriteArray( KEY_LOGINS );
	index	= 1;
	for ( int i = 0; i < comboName->count(); i++ ) {
		QString userName	= comboName->itemText( i );
		QString password	= comboName->itemData( i ).toString();
		if ( userName == comboName->currentText() )
			loginsSettings.setArrayIndex(0);
		else
			loginsSettings.setArrayIndex(index++);
   		loginsSettings.setValue( KEY_USERNAME, userName );
       	loginsSettings.setValue( KEY_PASSWORD, crypt(password.toUtf8()) );
	}
	loginsSettings.endArray();
	loginsSettings.endGroup();
		
	accept();
}
void UILogin::on_btnRegister_clicked()
{
	LaunchURL( WWW_HOST_REGISTER_URL );	
}
void UILogin::on_cbSaveName_clicked( bool checked )
{
	if ( checked ) {
		cbSavePassword->setEnabled( true );
	}else {
		cbSavePassword->setChecked( false );
		cbSavePassword->setEnabled( false );
	}
}
void UILogin::on_comboName_activated( int index )
{
	djDebug() << "on_comboName_activated" << index;
	QString curUsername	= comboName->itemText(index);
    QString curPassword	= comboName->itemData(index).toString();
    
	lePassword->setText( curPassword );
	
	cbSaveName->setChecked( true );
    cbSavePassword->setChecked( !curPassword.isEmpty() );
}
QString UILogin::loginUserName() const
{
	return m_useName;
}
QString UILogin::loginPassword() const
{
	return m_password;
}
