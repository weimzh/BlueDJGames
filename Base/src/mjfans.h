#ifndef __DJGAME_MAHJONG_FANS_H__
#define __DJGAME_MAHJONG_FANS_H__

#include "mjrule.h"

#define DJGAME_MAHJONG_FAN_TIANHU			7
#define DJGAME_MAHJONG_FAN_DIHU			8
#define DJGAME_MAHJONG_FAN_RENHU			9
//88
#define DJGAME_MAHJONG_FAN_DA4XI			10 
//DJGAME_MAHJONG_FAN_RENHU+1
#define DJGAME_MAHJONG_FAN_DA3YUAN			(DJGAME_MAHJONG_FAN_DA4XI+1)	
#define DJGAME_MAHJONG_FAN_LIU1SE			(DJGAME_MAHJONG_FAN_DA3YUAN+1)	
#define DJGAME_MAHJONG_FAN_HONGKONGQUE		(DJGAME_MAHJONG_FAN_LIU1SE+1)	
#define DJGAME_MAHJONG_FAN_9LIANBAODENG		(DJGAME_MAHJONG_FAN_HONGKONGQUE+1)	
#define DJGAME_MAHJONG_FAN_4GANG			(DJGAME_MAHJONG_FAN_9LIANBAODENG+1)
#define DJGAME_MAHJONG_FAN_LIAN7DUI			(DJGAME_MAHJONG_FAN_4GANG+1)	
#define DJGAME_MAHJONG_FAN_131				(DJGAME_MAHJONG_FAN_LIAN7DUI+1)	
//64
#define DJGAME_MAHJONG_FAN_QING19			(DJGAME_MAHJONG_FAN_131+1)		
#define DJGAME_MAHJONG_FAN_XIAO4XI			(DJGAME_MAHJONG_FAN_QING19+1)	
#define DJGAME_MAHJONG_FAN_XIAO3YUAN		(DJGAME_MAHJONG_FAN_XIAO4XI+1)	
#define DJGAME_MAHJONG_FAN_ZI1SE			(DJGAME_MAHJONG_FAN_XIAO3YUAN+1)
#define DJGAME_MAHJONG_FAN_4ANKE			(DJGAME_MAHJONG_FAN_ZI1SE+1)	
#define DJGAME_MAHJONG_FAN_1SE2LONGHUI		(DJGAME_MAHJONG_FAN_4ANKE+1)		
//48
#define DJGAME_MAHJONG_FAN_1SE4TONGSHUN		(DJGAME_MAHJONG_FAN_1SE2LONGHUI+1)	
#define DJGAME_MAHJONG_FAN_1SE4JIEGAO		(DJGAME_MAHJONG_FAN_1SE4TONGSHUN+1)	
//32
#define DJGAME_MAHJONG_FAN_1SE4BUGAO		(DJGAME_MAHJONG_FAN_1SE4JIEGAO+1)	
#define DJGAME_MAHJONG_FAN_3GANG			(DJGAME_MAHJONG_FAN_1SE4BUGAO+1)	
#define DJGAME_MAHJONG_FAN_HUN19			(DJGAME_MAHJONG_FAN_3GANG+1)		
//24
#define DJGAME_MAHJONG_FAN_7DUI				(DJGAME_MAHJONG_FAN_HUN19+1)		
#define DJGAME_MAHJONG_FAN_7XINGBUKAO		(DJGAME_MAHJONG_FAN_7DUI+1)
#define DJGAME_MAHJONG_FAN_QUAN2KE			(DJGAME_MAHJONG_FAN_7XINGBUKAO+1)
#define DJGAME_MAHJONG_FAN_QING1SE			(DJGAME_MAHJONG_FAN_QUAN2KE+1)
#define DJGAME_MAHJONG_FAN_1SE3TONGSHUN		(DJGAME_MAHJONG_FAN_QING1SE+1)
#define DJGAME_MAHJONG_FAN_1SE3JIEGAO		(DJGAME_MAHJONG_FAN_1SE3TONGSHUN+1)
#define DJGAME_MAHJONG_FAN_QUANDA			(DJGAME_MAHJONG_FAN_1SE3JIEGAO+1)
#define DJGAME_MAHJONG_FAN_QUANZHONG		(DJGAME_MAHJONG_FAN_QUANDA+1)
#define DJGAME_MAHJONG_FAN_QUANXIAO			(DJGAME_MAHJONG_FAN_QUANZHONG+1)
//16
#define DJGAME_MAHJONG_FAN_QINGLONG			(DJGAME_MAHJONG_FAN_QUANXIAO+1)
#define DJGAME_MAHJONG_FAN_3SE2LONGHUI		(DJGAME_MAHJONG_FAN_QINGLONG+1)
#define DJGAME_MAHJONG_FAN_1SE3BUGAO		(DJGAME_MAHJONG_FAN_3SE2LONGHUI+1)
#define DJGAME_MAHJONG_FAN_QUANDAI5			(DJGAME_MAHJONG_FAN_1SE3BUGAO+1)
#define DJGAME_MAHJONG_FAN_3TONGKE			(DJGAME_MAHJONG_FAN_QUANDAI5+1)
#define DJGAME_MAHJONG_FAN_3ANKE			(DJGAME_MAHJONG_FAN_3TONGKE+1)

//12
#define DJGAME_MAHJONG_FAN_QUANBUKAO		(DJGAME_MAHJONG_FAN_3ANKE+1)
#define DJGAME_MAHJONG_FAN_ZUHELONG			(DJGAME_MAHJONG_FAN_QUANBUKAO+1)
#define DJGAME_MAHJONG_FAN_DAYU5			(DJGAME_MAHJONG_FAN_ZUHELONG+1)
#define DJGAME_MAHJONG_FAN_XIAOYU5			(DJGAME_MAHJONG_FAN_DAYU5+1)
#define DJGAME_MAHJONG_FAN_3FENGKE			(DJGAME_MAHJONG_FAN_XIAOYU5+1)
//8
#define DJGAME_MAHJONG_FAN_HUALONG			(DJGAME_MAHJONG_FAN_3FENGKE+1)
#define DJGAME_MAHJONG_FAN_TUIBUDAO			(DJGAME_MAHJONG_FAN_HUALONG+1)
#define DJGAME_MAHJONG_FAN_3SE3TONGSHUN		(DJGAME_MAHJONG_FAN_TUIBUDAO+1)
#define DJGAME_MAHJONG_FAN_3SE3JIEGAO		(DJGAME_MAHJONG_FAN_3SE3TONGSHUN+1)
#define DJGAME_MAHJONG_FAN_WUFANHU			(DJGAME_MAHJONG_FAN_3SE3JIEGAO+1)
#define DJGAME_MAHJONG_FAN_MIAOSHOUHUICHUN	(DJGAME_MAHJONG_FAN_WUFANHU+1)
#define DJGAME_MAHJONG_FAN_HAIDILAOYUE		(DJGAME_MAHJONG_FAN_MIAOSHOUHUICHUN+1)
#define DJGAME_MAHJONG_FAN_GANGSHANGKAIHUA	(DJGAME_MAHJONG_FAN_HAIDILAOYUE+1)
#define DJGAME_MAHJONG_FAN_QIANGGANGHU		(DJGAME_MAHJONG_FAN_GANGSHANGKAIHUA+1)
//6
#define DJGAME_MAHJONG_FAN_PENGPENGHU		(DJGAME_MAHJONG_FAN_QIANGGANGHU+1)
#define DJGAME_MAHJONG_FAN_HUN1SE			(DJGAME_MAHJONG_FAN_PENGPENGHU+1)
#define DJGAME_MAHJONG_FAN_3SE3BUGAO		(DJGAME_MAHJONG_FAN_HUN1SE+1)
#define DJGAME_MAHJONG_FAN_5MENQI 			(DJGAME_MAHJONG_FAN_3SE3BUGAO+1)
#define DJGAME_MAHJONG_FAN_QUANQIUREN		(DJGAME_MAHJONG_FAN_5MENQI+1)
#define DJGAME_MAHJONG_FAN_2ANGANG			(DJGAME_MAHJONG_FAN_QUANQIUREN+1)
#define DJGAME_MAHJONG_FAN_2JIANKE			(DJGAME_MAHJONG_FAN_2ANGANG+1)
//4
#define DJGAME_MAHJONG_FAN_QUANDAI1			(DJGAME_MAHJONG_FAN_2JIANKE+1)
#define DJGAME_MAHJONG_FAN_BUQIUREN			(DJGAME_MAHJONG_FAN_QUANDAI1+1)
#define DJGAME_MAHJONG_FAN_2MINGGANG		(DJGAME_MAHJONG_FAN_BUQIUREN+1)
#define DJGAME_MAHJONG_FAN_HUJUEZHANG		(DJGAME_MAHJONG_FAN_2MINGGANG+1)
//2
#define DJGAME_MAHJONG_FAN_JIANKE			(DJGAME_MAHJONG_FAN_HUJUEZHANG+1)
#define DJGAME_MAHJONG_FAN_QUANFENGKE		(DJGAME_MAHJONG_FAN_JIANKE+1)
#define DJGAME_MAHJONG_FAN_MENFENGKE		(DJGAME_MAHJONG_FAN_QUANFENGKE+1)
#define DJGAME_MAHJONG_FAN_MENQIANQING		(DJGAME_MAHJONG_FAN_MENFENGKE+1)
#define DJGAME_MAHJONG_FAN_PINGHU			(DJGAME_MAHJONG_FAN_MENQIANQING+1)
#define DJGAME_MAHJONG_FAN_4GUI1			(DJGAME_MAHJONG_FAN_PINGHU+1)
#define DJGAME_MAHJONG_FAN_2TONGKE			(DJGAME_MAHJONG_FAN_4GUI1+1)
#define DJGAME_MAHJONG_FAN_2ANKE			(DJGAME_MAHJONG_FAN_2TONGKE+1)
#define DJGAME_MAHJONG_FAN_ANGANG			(DJGAME_MAHJONG_FAN_2ANKE+1)
#define DJGAME_MAHJONG_FAN_DUAN19			(DJGAME_MAHJONG_FAN_ANGANG+1)
//1
#define DJGAME_MAHJONG_FAN_1BANGAO			(DJGAME_MAHJONG_FAN_DUAN19+1)
#define DJGAME_MAHJONG_FAN_XIXIANGFENG		(DJGAME_MAHJONG_FAN_1BANGAO+1)
#define DJGAME_MAHJONG_FAN_LIAN6			(DJGAME_MAHJONG_FAN_XIXIANGFENG+1)
#define DJGAME_MAHJONG_FAN_LAOSHAOFU		(DJGAME_MAHJONG_FAN_LIAN6+1)
#define DJGAME_MAHJONG_FAN_19KE				(DJGAME_MAHJONG_FAN_LAOSHAOFU+1)
#define DJGAME_MAHJONG_FAN_MINGGANG			(DJGAME_MAHJONG_FAN_19KE+1)
#define DJGAME_MAHJONG_FAN_QUE1MEN			(DJGAME_MAHJONG_FAN_MINGGANG+1)
#define DJGAME_MAHJONG_FAN_WUZI				(DJGAME_MAHJONG_FAN_QUE1MEN+1)
#define DJGAME_MAHJONG_FAN_BIANZHANG		(DJGAME_MAHJONG_FAN_WUZI+1)
#define DJGAME_MAHJONG_FAN_KANZHANG			(DJGAME_MAHJONG_FAN_BIANZHANG+1)
#define DJGAME_MAHJONG_FAN_DANDIAO			(DJGAME_MAHJONG_FAN_KANZHANG+1)
#define DJGAME_MAHJONG_FAN_ZIMO				(DJGAME_MAHJONG_FAN_DANDIAO+1)

#define DJGAME_MAHJONG_FAN_HUAPAI			(DJGAME_MAHJONG_FAN_ZIMO+1)

#define DJGAME_GDMJ_FAN_QINGPENG				(DJGAME_MAHJONG_FAN_HUAPAI+1	)
#define DJGAME_GDMJ_FAN_HUNPENG				(DJGAME_GDMJ_FAN_QINGPENG+1)

#define DJGAME_MAHJONG_FAN_MAXFANS			(DJGAME_GDMJ_FAN_HUNPENG+2	)


typedef int (*LPMahjongFan_Proc)(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool, \
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain, \
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
typedef struct __tagMahjongFanTable
{
	unsigned char chFanID;
	 char szFanName[30];
	LPMahjongFan_Proc proc;
}MahjongFanTable,*PMahjongFanTable;

typedef struct __tagMahjongFanResult
{
	unsigned char chFanID;
	int 			iFan;
}MahjongFanResult,*PMahjongFanResult;


void MahjongFan_InitializeFanTable();
void MahjongFan_DeleteFanTable();

void MahjongFan_SetFanMask(int iFan,unsigned char *pchSupportFans);
void MahjongFan_ClearFanMask(int iFan,unsigned char *pchSupportFans);
void MahjongFan_Check(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,
						unsigned char *pchSupportFans,PMahjongFanResult presult,unsigned char chMaxResult);
////////////////////////////////////////////////////////////////////////////////////
int MahjongFan_Tianhu(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Dihu(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Renhu(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);

int MahjongFan_Da4Xi(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Da3Yuan(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Liu1Se(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_9LianBaoDeng(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_4Gang(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Lian7Dui(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_131(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Qing19(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Xiao4Xi(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Xiao3Yuan(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Zi1Se(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_4AnKe(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_1Se2LongHui(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_1Se4TongShun(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_1Se4JieGao(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_1Se4BuGao(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_3Gang(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Hun19(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_7Dui(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_7XingBuKao(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Quan2Ke(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Qing1Se(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_1Se3TongShun(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_1Se3JieGao(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_QuanDa(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_QuanZhong(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_QuanXiao(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_QingLong(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_3Se2LongHui(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_1Se3BuGao(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_QuanDai5(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_3TongKe(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_3AnKe(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_QuanBuKao(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_ZuHeLong(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_DaYu5(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_XiaoYu5(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_3FengKe(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_HuaLong(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_TuiBuDao(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_3Se3TongShun(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_3Se3JieGao(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
//int MahjongFan_WuFanHu(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
//						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
//						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_MiaoShouHuiChun(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_HaiDiLaoYue(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_GangShangKaiHua(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_QiangGangHu(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_PengPengHu(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Hun1Se(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_3Se3BuGao(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_5MenQi(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_QuanQiuRen(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_2AnGang(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_2JianKe(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_QuanDai1(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_BuQiuRen(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_2MingGang(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_HuJueZhang(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_JianKe(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_QuanFengKe(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_MenFengKe(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_MenQianQing(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_PingHu(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_4Gui1(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_2TongKe(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_2AnKe(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_AnGang(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Duan19(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_1BanGao(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_XiXiangFeng(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Lian6(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_LaoShaoFu(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_19Ke(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_MingGang(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_Que1Men(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_WuZi(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_BianZhang(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_KanZhang(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_DanDiao(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
int MahjongFan_ZiMo(PMahjongHUTypeWrap *ppwrap,unsigned char *pchOutPool,
						unsigned char chOutPoolLen,unsigned char chLastCard,unsigned char chRemain,
						unsigned char chGangStatus,unsigned char chQuanFeng,unsigned char chMenFeng,unsigned char *pchSupportFans);
					
						
						
						
///////////////////////////////////////////
// FAN PRoc define

#endif
