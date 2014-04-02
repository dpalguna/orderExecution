/*
 * build_book_uncross.cpp
 *
 *  Created on: Mar 26, 2014
 *      Author: deepan
  

  Contains the main routine
 */
#include "book_routines.h"
#include "compute_routines.h"
#include "read_and_update.h"
#include "read_next_message.h"
#include "perform_execution.h"

/*
   Remove "dead orders" when execution messages disobey priorities.
   To run, pass command line parameters for the ticker file and the \delta

*/

using namespace std;

int main(int argc, char* argv[])
{

 fstream input_file;
 executionSpecifications o_execSpecs;
 //Parameters to be read from command line
 string ticker; uint64_t jumpTime;
 if(argc == 5)
  {
   ticker = argv[1];
   o_execSpecs.initialize(atoi(argv[2]), atoi(argv[3]),0,0,0,0,atoi(argv[4]));
   jumpTime = atoi(argv[4]);
  }
 else
  {
    cerr<<endl<<"Need to specify parameters: ./build_book_uncross <ticker_file> <delta>";
    exit(1);
  }
 char *buffer; unsigned long buffer_length;
 vector<levelOneBook> closingBooks;
 vector<levelOneBook> closingConditionalBooks;
 vector<oneDState> positiveStates, negativeStates;
 positiveStates.resize(MAX_IMBALANCE+1);
 negativeStates.resize(MAX_IMBALANCE+1);
 oneDState balancedState;

 closingBooks.resize(static_cast<int>(jumpTime+1));
 unsigned int closingBookIndex = 0;
 globalTime o_globalTime, o_nextGlobalTime;

 map<uint64_t, order> orderID_to_properties;
 map<uint32_t, map<uint64_t, order> > bidBook;
 map<uint32_t, map<uint64_t, order> > askBook;
 input_file.open(ticker.c_str(),ios::in | ios::binary);


 if(input_file == NULL)
   {
     exit(1);
   }
  else
   {
     input_file.seekg (0, input_file.end);
     buffer_length = input_file.tellg();
     input_file.seekg (0, input_file.beg);
     buffer = new char[buffer_length];
     input_file.read (buffer,buffer_length);
     input_file.close();
     int buffer_index = 0;

     bool end_of_day = 0;
     bool start_of_trading = 0;


     while(!end_of_day)
      {
       char typeOfMessage = buffer[buffer_index];
       read_and_update(typeOfMessage, buffer, buffer_index, bidBook, askBook, orderID_to_properties,o_globalTime);
       start_of_trading = (o_globalTime.second == NINE_THIRTY_AM || start_of_trading);
       o_nextGlobalTime = o_globalTime;
       read_next_message(buffer[buffer_index], buffer, buffer_index, o_nextGlobalTime);//Simply reads the next msg. Pointer is not moved

       if(start_of_trading && o_globalTime.second < TWO_THIRTY_PM && o_nextGlobalTime.second < TWO_THIRTY_PM)
        {
    	   if(o_globalTime.timeStampToMs() != o_nextGlobalTime.timeStampToMs())
             {
    		      int numMultiples = o_nextGlobalTime.timeStampToMs() - o_globalTime.timeStampToMs()+(o_globalTime.nanoSecond%(MS_TO_NS) == 0);
               levelOneBook lob = bookToLevelOne(bidBook,askBook);
               marketOrder t_MO;
               for(int i = 0; i< numMultiples; i++)
                 {
                   predictions o_pred;
						 o_pred = predict(lob, positiveStates, negativeStates, balancedState,jumpTime);
                   closingBooks[closingBookIndex].initialize(lob.bidShares, lob.askShares, lob.bidPrice, lob.askPrice, o_pred);
                   closingConditionalBooks.push_back(closingBooks[closingBookIndex]);
                   closingBookIndex = ((closingBookIndex == (jumpTime))? 0: (closingBookIndex+1));
                 }
               int conditionalFutureIndex = closingConditionalBooks.size()-numMultiples;
             	if(o_globalTime.second > NINE_THIRTY_AM + jumpTime/(S_TO_MS))
           		  {
               		int pastIndex = ((closingBookIndex == jumpTime)?0:(closingBookIndex+1));
               		levelOneBook pastLevelOneBook = closingBooks[pastIndex];
               		levelOneBook currentLevelOneBook = closingBooks[closingBookIndex];
               		updateFeatures(positiveStates,negativeStates,balancedState,currentLevelOneBook,pastLevelOneBook);
                 }//Update features that look ahead for a constant time interval
               if(closingConditionalBooks.begin()->bidPrice != closingConditionalBooks.rbegin()->bidPrice ||
						closingConditionalBooks.begin()->askPrice != closingConditionalBooks.rbegin()->askPrice)//Mid price has changed
                 {
                    updateConditionalFeatures(closingConditionalBooks,positiveStates, negativeStates, balancedState, conditionalFutureIndex);
                 }
          	}//If for sampling time
        }

		 if(start_of_trading && o_globalTime.second == TWO_THIRTY_PM && typeOfMessage == 'T')
        {
           perform_execution(bidBook, askBook, orderID_to_properties, o_globalTime, positiveStates, negativeStates, balancedState, buffer, buffer_index, o_execSpecs);
        }
       end_of_day = (o_globalTime.second == FOUR_PM);
     }
     
   }
 cout<<endl<<"Exiting:";
 delete[] buffer;
 return 0;
}




