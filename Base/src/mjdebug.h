#ifndef __MJDEBUG_H__
#define __MJDEBUG_H__

#include <QtGui>
#include "DJGlobal.h"
#include "mjrule.h"
#include "mjfans.h"
#include "mjtype.h"

void PrintFanResult( const MahjongFanResult& fanResult );
void PrintFanResults( const MJFanResults& fanResults );
void PrintCardWrap( PMahjongCardWrap pCardWrap );
void PrintHuTypeWrap( PMahjongHUTypeWrap pHuTypeWrap );

void PrintMJCard( MJCard card );
void PrintMJSourceCard( MJSourceCard sourceCard );
void PrintMJCards( const MJCards mjcards );
void PrintMJSourceCards( const MJSourceCards& sourceCards ) ;
void PrintMJCardsGroups( const MJCardsGroups& groups );
#endif
