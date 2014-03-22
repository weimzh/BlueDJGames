#ifndef __DJGAME_MAHJONG_RULE_H__
#define __DJGAME_MAHJONG_RULE_H__

#define MAHJONG_RULE_STATUS_SUCCESS 		0
#define MAHJONG_RULE_STATUS_FAILED			1

#define DJGAME_MAHJONG_FAN_MASK_LENGTH		20	

typedef struct __tagMahjongRule
{
	unsigned char chChips;
	
	unsigned char chPlutus;
	unsigned char chGANG_FAN;
	unsigned char chOutFAN;
	unsigned char ch3PlutusFAN;
	
	unsigned char chSpecialGANG;
	
	unsigned char chProgressRule1;
	unsigned char chProgressRule2;
	
	unsigned char chHUMinTerm;
	unsigned char chHUMinTermFAN;
	unsigned char chHUBankerFAN; //==0xFF 's meaning is double
	unsigned char chHURule1;
	unsigned char chHURule2;
	
	unsigned char chRemainChips;
	unsigned char chNobodyHU;
	unsigned char chNextBanker;
	
	unsigned char chSupportFAN[DJGAME_MAHJONG_FAN_MASK_LENGTH];
//	unsigned char chFANs[160];
}MahjongRule,*PMahjongRule;

#define MAHJONG_RULE_DOUBLE_FAN				0xFF

#define MAHJONG_RULE_CHIPS_FENG			0x01
#define MAHJONG_RULE_CHIPS_XI			0x02
#define MAHJONG_RULE_CHIPS_4SEASON		0x04
#define MAHJONG_RULE_CHIPS_4FLOWER		0x08
#define MAHJONG_RULE_CHIPS_FORBID19		0x10
#define MAHJONG_RULE_CHIPS_SINGEFENG	0x20
#define MAHJONG_RULE_CHIPS_16CHIPS		0x40

#define MAHJONG_RULE_PLUTUS_FLOWERS		0x01
#define MAHJONG_RULE_PLUTUS_SHOW		0x02
#define MAHJONG_RULE_PLUTUS_SHOWUP		0x03
#define MAHJONG_RULE_PLUTUS_SHOWDOWN	0x04
#define MAHJONG_RULE_PLUTUS_ALLOWGANG	0x40
#define MAHJONG_RULE_PLUTUS_ALLOWOUT	0x80

#define MAHJONG_RULE_PROGRESS1_FORBID_EAT			0x01
#define MAHJONG_RULE_PROGRESS1_FORBID_XIPENG		0x02
#define MAHJONG_RULE_PROGRESS1_FORBID_OUTLAST		0x04
#define MAHJONG_RULE_PROGRESS1_FORBID_PAO			0x08
#define MAHJONG_RULE_PROGRESS1_FORBID_GANG			0x10
#define MAHJONG_RULE_PROGRESS1_FORBID_HAND1			0x20
#define MAHJONG_RULE_PROGRESS1_FORBID_OUTSAMEEAT	0x40
#define MAHJONG_RULE_PROGRESS1_AUTORENEW			0x80

#define MAHJONG_RULE_PROGRESS2_DIANPAOBAO			0x01
#define MAHJONG_RULE_PROGRESS2_ALLOW_MINGGANG		0x02
#define MAHJONG_RULE_PROGRESS2_ALLOW_DUOXIANG		0x04
#define MAHJONG_RULE_PROGRESS2_QING1SE_3KOUPEI		0x08
#define MAHJONG_RULE_PROGRESS2_4KOUPEI				0x10

#define MAHJONG_RULE_HU_MINTERM_FAN					0x01
#define MAHJONG_RULE_HU_MINTERM_QUANMEN				0x02
#define MAHJONG_RULE_HU_MINTERM_QUE1MEN				0x03
#define MAHJONG_RULE_HU_MINTERM_8CHIPS				0x04
					
#define MAHJONG_RULE_HU1_MUSTDIY					0x01
#define MAHJONG_RULE_HU1_INCLUDE_KE					0x02
#define MAHJONG_RULE_HU1_INCLUDE_SHUN				0x04
#define MAHJONG_RULE_HU1_INCLUDE_19					0x08
#define MAHJONG_RULE_HU1_INCLUDE_EATPENG			0x10
#define MAHJONG_RULE_HU1_ZHANIAO					0x20

#define MAHJONG_RULE_HUANG_RENEW					0x01
#define MAHJONG_RULE_HUANG_CHAJIAO					0x02
#define MAHJONG_RULE_HUANG_BANKERWIN				0x03
#define MAHJONG_RULE_NEXTBANKER_MUSTLUN				0x01
#define MAHJONG_RULE_NEXTBANKER_HULIAN				0x02
#define MAHJONG_RULE_NEXTBANKER_HUZUO				0x03


/////////////////////////////////////////////////////////////
//  card code
//     7       6       5      4      3      2      1      0     bit
// IsSeizor IsPlutus  [CardClass]   [  CARD Number / ZI     ]
/////////////////////////////////////////////////////////////

typedef struct __tagDJGameMahjongImage
{
	unsigned char chPage[0x40];
}DJGameMahjongImage,*PDJGameMahjongImage;

#define MAHJONG_CARD_ZI				0x00
#define MAHJONG_CARD_ZI_DONG			0x01
#define MAHJONG_CARD_ZI_NAN				0x02
#define MAHJONG_CARD_ZI_XI				0x03
#define MAHJONG_CARD_ZI_BEI				0x04
#define MAHJONG_CARD_ZI_ZHONG			0x05
#define MAHJONG_CARD_ZI_FA				0x06
#define MAHJONG_CARD_ZI_BAI				0x07
#define MAHJONG_CARD_ZI_SPRING			0x08
#define MAHJONG_CARD_ZI_SUMMER			0x09
#define MAHJONG_CARD_ZI_AUTUMN			0x0A
#define MAHJONG_CARD_ZI_WINTER			0x0B
#define MAHJONG_CARD_ZI_PLUM			0x0C
#define MAHJONG_CARD_ZI_ORCHID			0x0D
#define MAHJONG_CARD_ZI_BAMBOO			0x0E
#define MAHJONG_CARD_ZI_MUM				0x0F

#define MAHJONG_CARD_WAN			0x10
#define MAHJONG_CARD_TONG			0x20
#define MAHJONG_CARD_TIAO			0x30

#define MAHJONG_CARD_BACKGROUND			0x3B
#define MAHJONG_CARD_AN					0x3C

#define MAHJONG_CARD(__card)		((__card)&0x3F)

#define MAHJONG_NOTZI(__card) 					(((__card)&0x30)!=MAHJONG_CARD_ZI)
#define MAHJONG_ISZI(__card) 					(((__card)&0x30)==MAHJONG_CARD_ZI && (__card)!=0 )
#define MAHJONG_ISFENG(__card)					(((__card)&0x30)==MAHJONG_CARD_ZI && ((__card)&0x0F)<5 && ((__card)&0x0F)>0)
#define MAHJONG_ISJIAN(__card)					(((__card)&0x30)==MAHJONG_CARD_ZI && ((__card)&0x0F)>4 && ((__card)&0x0F)<8)
#define MAHJONG_ISDIY(__card) 					!((__card)&0x80)
#define MAHJONG_ISCHI(__card) 					((__card)&0x80)
#define GET_MAHJONG_CARDCLASS(__card)			((__card) & 0x30)
#define GET_MAHJONG_CARDVALUE(__card)			((__card) & 0x0F)
#define MAHJONG_ISFLOWERCARD(__card)	(GET_MAHJONG_CARDCLASS(__card) == MAHJONG_CARD_ZI \
										&& GET_MAHJONG_CARDVALUE(__card) > MAHJONG_CARD_ZI_BAI)

#define MAHJONG_MAKE_CARD(__class,__value)		(__class+__value)
#define MAHJONG_ISPLUTUS(__card)		(((__card)&0x40) && !((__card)&0x80))

#define MAHJONG_ISERRORCARD(__card)		(GET_MAHJONG_CARDVALUE(__card)==0 || ( (GET_MAHJONG_CARDCLASS(__card)>0) && GET_MAHJONG_CARDVALUE(__card)>9))

#define MAHJONG_CARD_PLUTUS_SEIZOR		0xC0
#define MAHJONG_CARD_ANY_SEIZOR			0x3E


#define MAHJONG_CARD_STYLE_NULL		0x00
#define MAHJONG_CARD_STYLE_PLUTUS	0x40
#define MAHJONG_CARD_STYLE_CHI		0x80

#define MAHJONG_GANG_STATUS_AN		0x01
#define MAHJONG_GANG_STATUS_PENG	0x02
#define MAHJONG_GANG_STATUS_KE		0x03

///////////////////////////////////////////////////////////////////
#define MAHJONG_CARDWRAP_NAME_SHUN		0x1
#define MAHJONG_CARDWRAP_NAME_KE		0x2
#define MAHJONG_CARDWRAP_NAME_DUI		0x3
#define MAHJONG_CARDWRAP_NAME_HUA		0x4
#define MAHJONG_CARDWRAP_NAME_19		0x5
#define MAHJONG_CARDWRAP_NAME_ANYC		0x6
#define MAHJONG_CARDWRAP_NAME_PINHUA	0x7
#define MAHJONG_CARDWRAP_NAME_GANG			0x8

typedef struct __tagMahjongCardWrap
{
	unsigned char chCardWrapName;
	unsigned char chPages;
	unsigned char chClass;
	unsigned char chMax,chMin;
	unsigned char chExist;
	char chPageReleative[4];
	unsigned char chWrap[4];
}MahjongCardWrap,*PMahjongCardWrap;

#define MAHJONG_WRAP_RELEATIVE_MUST			0x10
#define MAHJONG_WRAP_RELEATIVE_NULL			0x20

#define MAHJONG_TYPEWRAP_NORMAL				1
#define MAHJONG_TYPEWRAP_7DUI				2
#define MAHJONG_TYPEWRAP_13					3
#define MAHJONG_TYPEWRAP_19					4
#define MAHJONG_TYPEWRAP_HUALONG			5

#define MAHJONG_CARDWRAP_CLASS_ZI			1
#define MAHJONG_CARDWRAP_CLASS_WAN			2
#define MAHJONG_CARDWRAP_CLASS_TONG			3
#define MAHJONG_CARDWRAP_CLASS_TIAO			4


typedef struct __tagMahjongHUTypeWrap
{
	unsigned char chHuType;
	struct __tagMahjongHUTypeWrap *pnext;
	unsigned char chMaxCardWraps;
	unsigned char chCardWrapOption[9];
	MahjongCardWrap	cardwrap[9];
}MahjongHUTypeWrap,*PMahjongHUTypeWrap;
		
void InitializeNormalWrap(PMahjongHUTypeWrap pnormal);
void Initialize7DuiWrap(PMahjongHUTypeWrap pnormal);
void Initialize13Wrap(PMahjongHUTypeWrap pnormal);
void Initialize19Wrap(PMahjongHUTypeWrap pnormal);
void InitializeHuaLongWrap(PMahjongHUTypeWrap pnormal);

//void __DebugHUTypeWrap(int id,int id2,PMahjongHUTypeWrap pnormal,PMahjongHUTypeWrap pnew);

PMahjongHUTypeWrap MahjongRule_CheckHu(PMahjongHUTypeWrap pnormal,unsigned char *chTest,unsigned short shMask=0xFFFF);

unsigned char PushCard2Wrap(PMahjongCardWrap pcardwrap,unsigned char *chTest,unsigned char chCardIndex);
unsigned char MahjongHU_Recursion(PMahjongHUTypeWrap presult,PMahjongHUTypeWrap pwrap,unsigned char *chTest,
								unsigned char chWrapIndex,unsigned char chCardIndex,char *allocbuffer);



#define DJGAME_MAHJONG_RULE_HUMASK_NORMAL		0x01
#define DJGAME_MAHJONG_RULE_HUMASK_131		0x02
#define DJGAME_MAHJONG_RULE_HUMASK_ZUHELONG		0x04
#define DJGAME_MAHJONG_RULE_HUMASK_13		0x08
#define DJGAME_MAHJONG_RULE_HUMASK_7		0x10

#endif
