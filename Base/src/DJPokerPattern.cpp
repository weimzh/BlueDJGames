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

#include "DJPokerPattern.h"
#include "DJDesktopPokerController.h"

DJPokerPattern::DJPokerPattern(  int width, int length, 
					const DJPoker& keyPoker, const QList<DJPoker>& pokers )
{
	m_width		= width;
	m_length	= length;
	m_keyPoker	= keyPoker;
	m_pokers	= pokers;
}
QList<DJPokerPattern> DJPokerPattern::fromPokers( DJDesktopPokerController* desktop,
								const QList<DJPoker>& thePokers, bool isStrictSuit )
{
	QList<DJPokerPattern> patterns;
	
	QList<DJPoker> handPokers	= thePokers;
	int	width	= handPokers.size();
	while( width > 0 && !handPokers.isEmpty() ) {
		QHash<DJPoker,int> countPokers	= desktop->createCountPokers( handPokers );
	
		DJPoker	keyPoker;
		QList<DJPoker> patternPokers;
		QSet<DJPoker>	keExcluder;

		QList<DJPoker> group	= desktop->iterateGroup( countPokers, 
													width, keyPoker, keExcluder,
													isStrictSuit, false );
		if ( group.isEmpty() ) {
			width--;
		}else {
			//keep width, until empty
    		//found group and try to find shun contains this group
    		patternPokers << group;
    		DJPoker	lowestKeyPoker,highestKeyPoker; 
    		DJPoker currentKeyPoker	= keyPoker;
    		
    		djDebug()<<"current group"<<keyPoker.description();
    		//find lowest group of shun from current key
    		while( true ) {
        		DJPoker nextKeyPoker;
        		QList<DJPoker> nextGroup	= desktop->findNextGroup( countPokers, 
        														currentKeyPoker,
        														nextKeyPoker,
        														width, false,
        														isStrictSuit, false );
        		if ( nextGroup.isEmpty() ) {
        			lowestKeyPoker	= currentKeyPoker;
        			break;
        		}
        		//djDebug()<<"find lower next group"<<currentKeyPoker.description()<<nextKeyPoker.description();
        		foreach( DJPoker poker, nextGroup ) {
        			patternPokers.prepend(poker);
        		}
        		currentKeyPoker	= nextKeyPoker;
        		if ( desktop->isLowestSerialPoker( currentKeyPoker ) ) {
        			lowestKeyPoker	= currentKeyPoker;
        			break;
        		}
    		}
    		//find highest group of shun from current key
    		currentKeyPoker	= keyPoker;
    		while( true ) {
        		DJPoker nextKeyPoker;
        		QList<DJPoker> nextGroup	= desktop->findNextGroup( countPokers, 
        														currentKeyPoker,
        														nextKeyPoker,
        														width, true,
        														isStrictSuit, false );
        		if ( nextGroup.isEmpty() ) {
        			highestKeyPoker	= currentKeyPoker;
        			break;
        		}
        		//djDebug()<<"find higher next group"<<currentKeyPoker.description()<<nextKeyPoker.description();
        		foreach( DJPoker poker, nextGroup ) {
        			patternPokers.append(poker);
        		}
        		currentKeyPoker	= nextKeyPoker;
        		if ( desktop->isHighestSerialPoker( currentKeyPoker ) ) {
        			highestKeyPoker	= currentKeyPoker;
        			break;
        		}
    		}
    		//djDebug()<<"lowestKeyPoker"<<lowestKeyPoker.description()<<"highestKeyPoker"<<highestKeyPoker.description();
    		patterns << DJPokerPattern(width,patternPokers.size()/width,lowestKeyPoker,patternPokers);
			desktop->removeSubset( handPokers, patternPokers );
		}
	}
	qSort( patterns.begin(), patterns.end(), qGreater<DJPokerPattern>() );
	return patterns;
}
bool DJPokerPattern::operator< ( const DJPokerPattern & other ) const
{
	if ( m_width == other.m_width )
		return m_length < other.m_length;
	else
		return  m_width < other.m_width;
}
QList<DJPoker> DJPokerPattern::takeLength( int length )
{
	if ( length > m_length )
		return QList<DJPoker>();
		
	QList<DJPoker> pokers;
	for (int i = 0; i < length; ++i ) {
		for ( int j = 0; j < m_width; ++j ) {
			if ( !m_pokers.isEmpty() )
				pokers << m_pokers.takeLast();
		}
	}
	m_length-= length;
	return pokers;
}
QString DJPokerPattern::description() const
{
	QString result	= QString("PATTERN: width %1 length %2 keypoker %3")
												.arg(m_width)
												.arg(m_length)
												.arg(m_keyPoker.description());
	QString pokersDesc;
	foreach( DJPoker poker, m_pokers ) {
		pokersDesc += "\n";
		pokersDesc += poker.description();
	}
	result += pokersDesc;
	return result;
}
