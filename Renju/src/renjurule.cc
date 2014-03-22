#include <iostream>
#include "renjurule.h"

unsigned char RenjuRule_CheckRule(PRenjuRule prule,PRenjuBoard pboard,unsigned char x,unsigned char y,unsigned char chOwner)
{
	if(RenjuRule_GetNodeChip(pboard,x,y) != 0)
		return DJGAME_RENJU_RULE_FAILED;
	RenjuRule_SetNodeChip(pboard,x,y,chOwner);
	if(chOwner ==0 || chOwner > 2)
		return DJGAME_RENJU_RULE_SUCCESS;
	
	int i,dx[4]={1,0,1,1},dy[4]={0,1,1,-1};
	
	unsigned char chSerials,ch6=0;
	for(i=0;i<4;i++)
	{
		chSerials = RenjuRule_GetSerials(pboard,x,y,dx[i],dy[i]);
		if(chSerials >= 5)
		{
			if(chOwner == DJGAME_RENJU_OWNER_WHITE || chSerials == 5)
				return DJGAME_RENJU_RULE_WIN;
			if( prule->chRuleSet & RENJU_RULESET_HASFORBID)
			{
				ch6 ++; //如果5连和长连同时出现则先5胜,因此长连未必为禁手
			}else
				return DJGAME_RENJU_RULE_WIN;
		}
	}
	if(ch6 > 0)
	{
		RenjuRule_SetNodeChip(pboard,x,y,0);
		return DJGAME_RENJU_RULE_FORBID_6;
	}

	return DJGAME_RENJU_RULE_SUCCESS;
}



unsigned char RenjuRule_IsForbid(PRenjuBoard pboard,unsigned char x,unsigned char y)
{
	if(RenjuRule_GetNodeChip(pboard,x,y) != DJGAME_RENJU_OWNER_BLACK)
		return DJGAME_RENJU_RULE_FORBID_NOT;
	int i,dx[8]={1,-1,0,0,1,-1,1,-1},dy[8]={0,0,1,-1,1,-1,-1,1};
	unsigned char cx,cy,tx,ty,chChips,chBlanks,chMaxSerials,chMaxBlankSerials;
	unsigned char ch4=0,chA4=0,chA3=0;
	RenjuRule rule;
	RenjuBoard board;
	bool bRes;
	rule.chRuleSet = RENJU_RULESET_HASFORBID;
	
	for(i=0;i<8;i++)
	{
		memcpy(&board,pboard,sizeof(RenjuBoard));
		cx = x;
		cy = y;
		bRes = RenjuRule_GetOwnerHead(&board,&cx,&cy,-dx[i],-dy[i]);
		if(!bRes)
			return DJGAME_RENJU_RULE_FORBID_NOT;
		
		RenjuRule_GetRowStatus(&board,cx,cy,dx[i],dy[i],5,DJGAME_RENJU_OWNER_BLACK,
						&chChips,&chBlanks,&chMaxSerials,&chMaxBlankSerials);
		//if(chMaxBlankSerials == 1)
		{
			if(chChips == 4 && chBlanks == 1)
			{
				tx = cx;
				ty = cy;
				bRes = RenjuRule_GetFirstBlank(&board,&tx,&ty,dx[i],dy[i],5,DJGAME_RENJU_OWNER_BLACK);
				if(bRes)
				{
					if(RenjuRule_CheckRule(&rule,&board,tx,ty,DJGAME_RENJU_OWNER_BLACK)
								== DJGAME_RENJU_RULE_WIN)
					{
						ch4 ++;
						if(ch4>=2 || chA3> 0)
							return DJGAME_RENJU_RULE_FORBID_44;
						if(chMaxSerials == 4 && !(i&1))
						{
							memcpy(&board,pboard,sizeof(RenjuBoard));
							if(RenjuRule_CheckRule(&rule,&board,cx+dx[i+1],cy+dy[i+1],DJGAME_RENJU_OWNER_BLACK)
								== DJGAME_RENJU_RULE_WIN)
								chA4 ++; //活4
							//跳过同轴的另一方向
							i++;
						}
					}
				}
			}//if(chChips == 4 && chBlanks == 1)
			else if(chChips == 3 && (chMaxSerials == 3 || chMaxBlankSerials==1) && chBlanks == 2)
			{
				tx = cx;
				ty = cy;
				bRes = RenjuRule_GetFirstBlank(&board,&tx,&ty,dx[i],dy[i],5,DJGAME_RENJU_OWNER_BLACK);
				if(bRes)
				{
					if(RenjuRule_CheckRule(&rule,&board,tx,ty,DJGAME_RENJU_OWNER_BLACK) ==
							DJGAME_RENJU_RULE_SUCCESS && 
						RenjuRule_IsForbid(&board,tx,ty) == DJGAME_RENJU_RULE_FORBID_NOT_WITH_A4)
					{
						chA3 ++;
						if(chA3 >=2 || ch4>0 )
							return DJGAME_RENJU_RULE_FORBID_33;
						if( !(i&1) )
							i++;
					}
				}
			}
			
		}//if(chMaxBlankSerials == 1)
	}
	if(chA4 >0)
		return DJGAME_RENJU_RULE_FORBID_NOT_WITH_A4;
	return DJGAME_RENJU_RULE_FORBID_NOT;
}

unsigned char RenjuRule_GetSerials(PRenjuBoard pboard,unsigned char x,unsigned char y,
						int dx,int dy)
{
	unsigned char chSerials=0;
	unsigned char chChip,chOwner = RenjuRule_GetNodeChip(pboard,x,y);
	if(chOwner != DJGAME_RENJU_OWNER_BLACK && chOwner != DJGAME_RENJU_OWNER_WHITE)
		return 0;
	if(RenjuRule_GetOwnerHead(pboard,&x,&y,-dx,-dy))
	{
		do
		{
			chChip = RenjuRule_GetNodeChip(pboard,x,y);
			if(chChip == chOwner)
			{
				chSerials ++;
				x += dx;
				y += dy;
			}
		}while(chChip == chOwner);
	}
	return chSerials;	
}

bool RenjuRule_GetFirstBlank(PRenjuBoard pboard,unsigned char *x,unsigned char *y,
							int dx,int dy,unsigned char chRange,unsigned char chOwner)
{
	int i =0;
	unsigned char chChip;
	for(i=0;i<chRange;i++)
	{
		chChip = RenjuRule_GetNodeChip(pboard,*x,*y);
		if(chChip == 0)
			return true;
		else if(chChip != chOwner)
			return false;
		*x += dx;
		*y += dy;
	}
	return false;
}
bool RenjuRule_GetOwnerHead(PRenjuBoard pboard,unsigned char *x,unsigned char *y,int dx,int dy)
{
	unsigned char chOwner = RenjuRule_GetNodeChip(pboard,*x,*y);
	if(chOwner ==0 || chOwner > 2)
		return false;
	unsigned char chNext=0,cx,cy;
	cx = *x+dx;
	cy = *y+dy;
	do
	{
		chNext = RenjuRule_GetNodeChip(pboard,cx,cy);
		if(chNext == chOwner)
		{
			cx += dx;
			cy += dy;
		}
	}while(chNext == chOwner);
	*x = cx-dx;
	*y = cy-dy;
	return true;
}

unsigned char RenjuRule_GetRowStatus(PRenjuBoard pboard,unsigned char x,unsigned char y,
						int dx,int dy,unsigned char chRange,unsigned char chOwner,
						unsigned char *pchChips,unsigned char *pchBlanks,
						unsigned char *pchMaxSerials,unsigned char *pchMaxBlankSerials)
{
	*pchChips = 0;
	*pchBlanks = 0;
	*pchMaxSerials = 0;
	*pchMaxBlankSerials = 0;
	
	int i;
	unsigned char chCur,chCurMaxS=0,chCurMaxBS=0;
	for(i=0;i<chRange;i++)
	{
		chCur = RenjuRule_GetNodeChip(pboard,x,y);
		if(chCur == chOwner)
		{
			chCurMaxS ++;
			chCurMaxBS = 0;
			if(chCurMaxS > *pchMaxSerials)
				*pchMaxSerials = chCurMaxS;
			(*pchChips)++;
		}else if(chCur == 0)
		{
			chCurMaxS = 0;
			chCurMaxBS ++;
			if(chCurMaxBS > *pchMaxBlankSerials)
				*pchMaxBlankSerials = chCurMaxBS;
			(*pchBlanks)++;
		}else
			break;
		x += dx;
		y += dy;
	}
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////

bool RenjuRule_SetNodeChip(PRenjuBoard pboard,unsigned char x,unsigned char y,unsigned char chOwner)
{
	if(x==0 || y== 0 || x > DJGAME_RENJU_WIDTH || y>DJGAME_RENJU_WIDTH || chOwner > 2)
		return false;
	unsigned char chIndex,chBit;
	chIndex = (y-1)*DJGAME_RENJU_WIDTH+x;
	chBit = (chIndex&3);
	chIndex >>= 2;
	
	switch(chBit)
	{
		case 0 :
			pboard->chBuf[chIndex] &= 0xFC;
			break;
		case 1 :
			pboard->chBuf[chIndex] &= 0xF3;
			chOwner <<= 2;
			break;
		case 2 :
			pboard->chBuf[chIndex] &= 0xCF;
			chOwner <<= 4;
			break;
		case 3 :
			pboard->chBuf[chIndex] &= 0x3F;
			chOwner <<= 6;
			break;
	}
	pboard->chBuf[chIndex] |= chOwner;
		
	return true;
}

char RenjuRule_GetNodeChip(PRenjuBoard pboard,unsigned char x,unsigned char y)
{
	if(x==0 || y== 0 || x > DJGAME_RENJU_WIDTH || y>DJGAME_RENJU_WIDTH)
		return -1;
	unsigned char chIndex,chBit;
	chIndex = (y-1)*DJGAME_RENJU_WIDTH+x;
	chBit = (chIndex&3);
	chIndex >>= 2;
	
	switch(chBit)
	{
		case 0 :
			return (pboard->chBuf[chIndex] & 0x03);
		case 1 :
			return ((pboard->chBuf[chIndex] & 0x0C)>>2);
		case 2 :
			return ((pboard->chBuf[chIndex] & 0x30)>>4);
		case 3 :
			return ((pboard->chBuf[chIndex] & 0xC0)>>6);
	}
	return -1;
	
}
