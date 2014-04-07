/*
 * compute_routines.h
 *
 *  Created on: Mar 26, 2014
 *      Author: deepan


   Contains routines to do feature updates
 */
#pragma once
#ifndef COMPUTE_ROUTINES_H_
#define COMPUTE_ROUTINES_H_

#include "class_definitions.h"

oneDState levelOneBookToState(const levelOneBook& lob);
void updateFeatures(oneDState& a_ODS, const levelOneBook& a_currentLevelOneBook, const levelOneBook& a_pastLevelOneBook);
void updateConditionalFeatures(vector<levelOneBook>& a_closingConditionalBooks, vector<oneDState>& a_positiveStates,
                               vector<oneDState>& a_negativeStates, oneDState& a_balancedState, int a_futureIndex);
predictions predict(uint32_t a_bs, uint32_t a_as, uint32_t a_bp, uint32_t a_ap, 
                    const vector<oneDState>& a_positiveStates, const vector<oneDState>& a_negativeStates, const oneDState& a_balancedState,
                    const uint64_t a_jumpTime);
uint32_t round_to_nearest_int(long double num);


void updateFeatures(vector<oneDState>& a_positiveStates, vector<oneDState>& a_negativeStates,
		            oneDState& a_balancedState, const levelOneBook& a_currentLevelOneBook, const levelOneBook& a_pastLevelOneBook)
  {
    long double midPriceChange = (static_cast<long double>(a_currentLevelOneBook.bidPrice+a_currentLevelOneBook.askPrice))/2.0
                                 -(static_cast<long double>(a_pastLevelOneBook.bidPrice+a_pastLevelOneBook.askPrice))/2.0;
    predictions o_pred = a_pastLevelOneBook.instantaneousPrediction;
    bool t_probError = (o_pred.probPrediction != ((a_currentLevelOneBook.bidPrice+a_currentLevelOneBook.askPrice) 
                                                 != (a_pastLevelOneBook.bidPrice+a_pastLevelOneBook.askPrice)));
    bool t_conditionalTimeError = (o_pred.conditionalTimePrediction != ((a_currentLevelOneBook.bidPrice+a_currentLevelOneBook.askPrice) 
                                                 != (a_pastLevelOneBook.bidPrice+a_pastLevelOneBook.askPrice)));
    bool t_meanError = (o_pred.meanPrediction != ((a_currentLevelOneBook.bidPrice+a_currentLevelOneBook.askPrice)>
    											 (a_pastLevelOneBook.bidPrice+a_pastLevelOneBook.askPrice)));
    bool t_conditionalMeanError = (o_pred.conditionalMeanPrediction!=((a_currentLevelOneBook.bidPrice+a_currentLevelOneBook.askPrice)>
    											 (a_pastLevelOneBook.bidPrice+a_pastLevelOneBook.askPrice)));
    if(a_pastLevelOneBook.bidShares > a_pastLevelOneBook.askShares)
       {
           int a_imb = min(a_pastLevelOneBook.bidShares/100-a_pastLevelOneBook.askShares/100, static_cast<uint32_t>(MAX_IMBALANCE));

    	   a_positiveStates[a_imb].stateFeatures.meanMidPriceChange = (a_positiveStates[a_imb].stateFeatures.meanMidPriceChange
    		 *(static_cast<long double>(a_positiveStates[a_imb].timeSpan))+ midPriceChange)
           /(static_cast<long double>(a_positiveStates[a_imb].timeSpan)+1.0);

         a_positiveStates[a_imb].stateFeatures.probMidPriceChange = 
          (static_cast<long double>((a_currentLevelOneBook.bidPrice+a_currentLevelOneBook.askPrice) != 
          (a_pastLevelOneBook.bidPrice+a_pastLevelOneBook.askPrice))+ 
          a_positiveStates[a_imb].stateFeatures.probMidPriceChange*static_cast<long double>(a_positiveStates[a_imb].timeSpan)
          /(static_cast<long double>(a_positiveStates[a_imb].timeSpan)+1.0));

         a_positiveStates[a_imb].stateErrorRates.meanError = (static_cast<long double>(t_meanError)+ 
           a_positiveStates[a_imb].stateErrorRates.meanError*static_cast<long double>(a_positiveStates[a_imb].timeSpan))
          /(static_cast<long double>(a_positiveStates[a_imb].timeSpan)+1.0);

         a_positiveStates[a_imb].stateErrorRates.probError = (static_cast<long double>(t_probError)+ 
           a_positiveStates[a_imb].stateErrorRates.probError*static_cast<long double>(a_positiveStates[a_imb].timeSpan))
          /(static_cast<long double>(a_positiveStates[a_imb].timeSpan)+1.0);

         a_positiveStates[a_imb].stateErrorRates.conditionalMeanError = (static_cast<long double>(t_conditionalMeanError)+ 
           a_positiveStates[a_imb].stateErrorRates.conditionalMeanError*static_cast<long double>(a_positiveStates[a_imb].timeSpan))
          /(static_cast<long double>(a_positiveStates[a_imb].timeSpan)+1.0);

         a_positiveStates[a_imb].stateErrorRates.conditionalTimeError = (static_cast<long double>(t_conditionalTimeError)+ 
           a_positiveStates[a_imb].stateErrorRates.conditionalTimeError*static_cast<long double>(a_positiveStates[a_imb].timeSpan))
          /(static_cast<long double>(a_positiveStates[a_imb].timeSpan)+1.0);
    	   a_positiveStates[a_imb].timeSpan++;
       }
   else if(a_pastLevelOneBook.bidShares < a_pastLevelOneBook.askShares)
      {
	       int a_imb = min(a_pastLevelOneBook.askShares/100-a_pastLevelOneBook.bidShares/100, static_cast<uint32_t>(MAX_IMBALANCE));
	       	   a_negativeStates[a_imb].stateFeatures.meanMidPriceChange = (a_negativeStates[a_imb].stateFeatures.meanMidPriceChange
	       			                                                     *(static_cast<long double>(a_negativeStates[a_imb].timeSpan))
	                      + midPriceChange)/(static_cast<long double>(a_negativeStates[a_imb].timeSpan)+1.0);

          a_negativeStates[a_imb].stateFeatures.probMidPriceChange = 
          (static_cast<long double>((a_currentLevelOneBook.bidPrice+a_currentLevelOneBook.askPrice) != 
          (a_pastLevelOneBook.bidPrice+a_pastLevelOneBook.askPrice))+ 
          a_negativeStates[a_imb].stateFeatures.probMidPriceChange*static_cast<long double>(a_negativeStates[a_imb].timeSpan)
          /(static_cast<long double>(a_negativeStates[a_imb].timeSpan)+1.0));

         a_negativeStates[a_imb].stateErrorRates.meanError = (static_cast<long double>(t_meanError)+ 
           a_negativeStates[a_imb].stateErrorRates.meanError*static_cast<long double>(a_negativeStates[a_imb].timeSpan))
          /(static_cast<long double>(a_negativeStates[a_imb].timeSpan)+1.0);

         a_negativeStates[a_imb].stateErrorRates.probError = (static_cast<long double>(t_probError)+ 
           a_negativeStates[a_imb].stateErrorRates.probError*static_cast<long double>(a_negativeStates[a_imb].timeSpan))
          /(static_cast<long double>(a_negativeStates[a_imb].timeSpan)+1.0);

         a_negativeStates[a_imb].stateErrorRates.conditionalMeanError = (static_cast<long double>(t_conditionalMeanError)+ 
           a_negativeStates[a_imb].stateErrorRates.conditionalMeanError*static_cast<long double>(a_negativeStates[a_imb].timeSpan))
          /(static_cast<long double>(a_negativeStates[a_imb].timeSpan)+1.0);

         a_negativeStates[a_imb].stateErrorRates.conditionalTimeError = (static_cast<long double>(t_conditionalTimeError)+ 
           a_negativeStates[a_imb].stateErrorRates.conditionalTimeError*static_cast<long double>(a_negativeStates[a_imb].timeSpan))
          /(static_cast<long double>(a_negativeStates[a_imb].timeSpan)+1.0);

	       a_negativeStates[a_imb].timeSpan++;
      }
   else
   	{
	       a_balancedState.stateFeatures.meanMidPriceChange = (a_balancedState.stateFeatures.meanMidPriceChange
			                                               *(static_cast<long double>(a_balancedState.timeSpan))
	                                                + midPriceChange)/(static_cast<long double>(a_balancedState.timeSpan)+1.0);

          a_balancedState.stateFeatures.probMidPriceChange = 
          (static_cast<long double>((a_currentLevelOneBook.bidPrice+a_currentLevelOneBook.askPrice) != 
          (a_pastLevelOneBook.bidPrice+a_pastLevelOneBook.askPrice))+ 
          a_balancedState.stateFeatures.probMidPriceChange*static_cast<long double>(a_balancedState.timeSpan)
          /(static_cast<long double>(a_balancedState.timeSpan)+1.0));

         a_balancedState.stateErrorRates.meanError= (static_cast<long double>(t_meanError)+ 
           a_balancedState.stateErrorRates.meanError*static_cast<long double>(a_balancedState.timeSpan))
          /(static_cast<long double>(a_balancedState.timeSpan)+1.0);

         a_balancedState.stateErrorRates.probError = (static_cast<long double>(t_probError)+ 
           a_balancedState.stateErrorRates.probError*static_cast<long double>(a_balancedState.timeSpan))
          /(static_cast<long double>(a_balancedState.timeSpan)+1.0);

         a_balancedState.stateErrorRates.conditionalMeanError = (static_cast<long double>(t_conditionalMeanError)+ 
           a_balancedState.stateErrorRates.conditionalMeanError*static_cast<long double>(a_balancedState.timeSpan))
          /(static_cast<long double>(a_balancedState.timeSpan)+1.0);

         a_balancedState.stateErrorRates.conditionalTimeError = (static_cast<long double>(t_conditionalTimeError)+ 
           a_balancedState.stateErrorRates.conditionalTimeError*static_cast<long double>(a_balancedState.timeSpan))
          /(static_cast<long double>(a_balancedState.timeSpan)+1.0);

	       a_balancedState.timeSpan++;
    }



    return;
  }

void updateConditionalFeatures(vector<levelOneBook>& a_closingConditionalBooks, vector<oneDState>& a_positiveStates,
                               vector<oneDState>& a_negativeStates, oneDState& a_balancedState, int a_futureIndex)
{
   levelOneBook futureLevelOneBook = a_closingConditionalBooks[a_futureIndex];
   vector<levelOneBook>::iterator it = a_closingConditionalBooks.begin();
   int a_index = 0;
   while(it!=a_closingConditionalBooks.end() && a_index < a_futureIndex)
     {
			if((it->bidShares) > (it->askShares))
   			{
        	   long double midPriceChange = static_cast<long double>(futureLevelOneBook.bidPrice+futureLevelOneBook.askPrice)/2.0-
                                            static_cast<long double>(it->bidPrice+it->askPrice)/2.0;
               long double timeToMidPriceChange = static_cast<long double>(a_futureIndex - a_index);
               oneDState a_ODS = a_positiveStates[min(it->bidShares/100-it->askShares/100,static_cast<uint32_t>(MAX_IMBALANCE))];
               int a_imb = min(it->bidShares/100-it->askShares/100,static_cast<uint32_t>(MAX_IMBALANCE));
               a_positiveStates[a_imb].stateFeatures.conditionalMeanMidPriceChange
               = (a_ODS.stateFeatures.conditionalMeanMidPriceChange*(static_cast<long double>(a_ODS.numberOfMidPriceChanges))+ midPriceChange)
                 /(static_cast<long double>(a_ODS.numberOfMidPriceChanges)+1.0);
               a_positiveStates[a_imb].stateFeatures.timeToMidPriceChange
               = (a_ODS.stateFeatures.timeToMidPriceChange*(static_cast<long double>(a_ODS.numberOfMidPriceChanges))+ timeToMidPriceChange)
                 /(static_cast<long double>(a_ODS.numberOfMidPriceChanges)+1.0);
               a_positiveStates[a_imb].numberOfMidPriceChanges++;

      		}
			else if((it->bidShares) < (it->askShares))
   			{
               long double midPriceChange = static_cast<long double>(futureLevelOneBook.bidPrice+futureLevelOneBook.askPrice)/2.0-
                                            static_cast<long double>(it->bidPrice+it->askPrice)/2.0;
               long double timeToMidPriceChange = static_cast<long double>(a_futureIndex - a_index);
               oneDState a_ODS = a_negativeStates[min(it->askShares/100-it->bidShares/100,static_cast<uint32_t>(MAX_IMBALANCE))];
               int a_imb = min(it->bidShares/100-it->askShares/100,static_cast<uint32_t>(MAX_IMBALANCE));
               a_negativeStates[a_imb].stateFeatures.conditionalMeanMidPriceChange
               = (a_ODS.stateFeatures.conditionalMeanMidPriceChange*(static_cast<long double>(a_ODS.numberOfMidPriceChanges))+ midPriceChange)
                 /(static_cast<long double>(a_ODS.numberOfMidPriceChanges)+1.0);
               a_negativeStates[a_imb].stateFeatures.timeToMidPriceChange
               = (a_ODS.stateFeatures.timeToMidPriceChange*(static_cast<long double>(a_ODS.numberOfMidPriceChanges))+ timeToMidPriceChange)
                 /(static_cast<long double>(a_ODS.numberOfMidPriceChanges)+1.0);
	            a_negativeStates[a_imb].numberOfMidPriceChanges++;
       		}
   		else
    		{
               long double midPriceChange = static_cast<long double>(futureLevelOneBook.bidPrice+futureLevelOneBook.askPrice)/2.0-
                                            static_cast<long double>(it->bidPrice+it->askPrice)/2.0;
               long double timeToMidPriceChange = static_cast<long double>(a_futureIndex - a_index);
               oneDState a_ODS = a_balancedState;
               a_balancedState.stateFeatures.conditionalMeanMidPriceChange
               = (a_ODS.stateFeatures.conditionalMeanMidPriceChange*(static_cast<long double>(a_ODS.timeSpan))+ midPriceChange)
                 /(static_cast<long double>(a_ODS.numberOfMidPriceChanges)+1.0);
               a_balancedState.stateFeatures.timeToMidPriceChange
               = (a_ODS.stateFeatures.timeToMidPriceChange*(static_cast<long double>(a_ODS.numberOfMidPriceChanges))+ timeToMidPriceChange)
                 /(static_cast<long double>(a_ODS.numberOfMidPriceChanges)+1.0);
	            a_balancedState.numberOfMidPriceChanges++;
	   		}

        it++;
        a_index++;
     }
     a_closingConditionalBooks.erase(a_closingConditionalBooks.begin(), a_closingConditionalBooks.begin()+a_futureIndex);

   return;
}


predictions predict(levelOneBook a_lob,
                    const vector<oneDState>& a_positiveStates, const vector<oneDState>& a_negativeStates, const oneDState& a_balancedState,
                    const uint64_t a_jumpTime)
{

			uint32_t a_bs = a_lob.bidShares;
			uint32_t a_as = a_lob.askShares;
			predictions o_predictions;
			if(a_bs > a_as)
   			{
               uint32_t a_imb = min(a_bs/100-a_as/100,static_cast<uint32_t>(MAX_IMBALANCE));
               features a_sf = a_positiveStates[a_imb].stateFeatures;
               o_predictions.probPrediction = (a_sf.probMidPriceChange > 0);
               o_predictions.meanPrediction = (a_sf.meanMidPriceChange > 0);
               o_predictions.conditionalMeanPrediction = (a_sf.conditionalMeanMidPriceChange > 0);
               o_predictions.conditionalTimePrediction = (a_sf.timeToMidPriceChange < a_jumpTime);
      		}
			else if(a_bs < a_as)
   			{
               uint32_t a_imb = min(a_as/100-a_bs/100,static_cast<uint32_t>(MAX_IMBALANCE));
               features a_sf = a_negativeStates[a_imb].stateFeatures;
               o_predictions.probPrediction = (a_sf.probMidPriceChange > 0);
               o_predictions.meanPrediction = (a_sf.meanMidPriceChange > 0);
               o_predictions.conditionalMeanPrediction = (a_sf.conditionalMeanMidPriceChange > 0);
               o_predictions.conditionalTimePrediction = (a_sf.timeToMidPriceChange < a_jumpTime);
       		}
   		else
  			{
               features a_sf = a_balancedState.stateFeatures;
               o_predictions.probPrediction = (a_sf.probMidPriceChange > 0);
               o_predictions.meanPrediction = (a_sf.meanMidPriceChange > 0);
               o_predictions.conditionalMeanPrediction = (a_sf.conditionalMeanMidPriceChange > 0);
               o_predictions.conditionalTimePrediction = (a_sf.timeToMidPriceChange < a_jumpTime);
	   		}
  
  
 return o_predictions;
}

errorRates get_error_rates(levelOneBook a_lob,
                    const vector<oneDState>& a_positiveStates, const vector<oneDState>& a_negativeStates, const oneDState& a_balancedState)
{

			uint32_t a_bs = a_lob.bidShares;
			uint32_t a_as = a_lob.askShares;
			errorRates f_errorRates;
			if(a_bs > a_as)
   			{
               uint32_t a_imb = min(a_bs/100-a_as/100,static_cast<uint32_t>(MAX_IMBALANCE));
               f_errorRates = a_positiveStates[a_imb].stateErrorRates;
            }
			else if(a_bs < a_as)
   			{
               uint32_t a_imb = min(a_as/100-a_bs/100,static_cast<uint32_t>(MAX_IMBALANCE));
               f_errorRates = a_negativeStates[a_imb].stateErrorRates;
       		}
			else
    		{
               f_errorRates = a_balancedState.stateErrorRates;
	   		}
  
  
 return f_errorRates;
}
uint32_t round_to_nearest_int(long double num)
{
  uint32_t result;
  if( (num + 0.5) >= (int(num) + 1) )
   return (uint32_t(num)+1);
  else
   return (uint32_t(num));
}
#endif /* COMPUTE_ROUTINES_H_ */


