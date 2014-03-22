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

#include "UIGameSetup.h"
#include "Utility.h"

UIGameSetup::UIGameSetup( const QString& gameName, const QString& gameDisplayName, const QIcon& gameIcon, QWidget *parent ) 
	:QDialog( parent ),m_gameName(gameName)
{
	setupUi(this);
	
	setWindowTitle( gameDisplayName );
	setWindowIcon( gameIcon );
	//labelBreak->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	//labelSpeed->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	labelBetween->setAlignment(Qt::AlignCenter);
	
	quint8 	speed;
	bool	refuseSameIp;
	quint8	breakRate;
	qint32	minScore;
	qint32	maxScore;
	quint16 maxUnwelcome;
	
	GetSettingGameSetup( gameName, speed, refuseSameIp, breakRate, minScore, maxScore, maxUnwelcome );
	
	sbSpeed->setValue( speed );
	cbRefuseSameIp->setChecked( refuseSameIp );
	sbBreak->setValue( breakRate );
	sbRangeMin->setValue( minScore );
	sbRangeMax->setValue( maxScore );
	sbUnwelcome->setValue( maxUnwelcome );
	
	adjustSize();
}
UIGameSetup::~UIGameSetup()
{
	djDebug() << "DJLogin desstructor";
}
void UIGameSetup::on_okButton_clicked()
{
	accept();
	SetSettingGameSetup( m_gameName, sbSpeed->value(), cbRefuseSameIp->isChecked(), sbBreak->value(), sbRangeMin->value(), sbRangeMax->value(), sbUnwelcome->value() );
}
