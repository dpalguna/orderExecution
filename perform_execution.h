/*
 * perform_execution.h
 *
 *  Created on: Mar 26, 2014
 *      Author: deepan


   Performs the order execution after the time window has been reached
 */
#pragma once
#ifndef PERFORM_EXECUTION_H_
#define PERFORM_EXECUTION_H_

#include "class_definitions.h"
#include "read_next_message.h"
#include "read_and_update.h"
#include "compute_routines.h"
#include "book_routines.h"
void perform_execution(map<uint32_t, map<uint64_t, order> >& a_bidBook, map<uint32_t, map<uint64_t, order> >& a_askBook, 
                       map<uint64_t, order>& a_OID_to_properties, globalTime& a_globalTime,
                       vector<oneDState>& a_positiveStates, vector<oneDState>& a_negativeStates, oneDState& a_balancedState,
                       const char* a_buffer, int& a_buffer_index, executionSpecifications a_execSpecs)
{
  char f_typeOfMessage;
  bool f_changeDetection;
  bool f_signPrediction;
  levelOneBook f_levelOneBook;
  predictions f_predictions;
  errorRates f_errorRates;
  marketOrder f_MO;
  uint64_t f_jumpTime = a_execSpecs.jumpTime;
  uint32_t numSteps = 0;
  uint32_t uniformShares = round_to_nearest_int(static_cast<long double>(a_execSpecs.sharesToExecute)/static_cast<long double>(numSteps));
  uint32_t f_instUniformShares = 0, f_instShares = 0;
  while(a_globalTime.second != FOUR_PM)
   {
 	 	f_typeOfMessage = a_buffer[a_buffer_index];
 	 	read_and_update(f_typeOfMessage, a_buffer, a_buffer_index, a_bidBook, a_askBook, a_OID_to_properties,a_globalTime);
 	 	globalTime f_nextGlobalTime;
 	 	f_nextGlobalTime = a_globalTime;
 	 	read_next_message(a_buffer[a_buffer_index], a_buffer, a_buffer_index, f_nextGlobalTime);//Simply reads the next msg. Pointer is not moved
 	 	if(a_globalTime.second < FOUR_PM && f_nextGlobalTime.second < FOUR_PM)
 	   	{
 	   	    if(a_globalTime.timeStampToJump(f_jumpTime) != f_nextGlobalTime.timeStampToJump(f_jumpTime) 
						&& numSteps < a_execSpecs.timeHorizon/a_execSpecs.jumpTime)
 	   	       {
 							f_levelOneBook = bookToLevelOne(a_bidBook, a_askBook);
							f_predictions = predict(f_levelOneBook, a_positiveStates, a_negativeStates, a_balancedState, f_jumpTime);
							f_errorRates = get_error_rates(f_levelOneBook, a_positiveStates, a_negativeStates, a_balancedState);
                     f_changeDetection = ((f_errorRates.probError < f_errorRates.conditionalTimeError)
																?f_predictions.probPrediction:f_predictions.conditionalTimePrediction);
							f_signPrediction = ((f_errorRates.meanError<f_errorRates.conditionalMeanError)?
                                                f_predictions.meanPrediction:f_predictions.conditionalMeanPrediction);
                     if(f_changeDetection == 0)
                       {
                          f_instShares = min(uniformShares, a_execSpecs.sharesToExecute-a_execSpecs.sharesExecuted);
                          f_MO.initialize(f_instShares,'B');
                       }
                     else
                       {
                          if(f_signPrediction == 0)
                             {
                                f_MO.initialize(0,'B');
                             }
                          else
                             {
		                          f_instShares = min(2*uniformShares, a_execSpecs.sharesToExecute-a_execSpecs.sharesExecuted);
		                          f_MO.initialize(f_instShares,'B');
                             }
                       }
                    a_execSpecs.costOfExecution+=execute_shadow_market_order(a_bidBook,a_askBook,f_MO);
                    a_execSpecs.sharesExecuted+=f_instShares;
                    f_instShares = min(uniformShares, a_execSpecs.sharesToExecute-a_execSpecs.uniformSharesExecuted);
                    f_MO.initialize(f_instShares,'B');
                    a_execSpecs.costOfUniform+=execute_shadow_market_order(a_bidBook,a_askBook,f_MO);
                    numSteps++;
      		    }//Checking for sampling interval
              if(numSteps == a_execSpecs.timeHorizon/a_execSpecs.jumpTime)
                {
						f_instShares = a_execSpecs.sharesToExecute-a_execSpecs.sharesExecuted;
						f_MO.initialize(f_instShares,'B');
                  a_execSpecs.costOfExecution+=execute_shadow_market_order(a_bidBook,a_askBook,f_MO);
                  a_execSpecs.sharesExecuted+=f_instShares;
						f_instShares = a_execSpecs.sharesToExecute-a_execSpecs.uniformSharesExecuted;
						f_MO.initialize(f_instShares,'B');
                  a_execSpecs.costOfUniform+=execute_shadow_market_order(a_bidBook,a_askBook,f_MO);
                  a_execSpecs.uniformSharesExecuted+=f_instShares;
					 }
    		}
   }
  
  return;
}



#endif /* PERFORM_EXECUTION_H_ */


