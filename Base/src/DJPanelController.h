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

#ifndef DJPANELCONTROLLER_H
#define DJPANELCONTROLLER_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "protocol.h"

class DJGameController;
class DJTableController;
class DJDesktopController;
class DJTreeWidget;
class DJShowView;
class DJClientRequest;
class DJPanel;
class DJGameUser;
class DJPanelUserItem;
class DJGroupChat;
class DJToolButton;
class DJGameRoom;

class DJ_BASE_EXPORT DJPanelController : public QObject
{
    Q_OBJECT 
public:
	enum PlayerWidgetSectionId{
    	UserName,
    	NickName,
    	Seat,
    	PanelScore,
    	Score,
    	PanelChips,
    	Chips,
    	DoorWind,
    	RingWind,
    	LastSectionId
    };
    DJPanelController( DJTableController* tableController, quint8 seatId );
    virtual ~DJPanelController();
    
    virtual void gameInfo( const QByteArray& buf );
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout );
	virtual void gameTrace( const GeneralGameTrace2Head *gameTrace );
    virtual void magicResult( const MagicResult* magicResult );
    virtual void gameOver( const DJGameOver* gameOver );
    virtual void bonusCondition( const DJGameBonus* condition );
	virtual void bonusFulfilled( const DJGameBonusFulfill* fulfill );
	
    void createPanel( const QSize& desktopSize );
    DJPanel* panel() const { return m_panel; }
    virtual void createAccessoryWidgets( QWidget *parent );
    virtual void createDesktopWidgets( const QSize& desktopSize, QWidget *parent );
    
    void createUsersWidget( QWidget* parent );
    void createShowWidget( QWidget* parent );
	void createChatWidget( QWidget* parent );
	
	void createBarWidget( QWidget* parent );
    virtual DJDesktopController* createDesktopController( const QSize& desktopSize, QWidget *parent );
    
    DJDesktopController* desktopController() const { return m_desktopController; }
    void closeChildDialog();
	void removePendingAcl();
	
	void showPanel();
	quint32 selfUserId() const;
	quint8 tableId() const;
	quint8 seatId() const { return m_seatId; }
	int numberOfSeats() const;
	const QList<DJGameUser*>& players() const { return m_players; }
	const QList<DJGameUser*>& lookers() const { return m_lookers; }
	DJGameUser *player( quint8 seatId ) const;
	void userArrived( DJGameUser* user );
	void resetPlayers();
	DJGameController* gameController() const;
	DJGameRoom* gameRoom() const;
	
	DJGroupChat* chatWidget() const { return m_chatWidget; }
	bool isLookingOn() const;
	bool isPlayer( DJGameUser *user ) const;
	void receiveChat( const ChatContent *chatContent );
	void insertText2Editor( const QString& text );
	void insertText2Browser( const QString& text );
	void insertGameResult2Browser( const QString& text );
	void deltaUserData( DJGameUser *user, int value );
	QByteArray makeGameData( quint8 subType, const QByteArray& gameData = QByteArray(), quint8 type = DJGAME_PROTOCOL_TYPE_SUBMIT_GAMEDATA );
	bool sendRequest( const DJClientRequest& request, bool isDiscardable = false );
	
	void removeAllPlayerItems();
	void repaintAllPlayerItems();
	
	virtual DJPanelUserItem* addUserItem( DJGameUser* user, bool isPlayer );
	void removeUserItem( DJGameUser* user );
	DJPanelUserItem*	findUserItem( DJGameUser* user );
	
	int indexOfSection( int sectionId ) const;
	QString	sectionName( int sectionId ) const;
	QString baseSectionName( int sectionId );
	virtual DJPanelUserItem* createUserItem( DJGameUser *user, QTreeWidget* parent, int type = QTreeWidgetItem::Type );
	virtual void createUserWidgetHeader( QTreeWidget* treeWidget );
	virtual void userWidgetHeaderSections( QList<int> &sectionIds, QStringList &sectionNames );
	virtual QString	userItemNameSuffix( DJGameUser* user ) const;
	virtual void lookerInOut( DJGameUser *looker, bool in );
	
	void cardUsed( const ReqUseCard *useCard );
public slots:
	virtual void showUserContextMenu( DJGameUser *user );
	void handleLogoClicked();
	//void handleColorClicked();
	void handleHelpClicked();
	void handleUserItemDoubleClicked(QTreeWidgetItem * item, int column);
	
	void requestPrivateView( DJGameUser *host );
	void acceptPrivateView( DJGameUser *looker );
	void closePrivateView( DJGameUser *looker );
	void handleUsersWidgetContext( const QPoint& pos );
	void handleAcceptPrivateView( quint32 userId );
signals:
	void changeDesktopBrush( const QBrush& brush );	
private:
	DJTableController		*m_tableController;
	QList<DJGameUser*>		m_players;
	QList<DJGameUser*>		m_lookers;
	quint8					m_seatId;
	
	DJPanel					*m_panel;
	QList<void*>			m_acls;
	
	QWidget					*m_bar;
	
	DJTreeWidget			*m_usersWidget;
	DJShowView				*m_showView;
	DJGroupChat				*m_chatWidget;
	DJDesktopController		*m_desktopController;
	
	QList<int>				m_sectionIds;
	QStringList				m_sectionNames;
	
	DJToolButton			*m_logo,*m_color,*m_help;
	
	QMenu					*m_playerContextMenu;
	QAction					*m_actRequestPrivateView,*m_actAcceptPrivateView,*m_actClosePrivateView;
};
#endif
