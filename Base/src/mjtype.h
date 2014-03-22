#ifndef __MJTYPE_H__
#define __MJTYPE_H__

#include <vector>
#include <set>

#include "mjrule.h"
#include "mjfans.h"

typedef unsigned char							MJFanID;
typedef std::set< MJFanID >						MJFanIDs;
typedef MJFanIDs::iterator 						MJFanIDsIterator;

typedef unsigned char							MJCard;									
typedef std::vector< MJCard >					MJCards;
typedef MJCards::iterator 						MJCardsIterator;
typedef MJCards::const_iterator 				MJCardsConstIterator;

typedef unsigned short							MJSourceCard;									
typedef std::vector< MJSourceCard >				MJSourceCards;
typedef MJSourceCards::iterator 				MJSourceCardsIterator;
	
typedef std::vector< MJCards >					MJCardsGroups;
typedef MJCardsGroups::iterator 				MJCardsGroupsIterator;
typedef MJCardsGroups::const_iterator			MJCardsGroupsConstIterator;
	
typedef std::set< MJCard >						MJCardsSet;
typedef MJCardsSet::iterator					MJCardsSetIterator;  
	
typedef std::multiset< MJCard >					MJCardsMultiSet;
typedef MJCardsMultiSet::iterator				MJCardsMultiSetIterator;

typedef std::vector< MahjongFanResult >			MJFanResults;
typedef MJFanResults::iterator 					MJFanResultsIterator;

typedef std::vector< float >					MJCardsProbs;
	
#define SOURCE_MASK_MO		1
#define SOURCE_MASK_CHI		2
#define SOURCE_MASK_PENG	4
#define SOURCE_MASK_HU		8

#define MAX_STEPS	4
#define MAX_PLANS	40

#endif
