/*
 * read_next_message.h
 *
 *  Created on: Mar 26, 2014
 *      Author: deepan

  Takes in the message code and reads the message. Does not move the buffer pointer forward.
 */
#pragma once
#ifndef READ_NEXT_MESSAGE_H_
#define READ_NEXT_MESSAGE_H_
#include "class_definitions.h"


void read_next_message(const char& a_typeOfMessage, const char* a_buffer, int a_buffer_index, globalTime& a_globalTime)
{
switch(a_typeOfMessage)
        {
           case 'T': timeStamp o_timeStamp;
                     memmove((char*)&o_timeStamp,&a_buffer[a_buffer_index],sizeof(timeStamp));
                     a_globalTime.initialize(o_timeStamp.second,0);
                     break;
           case 'S': systemEventMessage o_systemEventMessage;
                     memmove((char*)&o_systemEventMessage,&a_buffer[a_buffer_index],sizeof(systemEventMessage));
                     a_globalTime.setNano(o_systemEventMessage.timeStamp);
                     break;
           case 'R': stockDirectory o_stockDirectory;
							memmove((char*)&o_stockDirectory,&a_buffer[a_buffer_index],sizeof(stockDirectory));
                     a_globalTime.setNano(o_stockDirectory.timeStamp);
                     break;
           case 'H': stockTradingAction o_stockTradingAction;
  							memmove((char*)&o_stockTradingAction,&a_buffer[a_buffer_index],sizeof(stockTradingAction));
                     a_globalTime.setNano(o_stockTradingAction.timeStamp);
                     break;
           case 'Y': regShoRestriction o_regShoRestriction;
                     memmove((char*)&o_regShoRestriction,&a_buffer[a_buffer_index],sizeof(regShoRestriction));
                     a_globalTime.setNano(o_regShoRestriction.timeStamp);
                     break;
           case 'L': marketParticipantPosition o_marketParticipantPosition;
							memmove((char*)&o_marketParticipantPosition,&a_buffer[a_buffer_index],sizeof(marketParticipantPosition));
                     a_globalTime.setNano(o_marketParticipantPosition.timeStamp);
                     break;
           case 'A': {
                    	addOrderMessage o_addOrderMessage;
							memmove((char*)&o_addOrderMessage,&a_buffer[a_buffer_index],sizeof(addOrderMessage));
                     a_globalTime.setNano(o_addOrderMessage.timeStamp);
                     }
                     break;
           case 'F': {
                     addOrderMessageAttribution o_addOrderMessageAttribution;
							memmove((char*)&o_addOrderMessageAttribution,&a_buffer[a_buffer_index],sizeof(addOrderMessageAttribution));
							a_globalTime.setNano(o_addOrderMessageAttribution.timeStamp);
                     break;
							}
           case 'E': {
							orderExecutedMessage o_orderExecutedMessage;
							memmove((char*)&o_orderExecutedMessage,&a_buffer[a_buffer_index],sizeof(orderExecutedMessage));
                     a_globalTime.setNano(o_orderExecutedMessage.timeStamp);
                     }
                     break;
           case 'C': {
						   orderExecutedWithPriceMessage o_orderExecutedWithPriceMessage;
							memmove((char*)&o_orderExecutedWithPriceMessage,&a_buffer[a_buffer_index],sizeof(orderExecutedWithPriceMessage));
                     a_globalTime.setNano(o_orderExecutedWithPriceMessage.timeStamp);
                     }
                     break;
           case 'X': {
							orderCancelMessage o_orderCancelMessage;
							memmove((char*)&o_orderCancelMessage,&a_buffer[a_buffer_index],sizeof(orderCancelMessage));
                     a_globalTime.setNano(o_orderCancelMessage.timeStamp);
                     }
                     break;
           case 'D': {
                     orderDeleteMessage o_orderDeleteMessage;
							memmove((char*)&o_orderDeleteMessage,&a_buffer[a_buffer_index],sizeof(orderDeleteMessage));
                     a_globalTime.setNano(o_orderDeleteMessage.timeStamp);
                     }
                     break;
           case 'U': {
							orderReplaceMessage o_orderReplaceMessage;
							memmove((char*)&o_orderReplaceMessage,&a_buffer[a_buffer_index],sizeof(orderReplaceMessage));
                     a_globalTime.setNano(o_orderReplaceMessage.timeStamp);
                     }
                     break;
           case 'P': tradeMessageNonCross o_tradeMessageNonCross;
							memmove((char*)&o_tradeMessageNonCross,&a_buffer[a_buffer_index],sizeof(tradeMessageNonCross));
                     a_globalTime.setNano(o_tradeMessageNonCross.timeStamp);
                     break;
           case 'Q': tradeMessageCross o_tradeMessageCross;
							memmove((char*)&o_tradeMessageCross,&a_buffer[a_buffer_index],sizeof(tradeMessageCross));
                     a_globalTime.setNano(o_tradeMessageCross.timeStamp);
                     break;
           case 'B': brokenTrade o_brokenTrade;
							memmove((char*)&o_brokenTrade,&a_buffer[a_buffer_index],sizeof(brokenTrade));
                     a_globalTime.setNano(o_brokenTrade.timeStamp);
                     //These messages can be ignored since we are only using ITCH data. Don't even have to convert endian
                     break;
           case 'I': netOrderImbalanceIndicator o_netOrderImbalanceIndicator;
							memmove((char*)&o_netOrderImbalanceIndicator,&a_buffer[a_buffer_index],sizeof(netOrderImbalanceIndicator));
                     a_globalTime.setNano(o_netOrderImbalanceIndicator.timeStamp);
                     break;
           case 'N':	retailPriceImprovementIndicator o_retailPriceImprovementIndicator;
							memmove((char*)&o_retailPriceImprovementIndicator,&a_buffer[a_buffer_index],sizeof(retailPriceImprovementIndicator));
                     a_globalTime.setNano(o_retailPriceImprovementIndicator.timeStamp);
                     break;
           default:  break;
        }
 return;
}



#endif /* READ_NEXT_MESSAGE_H_ */

