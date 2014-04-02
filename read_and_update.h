/*
 * read_and_update.h
 *
 *  Created on: Mar 26, 2014
 *      Author: deepan

//Takes in the message code, reads the message and updates the book.


 */

#ifndef READ_AND_UPDATE_H_
#define READ_AND_UPDATE_H_




#endif /* READ_AND_UPDATE_H_ */
#pragma once
#include "class_definitions.h"

void read_and_update(const char& a_typeOfMessage, const char* a_buffer, int& a_buffer_index, map<uint32_t, map<uint64_t, order> >&a_BB,  map<uint32_t, map<uint64_t, order> >&a_AB,  map<uint64_t, order>& a_OID_to_properties, globalTime& a_globalTime)
{
switch(a_typeOfMessage)
        {
           case 'T': timeStamp o_timeStamp;
                     memmove((char*)&o_timeStamp,&a_buffer[a_buffer_index],sizeof(timeStamp));
                     a_globalTime.initialize(o_timeStamp.second,0);
                     a_buffer_index+=sizeof(timeStamp);
                     break;
           case 'S': systemEventMessage o_systemEventMessage;
                     memmove((char*)&o_systemEventMessage,&a_buffer[a_buffer_index],sizeof(systemEventMessage));
                     a_globalTime.setNano(o_systemEventMessage.timeStamp);
                     a_buffer_index+=sizeof(o_systemEventMessage);
                     //o_systemEventMessage.print();
                     break;
           case 'R': stockDirectory o_stockDirectory;
							memmove((char*)&o_stockDirectory,&a_buffer[a_buffer_index],sizeof(stockDirectory));
                     a_globalTime.setNano(o_stockDirectory.timeStamp);
							a_buffer_index+=sizeof(stockDirectory);
                     break;
           case 'H': stockTradingAction o_stockTradingAction;
  							memmove((char*)&o_stockTradingAction,&a_buffer[a_buffer_index],sizeof(stockTradingAction));
                     a_globalTime.setNano(o_stockTradingAction.timeStamp);
							a_buffer_index+=sizeof(stockTradingAction);
                     break;
           case 'Y': regShoRestriction o_regShoRestriction;
                     memmove((char*)&o_regShoRestriction,&a_buffer[a_buffer_index],sizeof(regShoRestriction));
                     a_globalTime.setNano(o_regShoRestriction.timeStamp);
                     a_buffer_index+=sizeof(regShoRestriction);
                     break;
           case 'L': marketParticipantPosition o_marketParticipantPosition;
							memmove((char*)&o_marketParticipantPosition,&a_buffer[a_buffer_index],sizeof(marketParticipantPosition));
                     a_globalTime.setNano(o_marketParticipantPosition.timeStamp);
                     a_buffer_index+=sizeof(marketParticipantPosition);
                     break;
           case 'A': {
                    	addOrderMessage o_addOrderMessage;
							memmove((char*)&o_addOrderMessage,&a_buffer[a_buffer_index],sizeof(addOrderMessage));
                     //o_addOrderMessage.print();
                     order tempOrder;
                     tempOrder.initialize(o_addOrderMessage.OID, o_addOrderMessage.price,
                                          o_addOrderMessage.shares, o_addOrderMessage.buySellIndicator,
                                          o_timeStamp.second, o_addOrderMessage.timeStamp);
                     a_OID_to_properties.insert(pair<uint64_t,order>
                          (static_cast<uint64_t>(o_addOrderMessage.OID), tempOrder));

                     if(tempOrder.shares >0) add_order_book_entry(tempOrder,a_BB,a_AB, 'A');
                     a_globalTime.setNano(o_addOrderMessage.timeStamp);
                     a_buffer_index+=sizeof(addOrderMessage);
                     }
                     break;
           case 'F': {
                     addOrderMessageAttribution o_addOrderMessageAttribution;
							memmove((char*)&o_addOrderMessageAttribution,&a_buffer[a_buffer_index],sizeof(addOrderMessageAttribution));
							order tempOrder;
                     tempOrder.initialize(o_addOrderMessageAttribution.OID, o_addOrderMessageAttribution.price,
                                          o_addOrderMessageAttribution.shares, o_addOrderMessageAttribution.buySellIndicator,
														o_timeStamp.second,o_addOrderMessageAttribution.timeStamp);
                     a_OID_to_properties.insert(pair<uint64_t,order>
                          (static_cast<uint64_t>(o_addOrderMessageAttribution.OID), tempOrder));

                     if(tempOrder.shares > 0) add_order_book_entry(tempOrder,a_BB,a_AB, 'F');
                     a_globalTime.setNano(o_addOrderMessageAttribution.timeStamp);
                     a_buffer_index+=sizeof(addOrderMessageAttribution);
                     break;
							}
           case 'E': {
							orderExecutedMessage o_orderExecutedMessage;
							memmove((char*)&o_orderExecutedMessage,&a_buffer[a_buffer_index],sizeof(orderExecutedMessage));
                     map<uint64_t, order>::iterator it;
                     it = a_OID_to_properties.find(static_cast<uint64_t>(o_orderExecutedMessage.OID));
                     if(it == a_OID_to_properties.end())
                        {
                           cout<<endl<<"Executing an order not in the OID map!!";
                        }
                     else
                        {

  									order tempOrder = it->second; //Order that is getting executed
                           bool vp = execute_order_book_entry(tempOrder, o_orderExecutedMessage.executedShares, a_BB, a_AB);
                           if (vp)
                             {
                               delete_dead_orders(tempOrder, a_BB, a_AB);
                             }
                        }
                     a_globalTime.setNano(o_orderExecutedMessage.timeStamp);
                     a_buffer_index+=sizeof(orderExecutedMessage);
                     }
                     break;
           case 'C': {
						   orderExecutedWithPriceMessage o_orderExecutedWithPriceMessage;
							memmove((char*)&o_orderExecutedWithPriceMessage,&a_buffer[a_buffer_index],sizeof(orderExecutedWithPriceMessage));
                     map<uint64_t, order>::iterator it;
                     it = a_OID_to_properties.find(static_cast<uint64_t>(o_orderExecutedWithPriceMessage.OID));
                      if(it == a_OID_to_properties.end())
                        {
                          cout<<endl<<"Executing an order not in the OID map!!";
                        }
                      else
                        {
  									order tempOrder = it->second; //Order that is getting executed
									bool vp = execute_order_book_entry(tempOrder, o_orderExecutedWithPriceMessage.executedShares, a_BB, a_AB);
                        }
                     a_globalTime.setNano(o_orderExecutedWithPriceMessage.timeStamp);
                     a_buffer_index+=sizeof(o_orderExecutedWithPriceMessage);
                     }
                     break;
           case 'X': {
							orderCancelMessage o_orderCancelMessage;
							memmove((char*)&o_orderCancelMessage,&a_buffer[a_buffer_index],sizeof(orderCancelMessage));
                     map<uint64_t, order>::iterator it;
                     it = a_OID_to_properties.find(static_cast<uint64_t>(o_orderCancelMessage.OID));
                      if(it == a_OID_to_properties.end())
                        {
                           cout<<endl<<"Canceling an order not in the OID map!!";
                        }
                      else
                        {
  									order tempOrder = it->second; //Order that is getting canceled
									cancel_order_book_entry(tempOrder, o_orderCancelMessage.canceledShares, a_BB, a_AB);
                        }
                     a_globalTime.setNano(o_orderCancelMessage.timeStamp);
                     a_buffer_index+=sizeof(orderCancelMessage);
                     }
                     break;
           case 'D': {
                     orderDeleteMessage o_orderDeleteMessage;
							memmove((char*)&o_orderDeleteMessage,&a_buffer[a_buffer_index],sizeof(orderDeleteMessage));
                     map<uint64_t, order>::iterator it;
                     it = a_OID_to_properties.find(static_cast<uint64_t>(o_orderDeleteMessage.OID));
 							 if(it == a_OID_to_properties.end())
                        {
                           cout<<endl<<"Deleting an order not in the OID map!!";
                        }
                      else
                        {
  									order tempOrder = it->second; //Order that is getting deleted
									delete_order_book_entry(tempOrder, a_BB, a_AB);
                           a_OID_to_properties.erase(it);
                        }
                     a_globalTime.setNano(o_orderDeleteMessage.timeStamp);
                     a_buffer_index+=sizeof(orderDeleteMessage);
                     }
                     break;
           case 'U': {
							orderReplaceMessage o_orderReplaceMessage;
							memmove((char*)&o_orderReplaceMessage,&a_buffer[a_buffer_index],sizeof(orderReplaceMessage));
                     order deleteOrder;
                     map<uint64_t, order>::iterator it;
                     it = a_OID_to_properties.find(static_cast<uint64_t>(o_orderReplaceMessage.originalOID));
                     if(it == a_OID_to_properties.end())
                        {
                           cout<<endl<<"Replacing an order not in the OID map!!";
                        }
                      else
                        {
  									order tempOrder = it->second; //Order that is getting deleted
                           char originalSide = tempOrder.side;
									delete_order_book_entry(tempOrder, a_BB, a_AB);
                           a_OID_to_properties.erase(it);
    								tempOrder.initialize(o_orderReplaceMessage.newOID, o_orderReplaceMessage.price,
                                          o_orderReplaceMessage.shares, originalSide,
														o_timeStamp.second, o_orderReplaceMessage.timeStamp);
    		                  a_OID_to_properties.insert(pair<uint64_t,order>
          										                (static_cast<uint64_t>(o_orderReplaceMessage.newOID), tempOrder));
                           /*if(o_orderReplaceMessage.newOID==6965068 || o_orderReplaceMessage.newOID==6959552)
                             {
                               cout<<endl<<"found:"; o_orderReplaceMessage.print();
                              }*/
          		            add_order_book_entry(tempOrder,a_BB,a_AB, 'A');
                        }
                     a_globalTime.setNano(o_orderReplaceMessage.timeStamp);
                     a_buffer_index+=sizeof(orderReplaceMessage);
                     }
                     break;
           case 'P': tradeMessageNonCross o_tradeMessageNonCross;
							memmove((char*)&o_tradeMessageNonCross,&a_buffer[a_buffer_index],sizeof(tradeMessageNonCross));
                     a_globalTime.setNano(o_tradeMessageNonCross.timeStamp);
                     a_buffer_index+=sizeof(tradeMessageNonCross);
                     break;
           case 'Q': tradeMessageCross o_tradeMessageCross;
							memmove((char*)&o_tradeMessageCross,&a_buffer[a_buffer_index],sizeof(tradeMessageCross));
                     a_globalTime.setNano(o_tradeMessageCross.timeStamp);
                     a_buffer_index+=sizeof(tradeMessageCross);
                     break;
           case 'B': brokenTrade o_brokenTrade;
							memmove((char*)&o_brokenTrade,&a_buffer[a_buffer_index],sizeof(brokenTrade));
                     a_globalTime.setNano(o_brokenTrade.timeStamp);
                     a_buffer_index+=sizeof(brokenTrade);
                     //These messages can be ignored since we are only using ITCH data. Don't even have to convert endian
                     break;
           case 'I': netOrderImbalanceIndicator o_netOrderImbalanceIndicator;
							memmove((char*)&o_netOrderImbalanceIndicator,&a_buffer[a_buffer_index],sizeof(netOrderImbalanceIndicator));
                     a_globalTime.setNano(o_netOrderImbalanceIndicator.timeStamp);
                     a_buffer_index+=sizeof(netOrderImbalanceIndicator);
                     break;
           case 'N':	retailPriceImprovementIndicator o_retailPriceImprovementIndicator;
							memmove((char*)&o_retailPriceImprovementIndicator,&a_buffer[a_buffer_index],sizeof(retailPriceImprovementIndicator));
                     a_globalTime.setNano(o_retailPriceImprovementIndicator.timeStamp);
                     a_buffer_index+=sizeof(retailPriceImprovementIndicator);
                     break;
           default:  break;
        }
 return;
}
