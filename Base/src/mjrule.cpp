
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mjrule.h"
#include "mjfans.h"
#include "protocol.h"

extern PMahjongFanTable g_pmjfTable;
								
unsigned char MahjongHU_Check19(PMahjongHUTypeWrap pwrap);
unsigned char MahjongHU_Check13(PMahjongHUTypeWrap pwrap);
unsigned char MahjongHU_CheckZuHeLong(PMahjongHUTypeWrap pwrap);

PMahjongHUTypeWrap MahjongRule_CheckHu(PMahjongHUTypeWrap pnormal,unsigned char *chTest,unsigned short shMask)
{
	MahjongHUTypeWrap first,*pfirst,result,*presult;
	char *allocbuffer = NULL;
//	struct timeval tv;
//	struct timezone tz;
	pfirst = &first;
	presult = &result;
	presult->pnext = NULL;
//	ppresult = NULL;
//	InitBufferForAlloc(allocbuffer,iMaxSize);
	if(pnormal == NULL)
	{
		InitializeNormalWrap(pfirst);
		pfirst->pnext = NULL;
		MahjongHU_Recursion(presult,pfirst,chTest,0,0,allocbuffer);
		
		if(shMask & DJGAME_MAHJONG_RULE_HUMASK_7)
		{
			Initialize7DuiWrap(pfirst);
			pfirst->pnext = NULL;
			MahjongHU_Recursion(presult,pfirst,chTest,0,0,allocbuffer);
		}
		
		if(presult->pnext == NULL && (shMask & DJGAME_MAHJONG_RULE_HUMASK_13))
		{
			Initialize13Wrap(pfirst);
			pfirst->pnext = NULL;
			MahjongHU_Recursion(presult,pfirst,chTest,0,0,allocbuffer);
			if(presult->pnext != NULL && MahjongHU_Check13(presult->pnext) == 0)
				return NULL;
		}
		if(presult->pnext == NULL && (shMask & DJGAME_MAHJONG_RULE_HUMASK_ZUHELONG))
		{
			InitializeHuaLongWrap(pfirst);
			pfirst->pnext = NULL;
			MahjongHU_Recursion(presult,pfirst,chTest,0,0,allocbuffer);
			if(presult->pnext != NULL && MahjongHU_CheckZuHeLong(presult->pnext) == 0)
			{
				return NULL;
			}
		}
		if(presult->pnext == NULL&& (shMask & DJGAME_MAHJONG_RULE_HUMASK_131))
		{
			Initialize19Wrap(pfirst);
			pfirst->pnext = NULL;
			MahjongHU_Recursion(presult,pfirst,chTest,0,0,allocbuffer);
			if(presult->pnext != NULL && MahjongHU_Check19(presult->pnext) == 0)
			{
				return NULL;
			}
		}
	}else //if normal==NULL
	{
		memcpy(pfirst,pnormal,sizeof(MahjongHUTypeWrap));
		pfirst->pnext = NULL;
		MahjongHU_Recursion(presult,pfirst,chTest,0,0,allocbuffer);
		if(presult->pnext == NULL && (shMask & DJGAME_MAHJONG_RULE_HUMASK_ZUHELONG))
		{
			InitializeHuaLongWrap(pfirst);
			pfirst->pnext = NULL;
			MahjongHU_Recursion(presult,pfirst,chTest,0,0,allocbuffer);
			if(presult->pnext != NULL && MahjongHU_CheckZuHeLong(presult->pnext) == 0)
			{
				return NULL;
			}
		}
	}
		
	
//	gettimeofday(&tv,&tz);
//	printf("%d-%d \n",(int)tv.tv_sec,(int)tv.tv_usec);
//	__DebugHUTypeWrap(0,0,presult->pnext,NULL);
	return presult->pnext;
}

unsigned char MahjongHU_Check13(PMahjongHUTypeWrap pwrap)
{
	int i,j;
	unsigned char chValue,chCardNum[16];
	memset(chCardNum,0,16);
	
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			if(pwrap->cardwrap[i].chWrap[j]!=0)
			{
				chValue = GET_MAHJONG_CARDVALUE(pwrap->cardwrap[i].chWrap[j]);
				chCardNum[chValue] ++;
				if(chValue >9 || chCardNum[chValue] > 1)
					return 0;
			}
		}
	}
	return 1;
}

unsigned char MahjongHU_CheckZuHeLong(PMahjongHUTypeWrap pwrap)
{
	int i,j;
	unsigned char chValue,chCardNum[16];
	memset(chCardNum,0,16);
	
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			if(pwrap->cardwrap[i].chWrap[j]!=0)
			{
				chValue = GET_MAHJONG_CARDVALUE(pwrap->cardwrap[i].chWrap[j]);
				chCardNum[chValue] ++;
				if(chValue >9 || chCardNum[chValue] > 1)
					return 0;
			}else
				return 0;
		}
	}
	
	return 1;
}



unsigned char MahjongHU_Check19(PMahjongHUTypeWrap pwrap)
{
	unsigned char chValue;
	
	chValue = GET_MAHJONG_CARDVALUE(pwrap->cardwrap[8].chWrap[0]);
	if(MAHJONG_ISZI(pwrap->cardwrap[8].chWrap[0]) || 
		chValue == 1 || chValue == 9)
		return 1;
	return 0;
}

unsigned char MahjongHU_Recursion(PMahjongHUTypeWrap presult,PMahjongHUTypeWrap pwrap,unsigned char *chTestSrc,
								unsigned char chWrapIndex,unsigned char chCardIndex,char *allocbuffer)
{
	int i;
	MahjongHUTypeWrap wrap,**ppcur;
	unsigned char chCards[20],chSetup,chPreCardWrapName;
	
	memcpy(&wrap,pwrap,sizeof(MahjongHUTypeWrap));
	memcpy(chCards,chTestSrc,14);
	while(chCardIndex < 14)
	{
		if(chCards[chCardIndex] != 0)
		{
			chSetup = 0;
			i = 0;
			chPreCardWrapName = 0;
			
			for(i=0;i<9;i++)
			{
				if( (wrap.cardwrap[i].chCardWrapName != chPreCardWrapName || 
					chPreCardWrapName == MAHJONG_CARDWRAP_NAME_19 ||
					chPreCardWrapName == MAHJONG_CARDWRAP_NAME_HUA )&& 
					wrap.cardwrap[i].chExist == 0)
				{
					chPreCardWrapName = wrap.cardwrap[i].chCardWrapName;
					if(PushCard2Wrap( &(wrap.cardwrap[i]),chCards,chCardIndex) == 1)
					{
						chSetup ++;
						MahjongHU_Recursion(presult,&wrap,chCards,i,chCardIndex+1,allocbuffer);
						memcpy(&wrap,pwrap,sizeof(MahjongHUTypeWrap));
						memcpy(chCards,chTestSrc,14);
					}
					
				}//if(pwrap->cardwrap[i].chExist == 0)
			}//for
			return 0;
		}
			
		chCardIndex ++;
	}
	ppcur = &presult;
	while((*ppcur) != NULL)
		ppcur = &((*ppcur)->pnext);
	//(*ppcur) = (PMahjongHUTypeWrap)BufferAlloc(allocbuffer,sizeof(MahjongHUTypeWrap),false);
	(*ppcur) = (PMahjongHUTypeWrap)malloc(sizeof(MahjongHUTypeWrap));
	if(*ppcur != NULL)
	{
		memcpy((*ppcur),&wrap,sizeof(MahjongHUTypeWrap));
		(*ppcur)->pnext = 0;
	}else
	{
	//	DEBUG_OUTPUT(("MahjongHU_Recursion : BufferAlloc Overflow\n"));
		printf("MahjongHU_Recursion : malloc failed\n");
	}
	
	return 1;
}

unsigned char PushCard2Wrap(PMahjongCardWrap pcardwrap,unsigned char *chTest,unsigned char chCardIndex)
{
	unsigned char chWrapIndex;
	unsigned char chCards[20]={0};
	if(pcardwrap == NULL || pcardwrap->chExist != 0)
		return 0;
	
	memcpy(chCards,chTest,14);
	if(pcardwrap->chCardWrapName == MAHJONG_CARDWRAP_NAME_SHUN)
	{
		if( (chCards[chCardIndex] & 0x30) == MAHJONG_CARD_ZI)
			return 0;
		
		pcardwrap->chWrap[0] = chCards[chCardIndex];
		chCards[chCardIndex] = 0;
//		chNextCard = pcardwrap->chWrap[0] +1;
		chWrapIndex = 1;
		chCardIndex++;
		while(chCardIndex<14)
		{
			if(chCards[chCardIndex] == pcardwrap->chWrap[0] + chWrapIndex)
			{
				pcardwrap->chWrap[chWrapIndex] = chCards[chCardIndex];
				chCards[chCardIndex] = 0;
				if(chWrapIndex == 2)
				{
					pcardwrap->chExist = 1;
					memcpy(chTest,chCards,14);
					return 1;
				}
				chWrapIndex ++;
			}else if(chCards[chCardIndex] > pcardwrap->chWrap[0] + chWrapIndex)
			{
				pcardwrap->chWrap[0] = 0;
				pcardwrap->chWrap[1] = 0;
				pcardwrap->chWrap[2] = 0;
				return 0;
			}
			chCardIndex ++;
		}
		return 0;
	}
	
//	memcpy(chCards,chTest,14);
	if(pcardwrap->chCardWrapName == MAHJONG_CARDWRAP_NAME_KE || pcardwrap->chCardWrapName == MAHJONG_CARDWRAP_NAME_DUI)
	{
		pcardwrap->chWrap[0] = chCards[chCardIndex];
		chCards[chCardIndex] = 0;
//		chNextCard = pcardwrap->chWrap[0] +1;
		chWrapIndex = 1;
		chCardIndex++;
		while(chCardIndex<14)
		{
			if(chCards[chCardIndex] == pcardwrap->chWrap[0])
			{
				pcardwrap->chWrap[chWrapIndex] = chCards[chCardIndex];
				chCards[chCardIndex] = 0;
				if( (pcardwrap->chCardWrapName == MAHJONG_CARDWRAP_NAME_KE && chWrapIndex == 2)
					|| (pcardwrap->chCardWrapName == MAHJONG_CARDWRAP_NAME_DUI) )
				{
					pcardwrap->chExist = 1;
					memcpy(chTest,chCards,14);
					return 1;
				}
				chWrapIndex ++;
			}else
			{
				pcardwrap->chWrap[0] = 0;
				pcardwrap->chWrap[1] = 0;
				pcardwrap->chWrap[2] = 0;
				return 0;
			}
			chCardIndex ++;
		}
	}
	
	if(pcardwrap->chCardWrapName == MAHJONG_CARDWRAP_NAME_19)
	{
		if(chCards[chCardIndex] == ((pcardwrap->chClass-1)<<4)+pcardwrap->chMin)
		{
			pcardwrap->chWrap[0] = chCards[chCardIndex];
			pcardwrap->chExist = 1;
			chTest[chCardIndex] = 0;
			return 1;
		}
		return 0;
	}
	
	
	if(pcardwrap->chCardWrapName == MAHJONG_CARDWRAP_NAME_HUA)
	{
		if( (chCards[chCardIndex] & 0x30) != ((pcardwrap->chClass-1)<<4))
			return 0;
		
		pcardwrap->chWrap[0] = chCards[chCardIndex];
		pcardwrap->chWrap[1] = 0;
		pcardwrap->chWrap[2] = 0;
		chCards[chCardIndex] = 0;
		chCardIndex++;
		while(chCardIndex<14)
		{
			if( (chCards[chCardIndex] & 0x30) != (pcardwrap->chWrap[0] & 0x30))
				break;
			if(pcardwrap->chWrap[1] ==  0 && pcardwrap->chWrap[0]+3 == chCards[chCardIndex])
			{
				pcardwrap->chWrap[1] = chCards[chCardIndex];
				chCards[chCardIndex] = 0;
			}
			if(pcardwrap->chWrap[2] ==  0 && pcardwrap->chWrap[0]+6 == chCards[chCardIndex])
			{
				pcardwrap->chWrap[2] = chCards[chCardIndex];
				chCards[chCardIndex] = 0;
			}
			chCardIndex ++;
		}
		if( (pcardwrap->chWrap[1] ==  0 && (pcardwrap->chPageReleative[1] & MAHJONG_WRAP_RELEATIVE_MUST) )
			||	(pcardwrap->chWrap[2] ==  0 && (pcardwrap->chPageReleative[2] & MAHJONG_WRAP_RELEATIVE_MUST) ))
		{
			pcardwrap->chWrap[0] = 0;
			pcardwrap->chWrap[1] = 0;
			pcardwrap->chWrap[2] = 0;
			return 0;;
		}
		pcardwrap->chExist = 1;
		memcpy(chTest,chCards,14);
		return 1;
	}
	
	if(pcardwrap->chCardWrapName == MAHJONG_CARDWRAP_NAME_PINHUA)
	{
		if( (chCards[chCardIndex] & 0x30) != MAHJONG_CARD_ZI || chCards[chCardIndex] > pcardwrap->chMax
			|| chCards[chCardIndex] < pcardwrap->chMin)
			return 0;
		
		pcardwrap->chWrap[0] = 0;
		pcardwrap->chWrap[1] = 0;
		pcardwrap->chWrap[2] = 0;
		pcardwrap->chWrap[3] = 0;
		
		pcardwrap->chWrap[chCards[chCardIndex] - pcardwrap->chMin] = chCards[chCardIndex];
		chCards[chCardIndex] = 0;
		chCardIndex++;
		while(chCardIndex<14)
		{
			if( (chCards[chCardIndex] & 0x30) != MAHJONG_CARD_ZI  || chCards[chCardIndex] > pcardwrap->chMax)
				break;
			if(pcardwrap->chWrap[chCards[chCardIndex] - pcardwrap->chMin] == 0)
			{
				pcardwrap->chWrap[chCards[chCardIndex] - pcardwrap->chMin] = chCards[chCardIndex];
				chCards[chCardIndex] = 0;
			}
			chCardIndex ++;
		}
		pcardwrap->chExist = 1;
		memcpy(chTest,chCards,14);
		return 1;
	}
	
	if(pcardwrap->chCardWrapName == MAHJONG_CARDWRAP_NAME_ANYC)
	{
		pcardwrap->chWrap[0] = chCards[chCardIndex];
		chTest[chCardIndex] =0;
		pcardwrap->chExist = 1;
		return 1;
	}
	return 0;
	
}




void InitializeNormalWrap(PMahjongHUTypeWrap pnormal)
{
	int i;
	memset(pnormal,0,sizeof(MahjongHUTypeWrap));
	pnormal->chHuType = MAHJONG_TYPEWRAP_NORMAL;
	pnormal->chMaxCardWraps = 4;
	for(i=0;i<4;i++)
	{//shun
		pnormal->cardwrap[i].chPages = 3;
		pnormal->cardwrap[i].chCardWrapName = MAHJONG_CARDWRAP_NAME_SHUN;
		pnormal->cardwrap[i].chPageReleative[0] =  MAHJONG_WRAP_RELEATIVE_MUST;;
		pnormal->cardwrap[i].chPageReleative[1] = 1 | MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPageReleative[2] = 2 | MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPageReleative[3] =  MAHJONG_WRAP_RELEATIVE_NULL;
	}
	
	for(i=4;i<8;i++)
	{//KE
		pnormal->cardwrap[i].chPages = 3;
		pnormal->cardwrap[i].chCardWrapName = MAHJONG_CARDWRAP_NAME_KE;
		pnormal->cardwrap[i].chPageReleative[0]= MAHJONG_WRAP_RELEATIVE_MUST;;
		pnormal->cardwrap[i].chPageReleative[1]= MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPageReleative[2]= MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPageReleative[3]= 0;//option = gang
	}
	pnormal->chCardWrapOption[8] = MAHJONG_WRAP_RELEATIVE_MUST;
	pnormal->cardwrap[8].chPages = 2;
	pnormal->cardwrap[8].chCardWrapName = MAHJONG_CARDWRAP_NAME_DUI;
	pnormal->cardwrap[8].chPageReleative[0]= MAHJONG_WRAP_RELEATIVE_MUST;
	pnormal->cardwrap[8].chPageReleative[1]= MAHJONG_WRAP_RELEATIVE_MUST;
	pnormal->cardwrap[8].chPageReleative[2]= MAHJONG_WRAP_RELEATIVE_NULL;
	pnormal->cardwrap[8].chPageReleative[3]= MAHJONG_WRAP_RELEATIVE_NULL;
}

void Initialize7DuiWrap(PMahjongHUTypeWrap pnormal)
{
	int i;
	memset(pnormal,0,sizeof(MahjongHUTypeWrap));
	pnormal->chHuType = MAHJONG_TYPEWRAP_7DUI;
	pnormal->chMaxCardWraps = 7;
	
	for(i=0;i<7;i++)
	{
		pnormal->chCardWrapOption[i] = MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPages = 2;
		pnormal->cardwrap[i].chCardWrapName = MAHJONG_CARDWRAP_NAME_DUI;
		pnormal->cardwrap[i].chPageReleative[0]= MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPageReleative[1]= MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPageReleative[2]= MAHJONG_WRAP_RELEATIVE_NULL;
		pnormal->cardwrap[i].chPageReleative[3]= MAHJONG_WRAP_RELEATIVE_NULL;
	}
}

void Initialize13Wrap(PMahjongHUTypeWrap pnormal)
{
	int i;
	memset(pnormal,0,sizeof(MahjongHUTypeWrap));
	pnormal->chHuType = MAHJONG_TYPEWRAP_13;
	pnormal->chMaxCardWraps = 9;
	for(i=0;i<3;i++)
	{
		pnormal->cardwrap[i].chPages = 3;
		pnormal->cardwrap[i].chClass = i+2;
		pnormal->cardwrap[i].chCardWrapName = MAHJONG_CARDWRAP_NAME_HUA;
		pnormal->cardwrap[i].chPageReleative[0] = 0;
		pnormal->cardwrap[i].chPageReleative[1] = 3;
		pnormal->cardwrap[i].chPageReleative[2] = 6;
		pnormal->cardwrap[i].chPageReleative[3] =  MAHJONG_WRAP_RELEATIVE_NULL;
	}
	pnormal->cardwrap[3].chPages = 4;
	pnormal->cardwrap[3].chClass = 1;
	pnormal->cardwrap[3].chMin = 1;
	pnormal->cardwrap[3].chMax = 4;
	pnormal->cardwrap[3].chCardWrapName = MAHJONG_CARDWRAP_NAME_PINHUA;
	pnormal->cardwrap[3].chPageReleative[0] = 0;
	pnormal->cardwrap[3].chPageReleative[1] = 1;
	pnormal->cardwrap[3].chPageReleative[2] = 2;
	pnormal->cardwrap[3].chPageReleative[3] = 3;
	
	pnormal->cardwrap[4].chPages = 3;
	pnormal->cardwrap[4].chClass = 1;
	pnormal->cardwrap[4].chMin = 5;
	pnormal->cardwrap[4].chMax = 7;
	pnormal->cardwrap[4].chCardWrapName = MAHJONG_CARDWRAP_NAME_PINHUA;
	pnormal->cardwrap[4].chPageReleative[0] = 0;
	pnormal->cardwrap[4].chPageReleative[1] = 1;
	pnormal->cardwrap[4].chPageReleative[2] = 2;
	pnormal->cardwrap[4].chPageReleative[3] = MAHJONG_WRAP_RELEATIVE_NULL;
	
}

void InitializeHuaLongWrap(PMahjongHUTypeWrap pnormal)
{
	int i;
	memset(pnormal,0,sizeof(MahjongHUTypeWrap));
	pnormal->chHuType = MAHJONG_TYPEWRAP_HUALONG;
	pnormal->chMaxCardWraps = 5;
	for(i=0;i<3;i++)
	{
		pnormal->chCardWrapOption[i] = MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPages = 3;
		pnormal->cardwrap[i].chClass = i+2;
		pnormal->cardwrap[i].chCardWrapName = MAHJONG_CARDWRAP_NAME_HUA;
		pnormal->cardwrap[i].chPageReleative[0] = 0|MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPageReleative[1] = 3|MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPageReleative[2] = 6|MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPageReleative[3] =  MAHJONG_WRAP_RELEATIVE_NULL;
	}
	
	pnormal->cardwrap[3].chPages = 4;
	pnormal->cardwrap[3].chCardWrapName = MAHJONG_CARDWRAP_NAME_KE;
	pnormal->cardwrap[3].chPageReleative[0] = MAHJONG_WRAP_RELEATIVE_MUST;
	pnormal->cardwrap[3].chPageReleative[1] = MAHJONG_WRAP_RELEATIVE_MUST;
	pnormal->cardwrap[3].chPageReleative[2] = MAHJONG_WRAP_RELEATIVE_MUST;
	pnormal->cardwrap[3].chPageReleative[3] = 0;
	
	pnormal->cardwrap[4].chPages = 3;
	pnormal->cardwrap[4].chCardWrapName = MAHJONG_CARDWRAP_NAME_SHUN;
	pnormal->cardwrap[4].chPageReleative[0] = 0;
	pnormal->cardwrap[4].chPageReleative[1] = 1;
	pnormal->cardwrap[4].chPageReleative[2] = 2;
	pnormal->cardwrap[4].chPageReleative[3] = MAHJONG_WRAP_RELEATIVE_NULL;
	
	pnormal->cardwrap[5].chPages = 2;
	pnormal->chCardWrapOption[5] = MAHJONG_WRAP_RELEATIVE_MUST;
	pnormal->cardwrap[5].chCardWrapName = MAHJONG_CARDWRAP_NAME_DUI;
	pnormal->cardwrap[5].chPageReleative[0] = MAHJONG_WRAP_RELEATIVE_MUST;
	pnormal->cardwrap[5].chPageReleative[1] = MAHJONG_WRAP_RELEATIVE_MUST;
	pnormal->cardwrap[5].chPageReleative[2] = MAHJONG_WRAP_RELEATIVE_NULL;
	pnormal->cardwrap[5].chPageReleative[3] = MAHJONG_WRAP_RELEATIVE_NULL;
	
}


void Initialize19Wrap(PMahjongHUTypeWrap pnormal)
{
	int i;
	memset(pnormal,0,sizeof(MahjongHUTypeWrap));
	pnormal->chHuType = MAHJONG_TYPEWRAP_19;
	pnormal->chMaxCardWraps = 9;
	for(i=0;i<3;i++)
	{
		pnormal->cardwrap[i].chPages = 1;
		pnormal->cardwrap[i].chCardWrapName = MAHJONG_CARDWRAP_NAME_19;
		pnormal->cardwrap[i].chClass = i+2;
		pnormal->cardwrap[i].chMin = 1;
		pnormal->cardwrap[i].chMax = 1;
		pnormal->cardwrap[i].chPageReleative[0] = MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPageReleative[1] = MAHJONG_WRAP_RELEATIVE_NULL;
		pnormal->cardwrap[i].chPageReleative[2] = MAHJONG_WRAP_RELEATIVE_NULL;
		pnormal->cardwrap[i].chPageReleative[3] = MAHJONG_WRAP_RELEATIVE_NULL;
	}
	
	for(i=3;i<6;i++)
	{
		pnormal->cardwrap[i].chPages = 1;
		pnormal->cardwrap[i].chCardWrapName = MAHJONG_CARDWRAP_NAME_19;
		pnormal->cardwrap[i].chClass = i-1;
		pnormal->cardwrap[i].chMin = 9;
		pnormal->cardwrap[i].chMax = 9;
		pnormal->cardwrap[i].chPageReleative[0] = MAHJONG_WRAP_RELEATIVE_MUST;
		pnormal->cardwrap[i].chPageReleative[1] = MAHJONG_WRAP_RELEATIVE_NULL;
		pnormal->cardwrap[i].chPageReleative[2] = MAHJONG_WRAP_RELEATIVE_NULL;
		pnormal->cardwrap[i].chPageReleative[3] = MAHJONG_WRAP_RELEATIVE_NULL;
	}
	
	pnormal->cardwrap[6].chPages = 4;
	pnormal->cardwrap[6].chClass = 1;
	pnormal->cardwrap[6].chMin = 1;
	pnormal->cardwrap[6].chMax = 4;
	pnormal->cardwrap[6].chCardWrapName = MAHJONG_CARDWRAP_NAME_PINHUA;
	pnormal->cardwrap[6].chPageReleative[0] = 0;
	pnormal->cardwrap[6].chPageReleative[1] = 1;
	pnormal->cardwrap[6].chPageReleative[2] = 2;
	pnormal->cardwrap[6].chPageReleative[3] = 3;
	
	pnormal->cardwrap[7].chPages = 3;
	pnormal->cardwrap[7].chClass = 1;
	pnormal->cardwrap[7].chMin = 5;
	pnormal->cardwrap[7].chMax = 7;
	pnormal->cardwrap[7].chCardWrapName = MAHJONG_CARDWRAP_NAME_PINHUA;
	pnormal->cardwrap[7].chPageReleative[0] = 0;
	pnormal->cardwrap[7].chPageReleative[1] = 1;
	pnormal->cardwrap[7].chPageReleative[2] = 2;
	pnormal->cardwrap[7].chPageReleative[3] = MAHJONG_WRAP_RELEATIVE_NULL;
	
	pnormal->cardwrap[8].chPages = 1;
	pnormal->cardwrap[8].chCardWrapName = MAHJONG_CARDWRAP_NAME_ANYC;
	pnormal->cardwrap[8].chPageReleative[0] = 0;
	pnormal->cardwrap[8].chPageReleative[1] = MAHJONG_WRAP_RELEATIVE_NULL;
	pnormal->cardwrap[8].chPageReleative[2] = MAHJONG_WRAP_RELEATIVE_NULL;
	pnormal->cardwrap[8].chPageReleative[3] = MAHJONG_WRAP_RELEATIVE_NULL;
	
}



//////////////////////////////////////////
/*
void __DebugHUTypeWrap(int id,int id2,PMahjongHUTypeWrap pnormal,PMahjongHUTypeWrap pnew)
{
	PMahjongHUTypeWrap pwrap;
	FILE *f;
	int i,j;

	char buffer[2000]={0},temp[10]={0};
	f = fopen("./debug.res","a+");
	pwrap = pnormal;
	while(pwrap != NULL)
	{
		sprintf(buffer,"%d ,%d ",id,id2);
		switch(pwrap->chHuType)
		{
			case MAHJONG_TYPEWRAP_NORMAL :
				strcat(buffer,"MAHJONG_TYPEWRAP_NORMAL :");
				break;
			case MAHJONG_TYPEWRAP_7DUI :
				strcat(buffer,"MAHJONG_TYPEWRAP_7DUI :");
				break;
			case MAHJONG_TYPEWRAP_13 :
				strcat(buffer,"MAHJONG_TYPEWRAP_13 :");
				break;
			case MAHJONG_TYPEWRAP_19 :
				strcat(buffer,"MAHJONG_TYPEWRAP_19 :");
				break;
			case MAHJONG_TYPEWRAP_HUALONG :
				strcat(buffer,"MAHJONG_TYPEWRAP_HUALONG :");
				break;
		}
		for(i=0;i<9;i++)
		{
			if(pwrap->cardwrap[i].chExist == 1)
			{
				strcat(buffer," { ");
				for(j=0;j<4;j++)
				{
					sprintf(temp,"%02x ",pwrap->cardwrap[i].chWrap[j]);
					strcat(buffer,temp);
				}
				strcat(buffer," } ,");
			}
		}
		if(pwrap == pnew )
			strcat(buffer,"--delete");
		strcat(buffer,"\n");
		fwrite(buffer,strlen(buffer),1,f);
		pwrap = pwrap->pnext;
	}
	strcpy(buffer,"\n\n**********************\n\n");
	fwrite(buffer,strlen(buffer),1,f);
	close(f);
}
*/
