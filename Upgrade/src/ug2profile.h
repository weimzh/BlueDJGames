#ifndef __DJGAME2_UPGRADE_PROFILE_H__
#define __DJGAME2_UPGRADE_PROFILE_H__

#define UPGRADE_PUKE_CARDS								54
#define UPGRADE_MAX_SITES								6
#define UPGRADE_MAX_TEAMS								4

#define UPGRADE_PUKE_KING_BIG							0x3F
#define UPGRADE_PUKE_KING_SMALL							0x3E

//#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO			0x01
typedef struct __tagUpgradeRoom
{
	 int		xx1;//Player's scroe must >= the value
	 int		xx2;//Player's scroe must <= the value
	unsigned short		shTeamScore;	//win game must get the 'cards number'
	unsigned char		chTeamScoreLevel;	//upgrade interval ('cards number')
	unsigned char		chCards; //how many pukes 1-4
	unsigned char		chFirstLevel;
	unsigned char		chLowest;	//Lowest grade (forbid quit if current grade <= the value)
	unsigned char		chTeams;
	unsigned char		chHands;//hands card 
	unsigned char		chScore;//Player's score will dec/inc the value (lose/wins)
}UpgradeRoom,*PUpgradeRoom;


//#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//pgamehead->chBuf:
typedef struct _tagUpgradeCurrent
{
	unsigned char		chLevel;
	unsigned char		chMaster;
	unsigned char 		chLevels[UPGRADE_MAX_TEAMS+1];
}UpgradeCurrent,*PUpgradeCurrent;

#define DJGAME_UPGRADE_TABLE_STATUS_WAIT_SHOW			0x02
#define DJGAME_UPGRADE_TABLE_STATUS_WAIT_HANDS			0x03
#define DJGAME_UPGRADE_TABLE_STATUS_WAIT_OUT			0x04

//GameTrace
#define UPGRADE2_GAMETRACE_PICKUP		0x81

#define UPGRADE2_GAMETRACE_RESET		0x09
#define UPGRADE2_GAMETRACE_CLEARDESKTOP	0x0A

#define UPGRADE_GAMETRACE_THROW			0x03
#define UPGRADE_GAMETRACE_SHOW			0x04
//chBufLen == 3,chBuf[0] (bit : yyXXXXXX,yy=numbers-1,XXXXXX=card number],if chBuf[0] ==0 over


#define UPGRADE2_GAMETRACE_HANDS		0x85

//chBufLen = how  many Hands
#define UPGRADE_GAMETRACE_MASTER		0x06
//chBuf[chBufLen]...is score card
#define UPGRADE_GAMETRACE_GAMEOVER		0x07
//chBuf[0] = Rate,chBuf[1]...Banker's subtract 

#define UPGRADE3_GAMETRACE_SUBSTRACT	0x88

//////////////////////////////////
typedef struct __tagUpgradeCardType
{
	unsigned char chCardValue;
	unsigned char n;
	unsigned char chSerials;
	unsigned char chXX;
}UpgradeCardType,*PUpgradeCardType;
	
/////////////////////////////////////////////
#define DJGAME_UPGRADE_MAGIC_DEAL			0x01
#define DJGAME_UPGRADE_MAGIC_PEEK			0x02
#define DJGAME_UPGRADE_MAGIC_MASK			0x03

#endif
