/*
 * class_definitions.h
 *
 *  Created on: Mar 26, 2014
 *      Author: deepan


Contains the following class definitions:

1. Classes for messages
2. Class for states
3. Class for features that compute averages based on a fixed future time
4. Class for features that compute averages conditional on mid-price change
5. Class for time stamps and to manipulate them.
 */
#pragma once
#ifndef CLASS_DEFINITIONS_H_
#define CLASS_DEFINITIONS_H_
#define NINE_THIRTY_AM 34201
#define TEN_AM 36000
#define TWO_THIRTY_PM 52200
#define THREE_PM 54000
#define THREE_THIRTY_PM 55800
#define FOUR_PM 57600
#define MS_TO_NS 1000000
#define S_TO_NS 1000000000
#define S_TO_MS 1000
#define MAX_IMBALANCE 20


#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<fstream>
#include<cmath>
#include<iomanip>
#include<string>
#include<vector>
#include<map>
#include<iterator>
#include<algorithm>
#include<cstdint>
#include<algorithm>

using namespace std;

//There are 18 message types

class timeStamp{
public:
char message; //Of type 'T'
uint32_t second; //Number of seconds since midnight
void print()
  {
    cout<<endl<<message<<" "<<second;
  }
void convert_endian()
 {
   second = __builtin_bswap32(second);
 }
}__attribute__((packed));
class systemEventMessage{
public:
char message; //Of type 'S'
uint32_t timeStamp; //nanosecond portion of time stamp
char eventCode;
void print()
  {
    printf("\n%c %d %c",message,static_cast<unsigned int>(timeStamp),eventCode);
  }
void convert_endian()
{
  timeStamp = __builtin_bswap32(timeStamp);
}

}__attribute__((packed));
class stockDirectory{
public:
char message; //Of type 'R'
uint32_t timeStamp;//nanosecond portion of the time stamp
char stock[8];
char marketCategory;
char financialStatusIndicator;
uint32_t roundLotSize;
char roundLotsOnly;
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "
    <<stock[0]<<stock[1]<<stock[2]<<stock[3]<<stock[4]<<stock[5]<<stock[6]<<stock[7]
    <<" "<<marketCategory<<" "<<financialStatusIndicator<<" "<<roundLotSize<<" "<<roundLotsOnly;
  }

void convert_endian()
{
  timeStamp = __builtin_bswap32(timeStamp);
  roundLotSize = __builtin_bswap32(roundLotSize);
}
}__attribute__((packed));
class stockTradingAction{
public:
char message;//of type 'H'
uint32_t timeStamp;
char stock[8];
char tradingState;
char reserved;
char reason[4];
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<stock[0]<<stock[1]<<stock[2]<<stock[3]<<stock[4]<<stock[5]<<stock[6]<<stock[7];
  }
void convert_endian()
 {
  timeStamp = __builtin_bswap32(timeStamp);
 }
}__attribute__((packed));
class regShoRestriction{
public:
char message;//of type 'Y'
uint32_t timeStamp;
char stock[8];
char regShoAction;
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<stock[0]<<stock[1]<<stock[2]<<stock[3]<<stock[4]<<stock[5]<<stock[6]<<stock[7];
  }
void convert_endian()
  {
    timeStamp = __builtin_bswap32(timeStamp);
  }
}__attribute__((packed));

class marketParticipantPosition{
public:
char message;//of type 'L'
uint32_t timeStamp;
char mpid[4];
char stock[8];
char primaryMarketMaker;
char marketMakerMode;
char marketParticipantState;

void convert_endian()
{
  timeStamp = __builtin_bswap32(timeStamp);
}

void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<stock[0]<<stock[1]<<stock[2]<<stock[3]<<stock[4]<<stock[5]<<stock[6]<<stock[7];
  }

}__attribute__((packed));

class addOrderMessage{
public:
char message;//of type 'A'
uint32_t timeStamp;
uint64_t OID;
char buySellIndicator;
uint32_t shares;
char stock[8];
uint32_t price;
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<stock[0]<<stock[1]<<stock[2]<<stock[3]<<stock[4]<<stock[5]<<stock[6]<<stock[7];
    cout<<" "<<price<<" "<<OID<<" "<<shares<<" "<<buySellIndicator;
  }
void convert_endian()
  {
    timeStamp = __builtin_bswap32(timeStamp);
    OID = __builtin_bswap64(OID);
    shares = __builtin_bswap32(shares);
    price = __builtin_bswap32(price);
  }

}__attribute__((packed));

class addOrderMessageAttribution{
public:
char message;//of type 'F'
uint32_t timeStamp;
uint64_t OID;
char buySellIndicator;
uint32_t shares;
char stock[8];
uint32_t price;
char attribution[4];
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<stock[0]<<stock[1]<<stock[2]<<stock[3]<<stock[4]<<stock[5]<<stock[6]<<stock[7];
    cout<<" "<<price<<" "<<OID<<" "<<shares<<" "<<buySellIndicator;
  }
void convert_endian()
 {
   timeStamp = __builtin_bswap32(timeStamp);
   OID = __builtin_bswap64(OID);
   shares = __builtin_bswap32(shares);
   price = __builtin_bswap32(price);
 }

}__attribute__((packed));

class orderExecutedMessage{
public:
char message;//of type 'E'
uint32_t timeStamp;
uint64_t OID;
uint32_t executedShares;
uint64_t matchNumber;
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<OID<<" "<<executedShares<<" "<<matchNumber;
  }
void convert_endian()
  {
    timeStamp = __builtin_bswap32(timeStamp);
    OID = __builtin_bswap64(OID);
    executedShares = __builtin_bswap32(executedShares);
    matchNumber = __builtin_bswap64(matchNumber);
  }
}__attribute__((packed));

class orderExecutedWithPriceMessage{
public:
char message;//of type 'C'
uint32_t timeStamp;
uint64_t OID;
uint32_t executedShares;
uint64_t matchNumber;
char printable;
uint32_t executionPrice;
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<OID<<" "<<executedShares<<" "<<matchNumber<<" "<<printable<<" "<<executionPrice;
  }
void convert_endian()
  {
     timeStamp = __builtin_bswap32(timeStamp);
     OID = __builtin_bswap64(OID);
     executedShares = __builtin_bswap32(executedShares);
     matchNumber = __builtin_bswap64(matchNumber);
     executionPrice = __builtin_bswap32(executionPrice);
  }

}__attribute__((packed));

class orderCancelMessage{
public:
char message;//of type 'X'
uint32_t timeStamp;
uint64_t OID;
uint32_t canceledShares;
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<OID<<" "<<canceledShares;
  }
void convert_endian()
  {
    timeStamp = __builtin_bswap32(timeStamp);  OID = __builtin_bswap64(OID);
    canceledShares = __builtin_bswap32(canceledShares);
  }
}__attribute__((packed));

class orderDeleteMessage{
public:
char message;//of type 'D'
uint32_t timeStamp;
uint64_t OID;
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<OID;
  }
void convert_endian()
  {
    timeStamp = __builtin_bswap32(timeStamp);
    OID = __builtin_bswap64(OID);
  }
}__attribute__((packed));

class orderReplaceMessage{
public:
char message;//of type 'U'
uint32_t timeStamp;
uint64_t originalOID;
uint64_t newOID;
uint32_t shares;
uint32_t price;
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<originalOID<<" "<<newOID<<" "<<shares<<" "<<price;
  }
void convert_endian()
{
  timeStamp = __builtin_bswap32(timeStamp);
  originalOID = __builtin_bswap64(originalOID);
  newOID = __builtin_bswap64(newOID);
  shares = __builtin_bswap32(shares);
  price = __builtin_bswap32(price);
}
}__attribute__((packed));

class tradeMessageNonCross{
public:
char message;//of type 'P'
uint32_t timeStamp;
uint64_t OID;
char buySellIndicator;
uint32_t shares;
char stock[8];
uint32_t price;
uint64_t matchNumber;
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<stock[0]<<stock[1]<<stock[2]<<stock[3]<<stock[4]<<stock[5]<<stock[6]<<stock[7];
  }
void convert_endian()
  {
    timeStamp = __builtin_bswap32(timeStamp);    shares = __builtin_bswap32(shares);
    OID = __builtin_bswap64(OID);                price = __builtin_bswap32(price);
    matchNumber = __builtin_bswap64(matchNumber);
  }
}__attribute__((packed));

class tradeMessageCross{
public:
char message;//of type 'Q'
uint32_t timeStamp;
uint64_t shares;
uint64_t OID;
char stock[8];
uint32_t crossPrice;
uint64_t matchNumber;
char crossType;
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<stock[0]<<stock[1]<<stock[2]<<stock[3]<<stock[4]<<stock[5]<<stock[6]<<stock[7];
  }
void convert_endian()
  {
    timeStamp = __builtin_bswap32(timeStamp);    shares = __builtin_bswap64(shares);
    OID = __builtin_bswap64(OID);                crossPrice = __builtin_bswap32(crossPrice);
    matchNumber = __builtin_bswap64(matchNumber);

  }

}__attribute__((packed));

class brokenTrade{
public:
char message;//of type 'B'
uint32_t timeStamp;
uint64_t matchNumber;
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<matchNumber;
  }
void convert_endian()
  {
    timeStamp = __builtin_bswap32(timeStamp);
    matchNumber = __builtin_bswap64(matchNumber);
  }

}__attribute__((packed));

class netOrderImbalanceIndicator{
public:
char message;//of type 'I'
uint32_t timeStamp;
uint64_t pairedShares;
uint64_t imbalanceShares;
char imbalanceDirection;
char stock[8];
uint32_t fairPrice;
uint32_t nearPrice;
uint32_t currentReferencePrice;
char crossType;
char priceVariationIndicator;
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<stock[0]<<stock[1]<<stock[2]<<stock[3]<<stock[4]<<stock[5]<<stock[6]<<stock[7];
  }
void convert_endian()
  {
    timeStamp = __builtin_bswap32(timeStamp);              pairedShares = __builtin_bswap32(pairedShares);
    imbalanceShares = __builtin_bswap32(imbalanceShares);  fairPrice = __builtin_bswap32(fairPrice);
    nearPrice = __builtin_bswap32(nearPrice);              currentReferencePrice = __builtin_bswap32(currentReferencePrice);
  }
}__attribute__((packed));

class retailPriceImprovementIndicator{
public:
char message; //of type 'N'
uint32_t timeStamp;
char stock[8];
char interestFlag[1];
void print()
  {
    cout<<endl<<message<<" "<<timeStamp<<" "<<stock[0]<<stock[1]<<stock[2]<<stock[3]<<stock[4]<<stock[5]<<stock[6]<<stock[7];
  }

void convert_endian()
 {
  timeStamp = __builtin_bswap32(timeStamp);
 }
}__attribute__((packed));

class order
{
public:
uint64_t OID;
uint32_t price;
uint32_t shares;
char side;
uint32_t nanoSecond;
uint32_t second;
void initialize(uint64_t o, uint32_t p, uint32_t s, char a_side, uint32_t sec, uint32_t ns)
{
  OID = o; price = p; shares = s; side = a_side; nanoSecond = ns; second = sec;
}
};

class marketOrder
{
  public:
  uint32_t shares;
  char side; //It has to be buy or sell.
  void initialize(uint32_t a_sh, char a_si)
   {
     shares = a_sh;
     side = a_si;
   }
  marketOrder()
   {
     shares = 0;
     side  = 'X';
   }
};


class globalTime
{
public:
uint64_t second;
uint64_t nanoSecond;
globalTime()
  {
    second = 0;
    nanoSecond = 0;
  }
globalTime(uint32_t s, uint32_t ns)
  {
    second = s;
    nanoSecond = ns;
  }
void setNano(uint32_t ns)
  {
    nanoSecond = ns;
  }
void initialize(uint32_t s, uint32_t ns)
  {
    second = s;
    nanoSecond = ns;
  }
uint64_t timeStampToMs()
  {
    return ((second*S_TO_NS+nanoSecond)/(MS_TO_NS));
  }
uint64_t timeStampToJump(uint64_t a_jump)//a_jump is in millosecond
  {
    return((second*S_TO_NS+nanoSecond)/(MS_TO_NS*a_jump));
  }
void print()
  {
    cout<<endl<<second<<" "<<nanoSecond;
  }

};

class errorRates{
		public:
		long double meanError;
      long double probError;
      long double conditionalMeanError;
      long double conditionalTimeError; 
 		void initialize(long double a_me, long double a_pe, long double a_cme, long double a_cte)
        {
      		meanError = a_me;
      		probError = a_pe;
      		conditionalMeanError = a_cme;
      		conditionalTimeError = a_cte; 
        }
     errorRates()
 		  {
				meanError = probError = conditionalMeanError = conditionalTimeError = 0;
        }
     void construct()
        {
            meanError = probError = conditionalMeanError = conditionalTimeError = 0;
        }
};

class predictions{
		public:
		bool meanPrediction;
		bool probPrediction;
		bool conditionalMeanPrediction;
		bool conditionalTimePrediction;
 		void initialize(long double a_mp, long double a_pp, long double a_cmp, long double a_ctp)
        {
      		meanPrediction = a_mp;
      		probPrediction = a_pp;
      		conditionalMeanPrediction = a_cmp;
      		conditionalTimePrediction = a_ctp; 
        }
     predictions()
 		  {
				meanPrediction = probPrediction = conditionalMeanPrediction = conditionalTimePrediction = 0;
        }
     void construct()
 		  {
				meanPrediction = probPrediction = conditionalMeanPrediction = conditionalTimePrediction = 0;
		  }

};

class levelOneBook
{
  public:
  uint32_t bidShares;
  uint32_t askShares;
  uint32_t bidPrice;
  uint32_t askPrice;
  predictions instantaneousPrediction;
  void initialize(uint32_t a_bs, uint32_t a_as, uint32_t a_bp, uint32_t a_ap, predictions a_ip)
   {
     bidShares = a_bs;
     askShares = a_as;
     bidPrice = a_bp;
     askPrice = a_ap;
     instantaneousPrediction = a_ip;
   }
  void print()
   {
    cout<<endl<<bidShares<<" "<<askShares<<" "<<bidPrice<<" "<<askPrice;
   }
};

class features
{
  public:
  long double meanMidPriceChange;
  long double probMidPriceChange;
  long double conditionalMeanMidPriceChange;
  long double timeToMidPriceChange;

  features()
   {
    meanMidPriceChange = probMidPriceChange = conditionalMeanMidPriceChange = timeToMidPriceChange = 0;
   }
  void construct()
   {
    meanMidPriceChange = probMidPriceChange = conditionalMeanMidPriceChange = timeToMidPriceChange = 0;
   }
};

class oneDState
{
  public:
  int imbalance;
  uint64_t timeSpan;
  uint64_t numberOfMidPriceChanges;
  features stateFeatures;
  errorRates stateErrorRates;
  oneDState()
   {
     imbalance = 0;
     timeSpan = 0;
     numberOfMidPriceChanges = 0;
     stateFeatures.construct();
     stateErrorRates.construct();
   }
   void initialize(int a_imb, uint64_t a_ts, const features& a_sf, const errorRates& a_ser)
   {
     imbalance = a_imb;
     timeSpan = a_ts;
     stateFeatures = a_sf;
     stateErrorRates = a_ser;
   }
};

class executionSpecifications
{
	public:
	string ticker;
	char buyOrSell;
	uint32_t sharesToExecute;
    uint64_t timeHorizon;
    uint64_t costOfExecution;
    uint64_t costOfUniform;
    uint32_t sharesExecuted;
    uint32_t uniformSharesExecuted;
    uint64_t jumpTime;


	void initialize(string a_tick, char a_bos, uint32_t a_sh, uint64_t a_th, uint64_t a_ce, uint64_t a_cu, uint32_t a_se, uint32_t a_use, uint64_t a_jt)
     {
	   ticker = a_tick;
	   buyOrSell = a_bos;
       sharesToExecute = a_sh;
       timeHorizon = a_th;
	   costOfExecution = a_ce;
       costOfUniform = a_cu;
       sharesExecuted = a_se;
       uniformSharesExecuted = a_use;
	   jumpTime = a_jt;
       return;
     }
   void print()
   {
	   cout<<endl<<ticker<<" "<<sharesToExecute<<" "<<timeHorizon<<" "<<costOfExecution<<" "<<costOfUniform<<" "<<sharesExecuted<<" "<<uniformSharesExecuted<<" "<<jumpTime;
   }
};

#endif /* CLASS_DEFINITIONS_H_ */

