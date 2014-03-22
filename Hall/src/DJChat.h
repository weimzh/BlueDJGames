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

#ifndef DJCHAT_H
#define DJCHAT_H

#include <QtGui>
class DJTextBrowser;

class DJTextEdit : public QTextEdit
{
	Q_OBJECT
public:
    DJTextEdit( QWidget * parent = 0 );
    virtual ~DJTextEdit();
signals:
	void returnPressed();
protected:
	virtual void keyPressEvent ( QKeyEvent * e );
};
class DJSymbol;
class DJContact;
class DJToolButton;

class DJChat :public QWidget
{
	Q_OBJECT
public:
    DJChat( quint32 selfUserId, QString selfUserName, quint32 otherUserId, QString otherUserName, DJContact* contact, QWidget* parent = 0, Qt::WFlags f = Qt::Window );
    virtual ~DJChat();
    
	void receiveChat( quint32 speakerId, const QString& contents );
	void saveToFile( const QByteArray& contents );
	
	void setChatEnabled( bool isEnabled );
	void showChatWidget();
	void setGameId( quint16 gameId );
	void setOtherUserName( const QString& otherUserName );
protected:
	virtual void closeEvent( QCloseEvent * e );
private slots:
	void sendChat();
	void toggleHistory();
	void clearHistory();
	void toggleSymbol();
	void insertPlayerSymbol( const QString& name );
	void insertMemberSymbol( const QString& name );
private:
	DJTextBrowser	*m_browser;
	DJTextEdit		*m_editor;
	DJTextBrowser	*m_history;
	DJSymbol		*m_symbolWidget;
	DJToolButton	*m_symbol, *m_send;
	
	quint32			m_selfUserId;
	QString			m_selfUserName;
	quint32			m_otherUserId;
	QString			m_otherUserName;
	QDir			m_selfChatDir;
	QString			m_chatFileName;
	DJContact		*m_contact;
	quint16 		m_gameId;
};
#endif

