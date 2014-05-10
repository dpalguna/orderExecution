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
  uint32_t f_numSteps = 0;
  uint32_t f_uniformShares = round_to_nearest_int(static_cast<long double>(a_execSpecs.sharesToExecute)
		                    /(static_cast<long double>(a_execSpecs.timeHorizon)/static_cast<long double>(a_execSpecs.jumpTime)));
  uint32_t f_instShares = 0;

  while(a_globalTime.second != FOUR_PM && f_numSteps < a_execSpecs.timeHorizon/a_execSpecs.jumpTime)
   {

 	 	f_typeOfMessage = a_buffer[a_buffer_index];
 	 	read_and_update(f_typeOfMessage, a_buffer, a_buffer_index, a_bidBook, a_askBook, a_OID_to_properties,a_globalTime);
 	 	globalTime f_nextGlobalTime;
 	 	f_nextGlobalTime = a_globalTime;
 	 	read_next_message(a_buffer[a_buffer_index], a_buffer, a_buffer_index, f_nextGlobalTime);//Simply reads the next msg. Pointer is not moved
 	 	if(a_globalTime.second < FOUR_PM && f_nextGlobalTime.second < FOUR_PM)
 	   	{
 	   	    if(a_globalTime.timeStampToJump(f_jumpTime) != f_nextGlobalTime.timeStampToJump(f_jumpTime) 
						&& f_numSteps < a_execSpecs.timeHorizon/a_execSpecs.jumpTime-1)
 	   	       {
 					f_levelOneBook = bookToLevelOne(a_bidBook, a_askBook);
					f_predictions = predict(f_levelOneBook, a_positiveStates, a_negativeStates, a_balancedState, f_jumpTime);
					f_errorRates = get_error_rates(f_levelOneBook, a_positiveStates, a_negativeStates, a_balancedState);
                    f_changeDetection = ((f_errorRates.probError < f_errorRates.conditionalTimeError)
																?f_predictions.probPrediction:f_predictions.conditionalTimePrediction);
					f_signPrediction = ((f_errorRates.meanError<f_errorRates.conditionalMeanError)?
                                                f_predictions.meanPrediction:f_predictions.conditionalMeanPrediction);
					//printf("%Lf %Lf %Lf %Lf\n", f_errorRates.probError, f_errorRates.conditionalTimeError, f_errorRates.meanError, f_errorRates.conditionalMeanError);
					f_changeDetection = ((min(f_errorRates.probError, f_errorRates.conditionalTimeError) > 0.9)
							|| (min(f_errorRates.meanError, f_errorRates.conditionalMeanError) > 0.9))?0:f_changeDetection;
                     if(f_changeDetection == 0)//predict flat
                       {
                          f_instShares = min(f_uniformShares, a_execSpecs.sharesToExecute-a_execSpecs.sharesExecuted);
                          f_MO.initialize(f_instShares,a_execSpecs.buyOrSell);
                       }
                     else//predict change
                       {
                          if(f_signPrediction == 0)//Predict down
                             {
                        	   if(a_execSpecs.buyOrSell == 'B')//buying means slow down
                        	   {
                        		   f_MO.initialize(0,a_execSpecs.buyOrSell);
                        		   f_instShares = 0;
                        	   }
                        	   else//selling means accelerate
                        	   {
                        		   f_instShares = min(2*f_uniformShares, a_execSpecs.sharesToExecute-a_execSpecs.sharesExecuted);
                        		   f_MO.initialize(f_instShares,a_execSpecs.buyOrSell);
                        	   }
                             }
                          else//Predict up
                             {
                        	   if(a_execSpecs.buyOrSell == 'B')//buying means accelerate
                        	   {
                        		   f_instShares = min(2*f_uniformShares, a_execSpecs.sharesToExecute-a_execSpecs.sharesExecuted);
                        		   f_MO.initialize(f_instShares,a_execSpecs.buyOrSell);
                        	   }
                        	   else//selling means slow down
                        	   {
                        		   f_MO.initialize(0,a_execSpecs.buyOrSell);
                        		   f_instShares = 0;
                        	   }
                             }
                       }
                    a_execSpecs.costOfExecution+=execute_shadow_market_order(a_bidBook,a_askBook,f_MO);
                    a_execSpecs.sharesExecuted+=f_instShares;
                    f_instShares = min(f_uniformShares, a_execSpecs.sharesToExecute-a_execSpecs.uniformSharesExecuted);
                    f_MO.initialize(f_instShares,a_execSpecs.buyOrSell);
                    a_execSpecs.costOfUniform+=execute_shadow_market_order(a_bidBook,a_askBook,f_MO);
                    a_execSpecs.uniformSharesExecuted+=f_instShares;


                    f_numSteps++;
       		    }//Checking for sampling interval
              if(f_numSteps == a_execSpecs.timeHorizon/a_execSpecs.jumpTime-1)
                {
            	  //cerr<<endl<<"Last step";
				  f_instShares = a_execSpecs.sharesToExecute-a_execSpecs.sharesExecuted;
				  f_MO.initialize(f_instShares,'B');
                  a_execSpecs.costOfExecution+=execute_shadow_market_order(a_bidBook,a_askBook,f_MO);
                  a_execSpecs.sharesExecuted+=f_instShares;
                  cout<<f_instShares;
		          f_instShares = a_execSpecs.sharesToExecute-a_execSpecs.uniformSharesExecuted;
				  f_MO.initialize(f_instShares,'B');
				  //cout<<" "<<f_instShares<<" "<<f_numSteps;
                  a_execSpecs.costOfUniform+=execute_shadow_market_order(a_bidBook,a_askBook,f_MO);
                  a_execSpecs.uniformSharesExecuted+=f_instShares;
                  a_execSpecs.print();
                  f_numSteps++;
				}
    		}
   }
  
  return;
}

#endif /* PERFORM_EXECUTION_H_ */


