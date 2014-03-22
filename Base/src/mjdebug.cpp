#include <algorithm>
#include "mjdebug.h"

void PrintFanResult( const MahjongFanResult& fanResult )
{
	djDebug()<<"fanID="<<fanResult.chFanID<<"fans="<<fanResult.iFan;
}
void PrintFanResults( const MJFanResults& fanResults )
{
	for (unsigned int i = 0; i < fanResults.size(); ++i ) {
		PrintFanResult( fanResults[i] );
	}
}
void PrintCardWrap( PMahjongCardWrap pCardWrap )
{
	djDebug()<<"PrintCardWrap";
	if ( pCardWrap  ) {
		djDebug()<<hex<<"chExist="<<pCardWrap->chExist
					<<"CardWrapName="<<pCardWrap->chCardWrapName
					<<"chPages="<<pCardWrap->chPages
					<<"chClass="<<pCardWrap->chClass;
		
		djDebug()<<hex<<pCardWrap->chWrap[0]
					<<pCardWrap->chWrap[1]
					<<pCardWrap->chWrap[2]
					<<pCardWrap->chWrap[3];
  	}
}
void PrintHuTypeWrap( PMahjongHUTypeWrap pHuTypeWrap )
{ 
	djDebug()<<"PrintHuTypeWrap";
	
	while ( pHuTypeWrap ) {
		djDebug()<<hex<<"HuType="<<pHuTypeWrap->chHuType;
		for (int i = 0; i < 9; ++i ) {
			PrintCardWrap ( &pHuTypeWrap->cardwrap[i] );
		}
		pHuTypeWrap = pHuTypeWrap->pnext;
	}
}
void PrintMJCard( MJCard card )
{ 
	if ( 0 == card )
		djDebug()<<"--- ";
	else if ( MAHJONG_ISERRORCARD(card) )
		djDebug()<<"XXX ";
	else {
		unsigned char cardClass	= GET_MAHJONG_CARDCLASS(card);
		if ( MAHJONG_CARD_WAN == cardClass )
			djDebug()<<hex<<GET_MAHJONG_CARDVALUE(card)<<"W";
		else if ( MAHJONG_CARD_TONG == cardClass )
			djDebug()<<hex<<GET_MAHJONG_CARDVALUE(card)<<"B";
		else if ( MAHJONG_CARD_TIAO == cardClass )
			djDebug()<<hex<<GET_MAHJONG_CARDVALUE(card)<<"T";
		else
			djDebug()<<hex<<GET_MAHJONG_CARDVALUE(card)<<"F";
	}
}
void PrintMJSourceCard( MJSourceCard sourceCard )
{
	unsigned char source	= sourceCard >> 8;
	MJCard	card	= sourceCard & 0xFF;
	
	if ( source & SOURCE_MASK_MO ) {
		djDebug()<<"z";
	}
	if ( source & SOURCE_MASK_CHI ) {
		djDebug()<<"c";
	}
	if ( source & SOURCE_MASK_PENG ) {
		djDebug()<<"p";
	}
	PrintMJCard(card);
}
void PrintMJCards( const MJCards mjcards )
{ 
	if ( mjcards.empty() ) {
		djDebug()<<"empty MJCards";
		return;
	}
	for_each( mjcards.begin(), mjcards.end(), PrintMJCard );
}
void PrintMJSourceCards( const MJSourceCards& sourceCards )
{
	if ( sourceCards.empty() ) {
		djDebug()<<"empty MJSourceCards";
		return;
	}
	for_each( sourceCards.begin(), sourceCards.end(), PrintMJSourceCard );
}
void PrintMJCardsGroups( const MJCardsGroups& groups )
{ 
	djDebug()<<"PrintMJCardsGroups";
	
	if ( groups.empty() ) {
		djDebug()<<"empty mjcardsgroups";
		return;
	}
	for ( unsigned int i = 0; i < groups.size(); i++ ) {
		MJCards mjcards	= groups[i];
		PrintMJCards( mjcards );
 	}
}
