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

#ifndef GDMAHJONG_H
#define GDMAHJONG_H

#include<QtGui>
#include <Qt3Support>
#include "DJBaseGlobal.h"
#include "DJGamePanel.h"
#include "DJGameMJDesktop.h"

#include "protocol.h"
#include "mjrule.h"
#include "mj2profile.h"

class GDMahjongPanel;
class DJGamePlayerItem;	
class DJToolButton;

class GDMahjongDesktop : public DJGameMJDesktop
{
	Q_OBJECT
	public :
		GDMahjongDesktop(QWidget* parent,DJGamePanel *panel);
		virtual ~GDMahjongDesktop();
		
		virtual QString	playerItemNameSuffix( DJGameUser* user );
        virtual void gameInfo( const unsigned char *buf );
		virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
		virtual void gameTrace( const GeneralGameTrace2Head *gameTrace );
		virtual QString bonusTarget( const DJGameBonus* bonus );
		void StaticInitDesktop();
		void StaticGameTrace(GeneralGameTrace2Head *gameTrace);
		void RepaintCurrentStatus();
		
		void MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe);
		quint8 GetMahjongThrowTraceID(quint8 card);
	
		void Hu(unsigned char chSite,unsigned char *data,unsigned char chDataLen);
		void DisplayHu(unsigned char chSite,MJResult2 *presult);
		const MahjongCurrent& mahjongCurrent() const;
	protected:
		virtual bool handleItemClicked(Qt::MouseButton btn,const QPoint& pos,Q3CanvasItem * item);
		virtual void handleMatrixChanged( const QMatrix& matrix );
	protected slots :
		void PlayerStarted();
		void ClickStyle();
		void ClickQi();
		void ClickChu();
		void ClickChi();
		void ClickPeng();
		void ClickGang();
		void ClickTing();
		void ClickHu();
	private :
		MahjongCurrent 	c_current;
		GDMahjongPanel * c_MahjongPanel;
		quint8 			m_lastMGangCard;
		QWidget			*m_toolbar;
#ifdef DJ_OEM
		DJToolButton 	*m_qi,*m_peng,*m_chu,*m_gang,*m_hu,*m_style;
#else		
		QToolButton 	*m_qi,*m_peng,*m_chu,*m_gang,*m_hu,*m_style;
#endif		
		bool c_bReqTing,m_bTingAutoGang,m_bTingZimo;
};

class  GDMahjongPanel : public DJGamePanel
{
	Q_OBJECT
	public : 
		GDMahjongPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent = 0, Qt::WFlags f = Qt::Window);
        ~GDMahjongPanel();
			
		virtual void playerItemHeaderSections( QList<int> &sectionIds, QStringList &sectionNames );
		virtual DJGamePlayerItem* addPlayerItem( quint32 userId, bool isPlayer );
};

#endif
