/*-
 * Copyright (c) 2005-2007
 *	The Regents of the BlueDJ Tech.(www.bluedj.com,蓝迪科技).   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement
 * Copyright (c) 2005-2007
 *	The Regents of the BlueDJ Tech.(www.bluedj.com,蓝迪科技).   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the BlueDJ Tech.(蓝迪科技). 
 		www.bluedj.com
 
 * 4. Neither the name of the BlueDJ(蓝迪科技) nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * www.bluedj.com, gg@bluedj.com
 */
 
 #ifndef __DJGAME_MESSAGE_H__
#define __DJGAME_MESSAGE_H__

#define DJGAME_MAX_GAMES					240
#define DJGAME_MAX_GAMESERVERS				240
#define DJGAME_TIMEOUT_MAXIDLE				7300

#define DJGAME_PLAYER_MAX_VIEWS						5

#define DJGAME_1GAME_MAX_PLAYERS			8
 
#define DJGAME_MAX_PROTOCOL_SIZE			240
#define DJGAME_CUR_LANGUAGES				3
#define DJGAME_LANGUAGE_SC					1
#define DJGAME_LANGUAGE_TC					2
#define DJGAME_LANGUAGE_EN					4
 
#define IP_VERSION							4

#define DJGAME_TIMEOUT_KEEPLINE				120
#define DJGAME_TIMEOUT_FORBID				30
#define DJGAME_TIMEOUT_WAITSTART			30
#define DJGAME_TIMEOUT_LOCK			35

#define DJGAME_USERGAME_TIMEOUT_IDLE			0
#define DJGAME_USERGAME_TIMEOUT_SITDOWN			300
#define DJGAME_USERGAME_TIMEOUT_WAITSTART		30
#define DJGAME_USERGAME_TIMEOUT_LOCK				35

#define DJGAME_USERNAME_LEN					25
#define DJGAME_PASSWORD_LEN					32
#define DJGAME_MAX_TABLES					128
//#define DJGAME_ROOM_BROADCAST				0xFF
//#define DJGAME_ROOM_WELCOME					0xFE
//#define DJGAME_GAME_WELCOME					0xFD

#define DJGAME_VERSION					0

#define DJGAME_SELF_USERID					0xCC

#define DJGAME_LOWEST_MAGIC_NEED			1000

#define DJGAMESERVER_DEFAULT_PORT		888

#define MEMBER_OFFSET(__ps,__m)			((int)(&(((__ps)0)->__m)))
#define MEMSET_ZERO_PARTOFSTRUCT(__ps,__p,__m1,__m2)	memset(&((__p)->__m1),0,MEMBER_OFFSET(__ps,__m2)-MEMBER_OFFSET(__ps,__m1))

typedef struct __tagUserID
{
	unsigned char		chUserID[4];
}USERID,*PUSERID;


#define X86_USERID(uid)				(*((unsigned int *)(&(uid))))
#define PX86_USERID(puid) 			(*((unsigned int *)puid))

#define DJGAME_ROBOT_MAXID			12000
#define DJGAME_ROBOT_MINID			10001
#define DJGAME_INVALID_USER			0x7FFFFFFF
#define DJGAME_BLUEDJ_ROOM_ID			0xFE
#define DJGAME_BLUEDJ_GAME_ID			0xFD
#define DJGAME_AD_ID				0xFC

#define DJGAME_ADMINISTRATOR_TABLEGROUP 	0xFB
#define DJGAME_PLAYER_BROADCAST		0xFA

typedef struct __tagDJGameProtocol
{
	unsigned char	chTotalLen;
	unsigned char	chType;
	unsigned char	chSubType;
	unsigned char	chLanguage;
	unsigned char	chBuf[1];
}DJGameProtocol,*PDJGameProtocol;

#define PROTOCOL_TOTALLEN(buf)				*(((unsigned char *)buf)+MEMBER_OFFSET(PDJGameProtocol,chTotalLen))
#define PROTOCOL_TYPE(buf)					*(((unsigned char *)buf)+MEMBER_OFFSET(PDJGameProtocol,chType))
#define PROTOCOL_LANGUAGE(buf)				*(((unsigned char *)buf)+MEMBER_OFFSET(PDJGameProtocol,chLanguage))
#define PROTOCOL_SUBTYPE(buf)				*(((unsigned char *)buf)+MEMBER_OFFSET(PDJGameProtocol,chSubType))
#define PROTOCOL_BUFFER0(buf)				*(((unsigned char *)buf)+MEMBER_OFFSET(PDJGameProtocol,chBuf))

#define DJGAME_PROTOCOL_TYPE_GETGAMELIST		0x01		//Client Request
#define DJGAME_PROTOCOL_TYPE_GAMELIST			0x01		//Server Response
//server response client as following struct
typedef struct __tagGameDescriptor
{
	unsigned char		chLen;
	unsigned char		chGameClass;
	unsigned char		chGame;
	char				szGameName[1];
}GameDescritpor,*PGameDescritpor;

#define DJGAME_PROTOCOL_TYPE_GETGAMELIST2		0x11		//Client Request
#define DJGAME_PROTOCOL_TYPE_GAMELIST2			0x11		//Server Response
typedef struct __tagGameDescriptor2
{
	unsigned char		chLen;
	unsigned char		chGameClass;
	unsigned char		chGame;
	unsigned int 		uiPlayers;
	char				szGameName[1];
}GameDescritpor2,*PGameDescritpor2;

/*
#define DJGAME_PROTOCOL_TYPE_GETSERVERLIST				0x02	//Client Request
//chSubType is GameClassID , chBuf[0] is GameID
#define DJGAME_PROTOCOL_TYPE_SERVERLIST					0x02	//server response
typedef struct __tagServerDescriptor
{
	unsigned char		chLen;
	unsigned char		chGameClass;
	unsigned char		chGame;
	unsigned char		chServerID[sizeof(short)];			//short [0]=LowByte [1]=HiByte
	unsigned char		chIP[IP_VERSION];			//[0].[1].[2].[3] (.[5].[6].. IPv6)
	unsigned char		chPort[sizeof(short)];				//short [0]=LowByte [1]=HiByte
	char				szServerName[1];
}ServerDescritpor,*PServerDescritpor;
*/
#define DJGAME_PROTOCOL_TYPE_GETSERVERLIST2				0x12	//Client Request
//chSubType is GameClassID , chBuf[0] is GameID
#define DJGAME_PROTOCOL_TYPE_SERVERLIST2				0x12	//server response
typedef struct __tagServerDescriptor2
{
	unsigned char		chLen;
	unsigned char		chGameClass;
	unsigned char		chGame;
	unsigned char 		chIPV;
	unsigned int 		uiPlayers;
	unsigned char		chServerID[sizeof(short)];			//short [0]=LowByte [1]=HiByte
	unsigned char		chIP[IP_VERSION];			//[0].[1].[2].[3] (.[5].[6].. IPv6)
	unsigned char		chPort[sizeof(short)];				//short [0]=LowByte [1]=HiByte
	char				szServerName[1];
}ServerDescritpor2,*PServerDescritpor2;

//IPV6
#define DJGAME_PROTOCOL_TYPE_GETSERVERLIST6				0x13	//Client Request
//chSubType is GameClassID , chBuf[0] is GameID
#define DJGAME_PROTOCOL_TYPE_SERVERLIST6				0x13	//server response
typedef struct __tagServerDescriptor6
{
	unsigned char		chLen;
	unsigned char		chGameClass;
	unsigned char		chGame;
	unsigned char 		chIPV;
	unsigned int 		uiPlayers;
	unsigned char		chServerID[sizeof(short)];			//short [0]=LowByte [1]=HiByte
	unsigned char		chIP[20];			//[0].[1].[2].[3] (.[5].[6].. IPv6)
	unsigned char		chPort[sizeof(short)];				//short [0]=LowByte [1]=HiByte
	char				szServerName[1];
}ServerDescritpor6,*PServerDescritpor6;


#define DJGAME_PROTOCOL_TYPE_LOGIN				0x03		//client request
//chLanguage is current languages that can be user support, chBuf:
typedef struct __LoginInfo
{
	unsigned int 		uLoginType;
	unsigned char		szUserName[DJGAME_USERNAME_LEN + 2];
	unsigned char		szPassword[DJGAME_PASSWORD_LEN + 2];
	unsigned char		chGameClass;
	unsigned char		chGame;
	unsigned char		chOS;
}LOGININFO,*PLOGININFO;
#define DJGAME_CLIENT_OS_WINDOWS				0x01
#define DJGAME_CLIENT_OS_LINUX					0x02
#define DJGAME_CLIENT_OS_MACOS					0x03
#define DJGAME_CLIENT_OS_MOBILE					0x04
#define DJGAME_CLIENT_OS_TV						0x05
#define DJGAME_CLIENT_OS_BOX					0x06
#define DJGAME_CLIENT_OS_JJETO					0x07
#define DJGAME_CLIENT_OS_LOONGSON				0x08

#define DJGAME_LOGINTYPE_ROOT					0xFFFFFFFF
#define DJGAME_LOGINTYPE_QQ						0x0FFFFFFF

#define DJGAME_PROTOCOL_TYPE_QUIT					0x04	
#define DJGAME_PROTOCOL_TYPE_DISCONNECT				0x04
//chBuf[0] GameClass;  chBuf[1] GameID
#define DJGAME_PROTOCOL_SUBTYPE_DISCONNECT_OVERLAP	0x01
#define DJGAME_PROTOCOL_SUBTYPE_DISCONNECT_TIMEOUT	0x02

#define DJGAME_PROTOCOL_TYPE_SEARCH					0x05
//userid = *((int *)pprotocol->chBuf)
#define DJGAME_PROTOCOL_TYPE_SEARCH_RESULT			0x05
typedef struct __tagSearchResult
{
	USERID			userid;
	unsigned char chGameClass;
	unsigned char chGame;
	unsigned char chServerID[sizeof(short)];
}SearchResult,*PSearchResult;

#define DJGAME_PROTOCOL_TYPE_CHAT					0x06	//client same with server
//如果是房间广播，游戏广播，广告。。。则chLanguange = index;
//chSubType= strlen(szSentence) chBUf=, 
typedef struct __tagChatContent
{
	USERID			userid_speaker;
	USERID			userid_listener;	
	unsigned char   chGameClass;
	unsigned char   chGame;
	char 			szSentence[1];
}ChatContent,*PChatContent;

#define DJGAME_CHATSENTENCE_MAXLEN		182
#define DJGAME_CHATCONTENT_MAXSIZE		(sizeof(ChatContent)+DJGAME_CHATSENTENCE_MAXLEN+2)

#define DJGAME_PROTOCOL_TYPE_SPEED					0x07

#define DJGAME_PROTOCOL_TYPE_ADMIN					0x08	//administrator command
typedef struct __tagDJGameAdmin
{
	unsigned char chCommand;
	USERID userid;
	unsigned char chParam1;
	unsigned char chParam2;
	unsigned char chBufLen;
	unsigned char chBuf[4];
}DJGameAdminCommand,*PDJGameAdminCommand;
#define DJGAME_PROTOCOL_ADMIN_SUBTYPE_KICK		0x01	//force disconnect
#define DJGAME_PROTOCOL_ADMIN_SUBTYPE_SHUTUP	0x02
//chParam1 = min.
#define DJGAME_PROTOCOL_ADMIN_SUBTYPE_TXMONEY	0x03
#define DJGAME_PROTOCOL_ADMIN_SUBTYPE_TXMAGIC	0x05
#define DJGAME_PROTOCOL_ADMIN_SUBTYPE_TXCOUNTER	0x06
//chBufLen = 4,chBuf[0-3] = int

#define DJGAME_PROTOCOL_ADMIN_SUBTYPE_PLAYERDETAIL 0x07
//server return
typedef struct __tagDJGamePlayerDetail
{
	int				logintime;
	unsigned short shPosition;
	unsigned char  chXX[40];	//以后增加内容
	unsigned short shAddrLen;
	unsigned char  addr[1];
}DJGamePlayerDetail,*PDJGamePlayerDetail;

#define DJGAME_PROTOCOL_ADMIN_SUBTYPE_SERVERDETAIL 0x08
typedef struct __tagDJGameServerDetail
{
	unsigned short shDBQueue;
}DJGameServerDetail,*PDJGameServerDetail;

#define DJGAME_PROTOCOL_ADMIN_SUBTYPE_BANNEDIP 0x09
#define DJGAME_PROTOCOL_ADMIN_SUBTYPE_BANNEDACCOUNT 0x0A


#define DJGAME_PROTOCOL_TYPE_NEWGAME				0x09
//chBuf[0] = chGameClass [1] = chGame

#define DJGAME_PROTOCOL_TYPE_P2P				0x0B
#define DJGAME_PROTOCOL_P2P_SUBTYPE_REQUEST		0x01
//chBuf[0-3] userid
#define DJGAME_PROTOCOL_P2P_SUBTYPE_AGREE		0x02
typedef struct __tagDJGameP2P
{
	USERID			host;
	USERID			client;
	unsigned short  shPort;
	unsigned char 	addr_len;
	unsigned char 	addr[1];		//client 
}DJGameP2P,*PDJGameP2P;
//P2P的发起者也有可能发送这个协议包,以便交换主从关系

//P2P连接建立后,根据P2P协议(客户端定义即可)通讯,完成语音聊天,旁观(可看牌),传文件等功能

#define DJGAME_PROTOCOL_TYPE_SETFEATURE				0x10
typedef struct __tagDJGameProtocolFeature
{
	unsigned char 		chProtocolOption;
	unsigned char 		chProtocolVersion;
	unsigned char 		chBuf[8];
}DJGameProtocolFeature,*PDJGameProtocolFeature;
#define DJGAME_PROTOCOL_OPTION_SHORT				0x01
#define DJGAME_PROTOCOL_OPTION_CLOSEBROADCAST		0x02
#define DJGAME_PROTOCOL_OPTION_COMPRESS				0x04





#define DJGAME_PROTOCOL_TYPE_UNWELCOME			0x32
typedef struct __tagSetUnwelcome
{
	USERID userid;
}SetUnwelcome,*PSetUnwelcome;

#define DJGAME_PROTOCOL_TYPE_GETCARDS			0x33
typedef struct __tagPlayerCards
{
	unsigned char chCardID;
	unsigned char chPages;
}DJGamePlayerCards,*PDJGamePlayerCards;
////////////////////////////////////////////////////////////////////////////////
//
//			Game Data Structure
//
////////////////////////////////////////////////////////////////////////////////
#define DJGAME_PROTOCOL_TYPE_SUBMIT_GAMEDATA					0x20
#define DJGAME_PROTOCOL_TYPE_GAMEDATA							0x20
typedef struct __tagGameDataHead
{
	unsigned char chGameClass;
	unsigned char chGame;
	unsigned char chBuf[1];
}GameDataHead,*PGameDataHead;
#define GET_GAMEDATA_HEAD(__protocolpacket)	(PGameDataHead)(((PDJGameProtocol)__protocolpacket)->chBuf)

#define DJGAME_PROTOCOL_SUBTYPE_GET_USERINFO			0x01
////from Client :PGameDataHead->chBuf[0]-[3] = UserID
#define DJGAME_PROTOCOL_SUBTYPE_USERINFO				0x01
//the struct will be defined by game module, but Must include as following struct
typedef struct __tagGeneralUserDes
{
	USERID					userid;
	char	   		szUserName[DJGAME_USERNAME_LEN+2];
	int		iMoney;
	int		iMagic;
	unsigned char		chGender;
	unsigned char		chNation;
	unsigned char		chSupportLanguages;
	unsigned char	 	chUserType;
	unsigned int		uGroupID;
	unsigned short		shUnwelcome;
	unsigned int		uBreak;
	unsigned int		uTotal;
	unsigned int		uTotalTime;
	
	unsigned char	   chOS;
	unsigned char	   chSpeed;
	unsigned char	   chStatus;
	
	unsigned char 		chGroupDutyID;
	unsigned int 		uiAvatar;
	unsigned int		uiShow;
	int					iLastTime;
	int 		iCounter;
	char	   		szNickName[DJGAME_USERNAME_LEN+2];
}GeneralUserDes,*PGeneralUserDes;
//GeneralUserDes 中chStatus有以下几种状态:
#define DJGAME_USER_STATUS_NULL			0
#define DJGAME_USER_STATUS_NORMAL		1
#define DJGAME_USER_STATUS_INIT			2
#define DJGAME_USER_STATUS_FORBID		4
#define DJGAME_USER_STATUS_DELAYDEL		8
#define DJGAME_USER_STATUS_BREAK		0x80
#define DJGAME_USER_STATUS_KICKED		0x81
//在登录以后请求用户数据之前,先发送DJGAME_PROTOCOL_TYPE_SETFEATURE,
//可以设置为短用户数据协议,2.0以上版本才支持
typedef struct __tagShortUserDes
{
	unsigned char 			chRoomID;
	unsigned char 			chTable;
	unsigned char 			chSite;
	unsigned char 			chIsShort;	//==1
	USERID					userid;
	int						iMoney;
	int						iCounter;
	int						iGameScore;
	unsigned char			chGender;
	unsigned char 			chGameStatus;
	char	   				szUserName[1]; //名字,变长
}ShortUserDes,*PShortUserDes;

typedef struct __tagGameNormalSetup
{
	unsigned char	chLowestSpeed;
	unsigned char	chSameIP;
	unsigned char	chBreakRate;
	unsigned char	chXX;
	int	iLowestScore;
	int	iHighestScore;
	unsigned short	shMaxUnwelcome;
}GameNormalSetup,*PGameNormalSetup;

typedef struct __tagGameNormalData
{	
	int 					iScore;
	unsigned int 			uiWins;
	unsigned int 			uiLoses;
	unsigned short 			shGroup;
	unsigned char 			chStatus;
	
	unsigned char			chRoom;
	unsigned char			chTable;
	unsigned char			chSite;
	
	unsigned int 			uiDraws;
	unsigned char 			chUserTypeID;
	unsigned char 			chGroupDuty;
}GameNormalData,*PGameNormalData;

typedef struct __tagNormalUserDes
{
	unsigned char 			chGeneralUserDesLen; //chRoomID;
	unsigned char 			chGameNormalDataLen;//chTable;
	unsigned char 			chGameNormalSetupLen;//chSite;
	unsigned char 			chIsShort;	//==0
	GeneralUserDes			general;
	GameNormalData			gamedata;
	GameNormalSetup			setup;
	unsigned char 			chBuf[1];//extra data by game define
}NormalUserDes,*PNormalUserDes;

//由各游戏模块中控制的用户状态可以设置为以下几种:(通常由游戏基础库来控制即可)
#define DJGAME_USERGAME_STATUS_IDLE					0x00
#define DJGAME_USERGAME_STATUS_DOWN					0x01
#define DJGAME_USERGAME_STATUS_READY				0x02
#define DJGAME_USERGAME_STATUS_VIEW					0x80
#define DJGAME_USERGAME_STATUS_PLAY					0x04

#define DJGAME_PROTOCOL_SUBTYPE_GET_ROOMINFO			0x02
//from client: PGameDataHead->chBuf[0] is RoomID
#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO				0x02
//the struct will be defined by game module, but Must include as following struct
typedef struct __tagGeneralRoomDes
{
	unsigned char			chRoomID;
	unsigned char			chTables;
	unsigned char			chSites;				//max sites
	unsigned char			chOptions;				//Magic,View?
	unsigned char			chMaxUsers;
	unsigned char 			chSize;
	unsigned short			shCurUsers;
	int 			iPlayerLowestScore;
	int				iPlayerHighestScore;
	unsigned char 	chMultiple;
	unsigned char 	chOptions2;
	unsigned short  shGroup;
	int 			iMinCounter;
}GeneralRoomDes,*PGeneralRoomDes;

typedef struct __tagNormalRoomDes
{
	GeneralRoomDes	general;
	unsigned char   chBuf[1];
}NormalRoomDes,*PNormalRoomDes;

#define DJGAME_ROOMOPTION_UNMAGIC						0x01
#define DJGAME_ROOMOPTION_COUNTER					0x04
#define DJGAME_ROOMOPTION_PRIVATE_VIEW					0x10
#define DJGAME_ROOMOPTION_MATCH					0x20
#define DJGAME_ROOMOPTION_RANDASSIGNSITE				0x40
#define DJGAME_ROOMOPTION_ANYPLAYER				0x80

#define DJGAME_PROTOCOL_SUBTYPE_GET_TABLEMAP			0x03
//PGameDataHead->chBuf[0] is RoomID,chBuf[1] is MinTableID,[2]= MaxTableID
#define DJGAME_PROTOCOL_SUBTYPE_TABLEMAP				0x03
typedef struct __tagTableMapHead	//pGameDataHead->chBuf
{
	unsigned char chRoom;
	unsigned char chSites;
	unsigned char chBuf[1];
}TableMapHead,*PTableMapHead;
//一个tablemap包中可以包含多个tablemap,因此需要计算出下一个tablemap的起始地址
//同时,客户端应根据包的总长判断该包包含多少个tablemap
#define DJGAME_TABLEMAP_SIZE(__pmaphead)		(__pmaphead->chSites*sizeof(USERID)+2)
#define DJGAME_TABLEMAP_NEXT(__pmaphead,pmap)	((PDJGameTableMap)( ((char *)pmap)+DJGAME_TABLEMAP_SIZE(__pmaphead)))
typedef struct __tagDJGameTableMap
{
	unsigned char chTable;
	unsigned char chStatus;
	USERID			userid[1];
}DJGameTableMap,*PDJGameTableMap;
#define DJGAME_TABLE_STATUS_IDLE			0x0
#define DJGAME_TABLE_STATUS_WAITSTART		0x01
#define DJGAME_TABLE_STATUS_WAIT_LOCK		0xF0

 
#define DJGAME_PROTOCOL_SUBTYPE_DOWN					0x04
//PGameDataHead->chBuf[0] = TableID,PGameDataHead->chBuf[1] = siteID
#define DJGAME_PROTOCOL_SUBTYPE_UP						0x05
#define DJGAME_PROTOCOL_SUBTYPE_READY					0x06
#define DJGAME_PROTOCOL_SUBTYPE_CHANGEROOM				0x07
//chBuf[0] = RoomID
#define DJGAME_PROTOCOL_SUBTYPE_START					0x10 //if from server,chBuf[0] is start status

#define DJGAME_PROTOCOL_SUBTYPE_VIEW					0x11
#define DJGAME_PROTOCOL_SUBTYPE_QUITVIEW				0x12
//chbuf = userid


#define DJGAME_PROTOCOL_SUBTYPE_INVITE					0x13
typedef struct __tagInviteJoinIn
{
	USERID			userid;
	unsigned char	chTable;
	unsigned char	chStatus;
}InviteJoinIn,*PInviteJoinIn;

#define DJGAME_PROTOCOL_SUBTYPE_SETUP					0x14

#define DJGAME_PROTOCOL_SUBTYPE_QUIT					0x15

#define DJGAME_PROTOCOL_SUBTYPE_GAMEOVER				0x16 //已经废弃
//chBuf[0] == win/lost
#define DJGAME_GAMEOVER_WINNER		0x01
#define DJGAME_GAMEOVER_LOSER		0x00

#define DJGAME_PROTOCOL_SUBTYPE_GAMEOVER2				0x56
typedef struct __tagDJGameOverSite
{
	unsigned char chSite;
	int 			score;
}DJGameOverSite,*PDJGameOverSite;
typedef struct __tagDJGameOver
{
	unsigned char chSites;
	DJGameOverSite site[1];
}DJGameOver,*PDJGameOver;//从2.0起,gameover时发出每个用户的输赢状态
#define DJGAME_GAMEOVER_STATUS_SITES		0x80
//这些座位号用来表示,本结构是结果说明而不是某个人的分数
typedef struct __tagDJGameOverStatus
{
	unsigned char chSites; // == DJGAME_GAMEOVER_STATUS_SITES
	unsigned short shWinSites;
	unsigned short shLoseSites;		//Site Mask
}DJGameOverStatus,*PDJGameOverStatus;


#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//privode current game info,game module define

#define DJGAME_PROTOCOL_SUBTYPE_WAIT_USER				0x18 //已经废弃
typedef struct __tagDJGameWaitUser
{
	unsigned char chSite;
	unsigned char chTableStatus;
	unsigned char chTimeout;
	unsigned char chWaitMask;
}DJGameWaitUser,*PDJGameWaitUser;

#define DJGAME_PROTOCOL_SUBTYPE_WAIT_USER2				0x58
typedef struct __tagDJGameWaitUser2
{
	unsigned char chTable;
	unsigned char chTableStatus;
	unsigned short shTimeout;
	unsigned short 	shWaitMask;
}DJGameWaitUser2,*PDJGameWaitUser2;

#define DJGAME_PROTOCOL_SUBTYPE_BANISH					0x19
//chBuf[0] site id

#define DJGAME_PROTOCOL_SUBTYPE_QUITGROUP				0x1a
#define DJGAME_PROTOCOL_SUBTYPE_JOINGROUP				0x1b

#define DJGAME_PROTOCOL_SUBTYPE_TABLE2IDLE				0x1c
//chbuf = tableid


#define DJGAME_PROTOCOL_SUBTYPE_RANK				0x1d
typedef struct __tagDJGameRank
{
	unsigned char chType;
	unsigned char chIndex;
	unsigned int				uiScore;
	int 			timestamp;
	USERID			userid;
	char szUserName[DJGAME_USERNAME_LEN+2]; //服务器发送时,长度等于具体用户名的长度
}DJGameRank,*PDJGameRank;
#define GET_DJGAMERANK_SIZE(__prank)		(sizeof(DJGameRank)-(DJGAME_USERNAME_LEN-strlen((__prank)->szUserName)))
#define DJGAME_RANK_TYPE_DAILY				0x01
#define DJGAME_RANK_TYPE_WEEKLY				0x02
#define DJGAME_RANK_TYPE_MONTHLY			0x04
#define DJGAME_RANK_TYPE_YEARLY				0x08
#define DJGAME_RANK_TYPE_HISTORY			0x10

#define DJGAME_PROTOCOL_SUBTYPE_LOGIN					0x1e
typedef struct __tagGameLoginInfo
{
	unsigned short shServerID;
}GameLoginInfo,*PGameLoginInfo;
#define DJGAME_PROTOCOL_SUBTYPE_GETUSERSTATUS			0x1f
//request : chBuf[0]-chBuf[3] userid
typedef struct __tagDJGameUserStatus
{
	USERID			userid;
	unsigned char chUserStatus;
	unsigned char chTable;
	unsigned char chSite;
	unsigned char chTableStatus;
}DJGameUserStatus,*PDJGameUserStatus;

#define DJGAME_PROTOCOL_SUBTYPE_MAGIC					0x20
//chBuf[0] is magic id,chBuf[1]=object site
#define DJGAME_PROTOCOL_SUBTYPE_MAGIC_RESULT			0x21
typedef struct __tagMagicResult
{
	unsigned char chMagic;
	unsigned char chObject;
	unsigned char chResult;
	unsigned char chBufferLen;
	unsigned char buffer[1];
}MagicResult,*PMagicResult;

#define DJGAME_PROTOCOL_MAGIC_RESULT_SUCCESS			0x00
#define DJGAME_PROTOCOL_MAGIC_RESULT_EXPOSURE			0x01
#define DJGAME_PROTOCOL_MAGIC_RESULT_INVALID			0x02

#define DJGAME_PROTOCOL_SUBTYPE_MAGIC_EXPOSURE			0x22

#define DJGAME_PROTOCOL_SUBTYPE_KICK					0x23
//chBuf[0]-[3] = userid ,将该人踢开,只有先坐下的人能替后来者
//客户端踢人时发送本subtype,userid为被踢人
//服务器发送本subtype给被踢者,userid为踢人者

#define DJGAME_PROTOCOL_SUBTYPE_REQUEST_PRIVATE_VIEW	0x24
#define DJGAME_PROTOCOL_SUBTYPE_AGREE_PRIVATE_VIEW		0x25
#define DJGAME_PROTOCOL_SUBTYPE_CLOSE_PRIVATE_VIEW		0x26
typedef struct __tagDJGamePrivateView
{
	USERID		lookin;
	USERID		host;
	unsigned char chTable;
	unsigned char chSite;
}DJGamePrivateView,*PDJGamePrivateView;


//2.0起客户端应不再发送DJGAME_PROTOCOL_SUBTYPE_CHAT,改为组聊方式
#define DJGAME_PROTOCOL_SUBTYPE_CHAT					0x40 //已经废弃


#define DJGAME_PROTOCOL_SUBTYPE_ADMIN					0x80
//typedef struct __tagDJGameAdmin	reference DJGAME_PROTOCOL_TYPE_ADMIN
#define DJGAME_PROTOCOL_SUBTYPE_ADMIN_COMMAND_INCSCORE		0x01

#define DJGAME_PROTOCOL_SUBTYPE_ADMIN_COMMAND_FREETABLE		0x03
#define DJGAME_PROTOCOL_SUBTYPE_ADMIN_COMMAND_REPLACETEXT		0x10
//ReplaceRoomText : chParam1 = :
#define DJGAME_SENDROOMTEXT_OPTION_ROOMWEL			0x10
#define DJGAME_SENDROOMTEXT_OPTION_GAMEWEL			0x20
#define DJGAME_SENDROOMTEXT_OPTION_AD				0x40
//ReplaceRoomText : chParam2 = index
//如果index不为0，则替换原来相同ID的信息，如果该ID不存在，则加入新信息
//如index=0,则只当前发一编
/*
#define DJGAME_PROTOCOL_SUBTYPE_GAMEDATA			0xFF //已经废弃
typedef struct __GeneralGameTraceHead
{
	unsigned char		chType;
	unsigned char		chSite;
	unsigned char		chBufLen;
	unsigned char		chBuf[1];
}GeneralGameTraceHead,*PGeneralGameTraceHead;
*/
#define DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2			0xFE
typedef struct __GeneralGameTrace2Head
{
	unsigned char 		chTable;
	unsigned char		chType;
	unsigned char		chSite;
	unsigned char		chBufLen;
	unsigned char		chBuf[1];
}GeneralGameTrace2Head,*PGeneralGameTrace2Head;
//GameTrace data
//0x8X是私有的游戏数据,而其他是公共数据
#define DJGAME_PRIVATE_TRACE(__type)			((__type)&0x80)



#define DJGAME_PROTOCOL_SUBTYPE_GAMECARD			0x30
//客户端请求使用道具卡
typedef struct __tagReqUseCard
{
			int iParam;
	USERID		hostid;
	USERID		targetid;
	unsigned char chTable;
	unsigned char chCard;
}ReqUseCard,*PReqUseCard;

#define DJGAME_GAMECARD_TYPE_SCORE			0x00
#define DJGAME_GAMECARD_TYPE_EMOTION		0x80
#define DJGAME_GAMECARD_TYPE_GAME		0x40		//游戏自定义的卡片
#define DJGAME_GAMECARD_TYPE_TIMER		0x20		//计时卡
			//天使，福神（lucky)，10倍卡是计时卡,ReqUseCard.iParam指明剩余时间

#define DJGAME_GAMECARD_ANGEL				0x21	//天使卡，避免被攻击（负分加倍，赢分减半等）
#define DJGAME_GAMECARD_LUCKY				0x23	//福神卡，赢分加倍,输分减半
#define DJGAME_GAMECARD_10MULTIPLE		0x22	//赢分10倍

#define DJGAME_GAMECARD_ZERO				0x04	//清零
#define DJGAME_GAMECARD_DOUBLE			0x05 //赢分加倍
#define DJGAME_GAMECARD_NHALF				0x06 //输分减半
#define DJGAME_GAMECARD_5MULTIPLE			0x07	//赢分5倍
#define DJGAME_GAMECARD_N2MULTIPLE		0x09	//输分加倍
#define DJGAME_GAMECARD_N5MULTIPLE		0x0A	//输分5倍
#define DJGAME_GAMECARD_HALF				0x0B 	//赢分减半

#define DJGAME_GAMECARD_FLOWER				0x81
#define DJGAME_GAMECARD_BROADCAST			0x82
#define DJGAME_GAMECARD_EMOTION			0x83
#define DJGAME_GAMECARD_EGG			0x84

#define DJGAME_PROTOCOL_SUBTYPE_BONUS			0x31
typedef struct __tagDJGameBonus
{//游戏奖励
	unsigned char  chTable;
	unsigned char 	chRoom;
	unsigned short shBonus;
	unsigned char	chBonusType;
	unsigned char  chTarget;
	unsigned char  chParam1;
	unsigned char  chParam2;
	unsigned char  chBuf[8];	//游戏定义使用
}DJGameBonus,*PDJGameBonus;
#define DJGAME_PROTOCOL_GAMEBONUS_TYPE_CARD		0x01
#define DJGAME_PROTOCOL_GAMEBONUS_TYPE_COUNTER		0x02
#define DJGAME_PROTOCOL_GAMEBONUS_TYPE_MONEY		0x03

#define DJGAME_PROTOCOL_SUBTYPE_BONUSFULFILL		0x32
typedef struct __tagDJGameBonusFulfill
{//兑现游戏奖励
	unsigned char  chTable;
	unsigned char 	chRoom;
	unsigned short shBonus;
	unsigned char	chBonusType;
	USERID			uid;
}DJGameBonusFulfill,*PDJGameBonusFulfill;

/////////////////////////////////////////////////////////////////

#define DJGAME_PROTOCOL_TYPE_TEAM			0x31
#define DJGAME_PROTOCOL_SUBTYPE_GET_TEAMNAME	0x01
#define DJGAME_PROTOCOL_SUBTYPE_GET_DUTYNAME	0x02



////////////////////////////////////////////////////////////////////////////////
//
//			Game Data Over
//
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//
//ACL
//
//////////////////////////////////////////////////////////////////////////////////
#define DJGAME_PROTOCOL_TYPE_ACL							0x80
//chBuf[0] is stauts id
#define DJGAME_PROTOCOL_STATUS_SUCCESS							0x00
#define DJGAME_PROTOCOL_STATUS_EXISTING_SUCCESS					0x01
#define DJGAME_PROTOCOL_STATUS_NEWGAME							0x02
 //DJGAME_PROTOCOL_STATUS_NEWGAME,chSubType = GameClass,chLanguage = game
#define DJGAME_PROTOCOL_STATUS_IGNORE							0x03
#define DJGAME_PROTOCOL_STATUS_SUCCESS_WAIT_START				0x04
//DJGAME_PROTOCOL_STATUS_SUCCESS_WAIT_START chSubType = GameClass,chLanguage = game
#define DJGAME_PROTOCOL_STATUS_SUCCESS_PENDING					0x05
#define DJGAME_PROTOCOL_STATUS_RENEW							0x06
#define DJGAME_PROTOCOL_STATUS_GAMEOVER							0x07
#define DJGAME_PROTOCOL_STATUS_SKIP								0x70

#define DJGAME_PROTOCOL_STATUS_SERVER_BUSY						0x81
#define DJGAME_PROTOCOL_STATUS_INVALID_INFO						0x82
#define DJGAME_PROTOCOL_STATUS_NO_SUCH_DATA						0x83
#define DJGAME_PROTOCOL_STATUS_UNSUPPORT_LANGUAGE				0x85
#define DJGAME_PROTOCOL_STATUS_OVERLOADING						0x86
#define DJGAME_PROTOCOL_STATUS_TIMEOUT							0x87
#define DJGAME_PROTOCOL_STATUS_INVALID_USERINFO					0x88
#define DJGAME_PROTOCOL_STATUS_OVERLAP							0x89
//DJGAME_PROTOCOL_STATUS_OVERLAP include a ServerID, chLanguage is HIBYTE, chSubType is LOBYTE; 
#define DJGAME_PROTOCOL_STATUS_UNINSTALL_GAME					0x8A
#define DJGAME_PROTOCOL_STATUS_AUTHENTICATION_FAILED			0x8B
#define DJGAME_PROTOCOL_STATUS_NOTNULL							0x8C
#define DJGAME_PROTOCOL_STATUS_PLAYING							0x8D
#define DJGAME_PROTOCOL_STATUS_VIEWING							0x8E
#define DJGAME_PROTOCOL_STATUS_SERVER_ERROR						0x8F
#define DJGAME_PROTOCOL_STATUS_FORCE_OFFLINE					0x90
#define DJGAME_PROTOCOL_STATUS_SETUP_CONFLICT					0x91
#define DJGAME_PROTOCOL_STATUS_OLDVERSION						0x92
#define DJGAME_PROTOCOL_STATUS_PROTOCOL_ERROR					0x93
#define DJGAME_PROTOCOL_STATUS_GAMERULE_FAILED					0x94
#define DJGAME_PROTOCOL_STATUS_MAGIC_ABSURD						0x95
#define DJGAME_PROTOCOL_STATUS_MAGIC_POINT_LACK					0x96
#define DJGAME_PROTOCOL_STATUS_MAGIC_ROOMFORBID					0x97
#define DJGAME_PROTOCOL_STATUS_MAGIC_UNKOWN						0x98
#define DJGAME_PROTOCOL_STATUS_MAGIC_ING						0x99
#define DJGAME_PROTOCOL_STATUS_FORBID							0x9A
#define DJGAME_PROTOCOL_STATUS_MONEY_LACK						0x9B
#define DJGAME_PROTOCOL_STATUS_COUNTER_LACK						0x9C

#define DJGAME_SUCCESS(status)									(!(status & 0x80))
#define DJGAME_FAILED(status)									(status & 0x80)
//////////////////////////////////////////////////////////////////////////////////
//
//		ACL Over
//
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//
//	Broadcast
//DJGAME_GAME_SYSTEM_GROUPCHAT
////////////////////////////////////////////////////////////////////
#define DJGAME_PROTOCOL_TYPE_BROADCAST				0x81
//chBuf is struct as following
#define BROADCAST_BUFFER_SIZE			4
typedef struct __tagBroadcastInfo
{
	USERID			userid;
	unsigned char	chType;
	unsigned char	chMsg;
	unsigned char	chBuf[BROADCAST_BUFFER_SIZE];
}BroadcastInfo,*PBroadcastInfo;
typedef struct __tagBroadcastHead
{
	unsigned char	chGameClass;
	unsigned char	chGame;
	unsigned char	chRoom;
	unsigned char	chMessages; //how many broadcast info in the packet
}BroadcastHead,*PBroadcastHead;
//DJGAME_MAX_PROTOCOL_SIZE

#define DJGAME_SYSTEM_ID				0
#define DJGAME_SYSTEM_MSG_QUIT			0x01
#define DJGAME_SYSTEM_MSG_TRYREBOOT		0x02
#define DJGAME_SYSTEM_MSG_SHUTUP		0x03

#define DJGAME_MSGTYPE_EVENT			0x01
#define DJGAME_MSG_EVENT_IN				0x01
#define DJGAME_MSG_EVENT_OUT			0x02
#define DJGAME_MSG_EVENT_BREAK			0x03
#define DJGAME_MSG_EVENT_CONTINUE		0x04
#define DJGAME_MSG_EVENT_DOWN			0x05	//chBuf[0] is TableID,chBuf[1] is SiteID
#define DJGAME_MSG_EVENT_UP				0x06
#define DJGAME_MSG_EVENT_READY			0x07	//chBuf[0] is TableID,chBuf[1] is SiteID
#define DJGAME_MSG_EVENT_OFFLINE		0x08
#define DJGAME_MSG_EVENT_VIEW			0x09
#define DJGAME_MSG_EVENT_QUITVIEW		0x0A
#define DJGAME_MSG_EVENT_START			0x0B	//chBuf[0] is TableID
#define DJGAME_MSG_EVENT_SCORE			0x0D	//score = *((int*)chBuf)
#define DJGAME_MSG_EVENT_MAGIC			0x0E  //magic = *((int*)chBuf)
#define DJGAME_MSG_EVENT_MONEY			0x0F  //money = *((int*)chBuf)
#define DJGAME_MSG_EVENT_COUNTER			0x10  //counter = *((int*)chBuf)

#define DJGAME_MSGTYPE_SPEED			0x02
//chMsg = Speed

#define DJGAME_MSGTYPE_GROUP			0x03
#define DJGAME_MSG_GROUP_INTO			0x01
#define DJGAME_MSG_GROUP_LEAVE			0x02

#define DJGAME_MSGTYPE_ROOM			0x04
//chMsg = roomid, chBuf =(int)players



//////////////////////////////////////////////////////////////////////
//
//	Broadcast over
//
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//
// Embeded Device Protocol 
//
////////////////////////////////////////////////////////////////////////

#define DJGAME_PROTOCOL_TYPE_EMBED				0x82

//define by DJGameEmbed.h

/////////////////////////////////////////////////////////////////////////
// Embeded Over
/////////////////////////////////////////////////////////////////////////

#define DJGAME_GAMECLASS_CHESS		1
#define DJGAME_GAMECLASS_CARDS		2
#define DJGAME_GAMECLASS_MAHJONG	3
#define DJGAME_GAMECLASS_COMBAT		4
#define DJGAME_GAMECLASS_SINGLE		5
#define DJGAME_GAMECLASS_OTHER		6

#define DJGAME_GAMECLASS_SYSTEM				0xFF
#define DJGAME_GAME_SYSTEM_GROUPCHAT		0xFF
#define DJGAME_GROUPCHAT					0xFFFF

#define DJGAME_JUNQI				0x101
#define DJGAME_CHESS_JUNQI			1
#define DJGAME_XIANGQI				0x102
#define DJGAME_CHESS_XIANGQI		2
#define DJGAME_WEIQI				0x103
#define DJGAME_CHESS_WEIQI			3
#define DJGAME_GOBANG				0x104
#define DJGAME_CHESS_GOBANG			4

typedef struct __tagDJGameChessTimer
{
	unsigned short shBaseTime;		//基本时间
	unsigned char chTimeAfterBaseTime;	//基本时间用光后每步的时间
	unsigned char chTimes;		//基本时间用光后,允许的超时次数
	unsigned char chTimeAfterTimeout;	//基本时间用光后,每次超时的最长时间
	unsigned char chSteps;			//基本时间内必需走的步数
}DJGameChessTimer,*PDJGameChessTimer;
#define DJGAMECHESS_RULEMASK_BASETIME					0x01
#define DJGAMECHESS_RULEMASK_TIMEAFTERBASETIME			0x02
#define DJGAMECHESS_RULEMASK_TIMES						0x04
#define DJGAMECHESS_RULEMASK_TIMEAFTERTIMEOUT			0x08
#define DJGAMECHESS_RULEMASK_STEPS						0x10

#define DJGAME_UPGRADE				0x201
#define DJGAME_CARDS_UPGRADE		1
#define DJGAME_DOUDZHU				0x202
#define DJGAME_CARDS_DOUDZHU		2
#define DJGAME_CHAODPI				0x203
#define DJGAME_CARDS_CHAODPI		3
#define DJGAME_NIUNIU				0x204
#define DJGAME_CARDS_NIUNIU			4
#define DJGAME_GONGZHU				0x205
#define DJGAME_CARDS_GONGZHU		5
#define DJGAME_SUOHA				0x206
#define DJGAME_CARDS_SUOHA		6
#define DJGAME_21					0x207
#define DJGAME_CARDS_21		7
#define DJGAME_CHUDD			0x208
#define DJGAME_CARDS_CHUDD		8


#define DJGAME_GBMAHJONG			0x301
#define DJGAME_MAHJONG_GB			1
#define DJGAME_DOMINO			0x302
#define DJGAME_MAHJONG_DOMINO		2
#define DJGAME_GDMJ			0x303
#define DJGAME_MAHJONG_GDMJ		3

#define DJGAME_SHISENSHO			0x501
#define DJGAME_SINGLE_SHISENSHO		1
#define DJGAME_GOODLUCK			0x502
#define DJGAME_SINGLE_GOODLUCK		2


#define DJGAME_GENDER_MALE			1
#define DJGAME_GENDER_FEMALE		2

#define DJGAME_USERTYPE_GUEST			1
#define DJGAME_USERTYPE_PLAYER			2
#define DJGAME_USERTYPE_MEMBER			3
#define DJGAME_USERTYPE_VIP			4
#define DJGAME_USERTYPE_STAR			5

#define DJGAME_USERTYPE_SERVICE			15
#define DJGAME_USERTYPE_EMERGENCY		17

#define DJGAME_USERTYPE_GAMEADMIN		20

#define DJGAME_USERTYPE_MANAGER2		30
#define DJGAME_USERTYPE_ADMIN2			35
#define DJGAME_USERTYPE_SADMIN2			40


#define IS_ADMIN(__pgeneral,__pusergame)			((__pgeneral)->chUserType >=DJGAME_USERTYPE_GAMEADMIN || (__pusergame)->usergamedata.chUserTypeID>=DJGAME_USERTYPE_GAMEADMIN)
#define IS_SUPERADMIN(__pgeneral,__pusergame)			((__pgeneral)->chUserType >=DJGAME_USERTYPE_SADMIN2)

#define IS_VIP(__pgeneral,__pusergame)			((__pgeneral)->chUserType >=DJGAME_USERTYPE_VIP || (__pusergame)->usergamedata.chUserTypeID>=DJGAME_USERTYPE_VIP)

#define DJGAME_AUTHENTICATE_BROADCAST			0x01
#define DJGAME_AUTHENTICATE_KICK				0x02
#define DJGAME_AUTHENTICATE_GAMEMANAGER			0x03
#define DJGAME_AUTHENTICATE_ROOMWELCOME			0x04
#define DJGAME_AUTHENTICATE_GAMEWELCOME			0x05

#define DJGAME_AUTHENTICATE_SUCCESS				0x01
#define DJGAME_AUTHENTICATE_FAILED				0

///////////////////////////////////////////
#define MAKEGROUPID_GAME(__class,__game)		((unsigned int)((((unsigned int)__class)<<24)+(((unsigned int)__game)<<16)) )
#define MAKEGROUPID_ROOM(__class,__game,__room)		((unsigned int)((((unsigned int)__class)<<24)+(((unsigned int)__game)<<16) + (((unsigned int)__room)<<8)))
#define MAKEGROUPID_TABLE(__class,__game,__room,__table)		((unsigned int)((((unsigned int)__class)<<24)+(((unsigned int)__game)<<16) + (((unsigned int)__room)<<8)+__table))
#define MAKEGROUPID_SITE(__class,__game,__room,__table,__site)		((((unsigned int)__site)<<12)+MAKEGROUPID_TABLE(__class,__game,__room,__table))
#define MAKEGROUPID_CHAT(__userid)	
#define MAKEGROUPID_SITE_BYTABLE(__tablegroupid,__site)				((((unsigned int)__site)<<12)+(__tablegroupid))


///////////////////////////////////////////////////////////////


/////////////////////////////////////////
// for Poker

#define DJGAME_POKER_BACKGROUND			0x0F
#define DJGAME_POKER_BIGKING			0x3F
#define DJGAME_POKER_SMALLKING			0x3E

#define DJGAME_POKER_2					0x02
#define DJGAME_POKER_3					0x03
#define DJGAME_POKER_4					0x04
#define DJGAME_POKER_5					0x05
#define DJGAME_POKER_6					0x06
#define DJGAME_POKER_7					0x07
#define DJGAME_POKER_8					0x08
#define DJGAME_POKER_9					0x09
#define DJGAME_POKER_10					0x0A
#define DJGAME_POKER_J					0x0B
#define DJGAME_POKER_Q					0x0C
#define DJGAME_POKER_K					0x0D
#define DJGAME_POKER_A					0x01

#define DJGAME_POKER_CLASS_S			4
#define DJGAME_POKER_CLASS_H			3
#define DJGAME_POKER_CLASS_D			2
#define DJGAME_POKER_CLASS_C			1

#define DJGAME_POKER_CARD(__card)		((__card)&0x3F)
#define DJGAME_POKER_VALUE(__card)		((__card)&0x0F)
#define DJGAME_POKER_CLASS(__card)		((((__card)&0x30)>>4)+1)
#define DJGAME_POKER_PAGES(__card)		((((__card)&0xC0)>>6)+1)

#define DJGAME_POKER_MAKE_CODE(__pages,__class,__value)		((unsigned char)(((__class-1)<<4)+(__value)))

#define DJGAME_POKER_ISBK(__card)		(((__card)&0x3F)==DJGAME_POKER_BIGKING)
#define DJGAME_POKER_ISSK(__card)		(((__card)&0x3F)==DJGAME_POKER_SMALLKING)

#define DJGAME_POKER_ERRORCARD(__card)	(!DJGAME_POKER_ISBK(__card) && !DJGAME_POKER_ISSK(__card) && (((__card)&0x0F)<DJGAME_POKER_A || ((__card)&0x0F)>DJGAME_POKER_K))

typedef struct __tagDJGamePokerImage
{
	unsigned char chPage[0x40];
	unsigned char chSign[0x40];
}DJGamePokerImage,*PDJGamePokerImage;


////////////////////////////////////////////////////

#endif
