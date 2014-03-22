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

#ifndef DDZDESKTOPCONTROLLER_H
#define DDZDESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopPokerController.h"
#include "ddzprofile.h"

class DJPanelController;
class DJGraphicsTextItem;
class DJGraphicsPixmapItem;
class DJPoker;
class DJPokerPattern;

class DDZCallButton : public QPushButton
{
	Q_OBJECT
public :
	DDZCallButton( QWidget * parent = 0 );
	DDZCallButton( const QString & text, QWidget * parent = 0 );
	DDZCallButton( const QIcon & icon, const QString & text, QWidget * parent = 0 );
	~DDZCallButton();
	
	int score() const { return m_score; }
	void setScore( int score );
public slots:
	void handleClicked();
signals:
	void scoreCalled( int score );
private:
	void init();
	int			m_score;
};
class DDZDesktopController : public DJDesktopPokerController
{
	Q_OBJECT
public :
	DDZDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~DDZDesktopController();
	
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual void initUnderGameInfo( const QByteArray& buf );
	virtual int rankOnPoint( const DJPoker& poker ) const;
	virtual void repaintAll();
	virtual void repaintCurrentStatus();
	virtual void repaintHost();
	void repaintDizhu();
	void repaintBombScore();
	virtual void locateAll();
	virtual void locateAllNonMatrimoveItems();
	virtual void locateEmotionPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns );
	void repaintEmotion( quint8 seat, const QPixmap& pix );
	void hideEmotion( quint8 seat );
	virtual void throwSelectedCards();
	virtual QByteArray createThrowTrace( DesktopItems& items );
	virtual bool isChuEnabled() const;
	bool isQiEnabled() const;
	virtual void fillPlayerInfo();
	virtual QString	userItemNameSuffix( DJGameUser* user ) const;
	virtual QString bonusTarget( const DJGameBonus* bonus );
	virtual DJPoker higherSerialPoker( const DJPoker& poker, bool isStrict ) const;
	virtual DJPoker lowerSerialPoker( const DJPoker& poker, bool isStrict ) const;
public slots:
	void handleScoreCalled( int score );
private slots:
	void clickTip();
	void clickThrow();
	void clickArrange();
	void clickPass();
private:
	QPixmap createCallPixmap( int score );
	bool isFriend( quint8 seat1, quint8 seat2 ) const;
	DJPoker greaterPoker( const DJPoker& poker ) const;
	QList<DJPoker> findMajor( const QHash<DJPoker,int>& countPokers,
									const DJPoker& otherKeyPoker, 
									int width, int length, 
									QSet<DJPoker>& excluder ) const;	
	QList<DJPoker> findMinor( const QHash<DJPoker,int>& countPokers,
									int width, int length, 
									QSet<DJPoker>& excluder ) const;
	DoudzhuCurrent m_current;
	quint8	m_seatOfLastCall,m_seatOfLastThrow;
	quint8 	m_scoreOfLastCall;
	
	QList<DJGraphicsPixmapItem*>	m_emotionItems;
	QList<QPoint>					m_emotionPoints;
	QList<Qt::Alignment>			m_emotionAligns;
		
	QList<DDZCallButton*>			m_callButtons;
	
	QWidget			*m_actionContainer;
	QToolButton		*m_throw,*m_arrange,*m_pass,*m_tip;
	
	DJGraphicsTextItem 		*m_dizhuText,*m_bombText,*m_scoreText;
	int				m_bombs,m_score;
	quint8			m_lastThrowType;
	int				m_styleOfArrange;
	
	QList<DJPoker>		m_tipHandPokers;
	QHash<DJPoker,int>	m_tipCountPokers;
	QSet<DJPoker>		m_tipMajorExcluder;
	QSet<DJPoker>		m_tipBombExcluder;
	bool				m_tipRocket;
};

#endif
