/*
 * book_routines.h
 *
 *  Created on: Mar 26, 2014
 *      Author: deepan
 */

#ifndef BOOK_ROUTINES_H_
#define BOOK_ROUTINES_H_




#endif /* BOOK_ROUTINES_H_ */
#pragma once
#include "class_definitions.h"

void voluntary_uncross(order a_order, map<uint32_t, map<uint64_t, order> >&a_bidBook, map<uint32_t, map<uint64_t, order> >&a_askBook);
void cancel_order_book_entry(order a_canceledOrder, uint32_t a_canceledShares, map<uint32_t, map<uint64_t, order> >&a_bidBook,
                              map<uint32_t, map<uint64_t, order> >&a_askBook);
void add_order_book_entry(order a_order, map<uint32_t, map<uint64_t, order> >&a_bidBook, map<uint32_t,
                             map<uint64_t, order> >&a_askBook, char a_msgCode);
bool execute_order_book_entry(order a_executedOrder, uint32_t a_executedShares, map<uint32_t, map<uint64_t, order> >&a_bidBook,
                              map<uint32_t, map<uint64_t, order> >&a_askBook);
void delete_dead_orders(order a_executedOrder, map<uint32_t, map<uint64_t, order> >&a_bidBook, map<uint32_t, map<uint64_t, order> >&a_askBook);
void print_order_book(map<uint32_t, map<uint64_t, order> >&a_bidBook, map<uint32_t, map<uint64_t, order> >&a_askBook);
uint64_t execute_shadow_market_order(map<uint32_t, map<uint64_t, order> >&a_bidBook, map<uint32_t, map<uint64_t, order> >&a_askBook,
                                 const marketOrder& a_MO);




void add_order_book_entry(order a_order, map<uint32_t, map<uint64_t, order> >&a_bidBook, map<uint32_t,
                             map<uint64_t, order> >&a_askBook, char a_msgCode)
{

  switch(a_msgCode)
   {
      case 'A': case 'F':
                if(a_order.side == 'B')
                  {
                     if(a_bidBook.find(a_order.price) == a_bidBook.end())//The price is not present in the book
                       {

                         map <uint64_t, order> map_at_price;
                         map_at_price.insert(pair<uint64_t, order>(static_cast<uint64_t>(a_order.OID),a_order));
                         a_bidBook.insert(pair<uint32_t, map<uint64_t, order> >
                         (static_cast<uint32_t>(a_order.price), map_at_price));
                       }
                     else//The price is already there
                       {
                         ((a_bidBook.find(a_order.price))->second).insert(pair<uint64_t, order>(a_order.OID,a_order));
                       }
                  }
                else//add sell order
                  {
                     if(a_askBook.find(a_order.price) == a_askBook.end())
                       {
                         map <uint64_t, order> map_at_price;
                         map_at_price.insert(pair<uint64_t, order>(static_cast<uint64_t>(a_order.OID),a_order));
                         a_askBook.insert(pair<uint32_t, map<uint64_t, order> >
                         (static_cast<uint32_t>(a_order.price), map_at_price));
                       }
                     else
                       {
                         ((a_askBook.find(a_order.price))->second).insert(pair<uint64_t, order>(a_order.OID,a_order));
                       }
                  }
     default: break;
   }

}

bool execute_order_book_entry(order a_executedOrder, uint32_t a_executedShares, map<uint32_t, map<uint64_t, order> >&a_bidBook,
                              map<uint32_t, map<uint64_t, order> >&a_askBook)
{
  static int numViolations = 0;
  bool violatePriority = 0;
  if(a_executedOrder.side == 'B') //A buy order got executed
    {
       	map<uint32_t, map<uint64_t, order> >::iterator pit = a_bidBook.find(a_executedOrder.price);
       	if(pit != a_bidBook.end())
       		{
        			map<uint64_t, order>::iterator oit = (pit->second).find(a_executedOrder.OID);
        			if(oit != (pit->second).end())
         			{
          				(oit->second).shares = ((oit->second).shares > a_executedShares)?(oit->second).shares-a_executedShares:0;
          				if(oit->second.OID != ((pit->second).begin())->second.OID)
            				{
              					violatePriority = 1;
              					++numViolations;
				            }
          				if((oit->second).shares == 0)
          					{
           						(pit->second).erase(oit);
           						if(pit->second.empty())
            						{
               						a_bidBook.erase(pit);
            						}
          					}
         			}
       		}
    }
  else //sell order got executed
    {
       map<uint32_t, map<uint64_t, order> >::iterator pit = a_askBook.find(a_executedOrder.price);
       if(pit!=a_askBook.end())
        {
        		map<uint64_t, order>::iterator oit = (pit->second).find(a_executedOrder.OID);
        		if(oit  != (pit->second).end())
        			{
         				(oit->second).shares = ((oit->second).shares > a_executedShares)?(oit->second).shares-a_executedShares:0;
         				if(oit->second.OID != ((pit->second).begin())->second.OID)
            				{
              					violatePriority = 1;
              					++numViolations;
            				}
         				if((oit->second).shares == 0)
         					{
           						(pit->second).erase(oit);
           						if(pit->second.empty())
            						{
              							a_askBook.erase(pit);
            						}
         					}
        			 }
       	}
    }

  return violatePriority;
}


void cancel_order_book_entry(order a_canceledOrder, uint32_t a_canceledShares, map<uint32_t, map<uint64_t, order> >&a_bidBook,
                              map<uint32_t, map<uint64_t, order> >&a_askBook)
{
  if(a_canceledOrder.side == 'B') //A buy order got executed
    {
       map<uint32_t, map<uint64_t, order> >::iterator pit = a_bidBook.find(a_canceledOrder.price);
       if(pit != a_bidBook.end())
       {
        	map<uint64_t, order>::iterator oit = (pit->second).find(a_canceledOrder.OID);
        	if(oit != (pit->second).end() )
        		{
         		(oit->second).shares = ((oit->second).shares > a_canceledShares)?(oit->second).shares-a_canceledShares:0;
         		if((oit->second).shares == 0)
          			{
           				(pit->second).erase(oit);
           				if(pit->second.empty())
             				{
               				a_bidBook.erase(pit);
             				}
          			}
         	}
       }
    }
  else //sell order got executed
    {
       map<uint32_t, map<uint64_t, order> >::iterator pit = a_askBook.find(a_canceledOrder.price);
       if(pit!=a_askBook.end())
       {
       	map<uint64_t, order>::iterator oit = (pit->second).find(a_canceledOrder.OID);
         if(oit!=(pit->second).end())
            {
              (oit->second).shares = ((oit->second).shares > a_canceledShares)?(oit->second).shares-a_canceledShares:0;
         	  if((oit->second).shares == 0)
         		  {
           			  (pit->second).erase(oit);
           			  if(pit->second.empty())
             		     {
               		     a_askBook.erase(pit);
             		     }
         	     }
            }
       }
    }

  return;
}

void delete_order_book_entry(order a_deletedOrder, map<uint32_t, map<uint64_t, order> >&a_bidBook,
                              map<uint32_t, map<uint64_t, order> >&a_askBook)
{
  if(a_deletedOrder.side == 'B') //A buy order got deleted
    {
       map<uint32_t, map<uint64_t, order> >::iterator pit = a_bidBook.find(a_deletedOrder.price);
       if(pit != a_bidBook.end())
       {
          map<uint64_t, order>::iterator oit = (pit->second).find(a_deletedOrder.OID);
          if(oit != (pit->second).end())
           {
             (pit->second).erase(oit);
             if(pit->second.empty())
              {
                a_bidBook.erase(pit);
              }
           }
       }
    }
  else //sell order got deleted
    {
       map<uint32_t, map<uint64_t, order> >::iterator pit = a_askBook.find(a_deletedOrder.price);
       if(pit != a_askBook.end())
        {
          map<uint64_t, order>::iterator oit = (pit->second).find(a_deletedOrder.OID);
          if(oit!=(pit->second).end())
           {
            (pit->second).erase(oit);
            if(pit->second.empty())
              {
                a_askBook.erase(pit);
              }
           }
        }
    }

  return;
}

void voluntary_uncross(order a_order, map<uint32_t, map<uint64_t, order> >&a_bidBook, map<uint32_t, map<uint64_t, order> >&a_askBook)
{
   vector<order> deletedOrders;

   static int count =0;
   count++;
	if(a_order.side == 'S') //Sell order treated as a MO and uncrossed with the bid book
     {
 	      map<uint32_t, map<uint64_t, order> >::reverse_iterator pit = a_bidBook.rbegin();
			uint32_t totalShares = a_order.shares;
         while(totalShares > 0 && pit!=a_bidBook.rend())
          {
            map<uint64_t, order>::iterator oit = (pit->second).begin();

            while(oit!=(pit->second).end() && totalShares>0)
			     {
                 uint32_t originalTotal = totalShares;
				     totalShares = totalShares > (oit->second.shares) ? totalShares-(oit->second.shares):0;
                 oit->second.shares = originalTotal > (oit->second.shares) ? 0: oit->second.shares - originalTotal;
                 if (oit->second.shares == 0)
                 {
						  deletedOrders.push_back(oit->second);
                 }
                 oit++;
			     }
            pit++;
          }
     }
   else//uncross buy order with ask book
     {
         map<uint32_t, map<uint64_t, order> >::iterator pit = a_askBook.begin();
			uint32_t totalShares = a_order.shares;
         while(totalShares > 0 && pit!=a_askBook.end())
          {
            map<uint64_t, order>::iterator oit = (pit->second).begin();
            while(oit!=(pit->second).end() && totalShares>0)
			     {
                 uint32_t originalTotal = totalShares;
				     totalShares = totalShares > (oit->second.shares) ? totalShares-(oit->second.shares):0;
                 oit->second.shares = originalTotal > (oit->second.shares) ? 0: oit->second.shares - originalTotal;
                 if (oit->second.shares == 0)
                   {
                     deletedOrders.push_back(oit->second);
                   }
                  oit++;
			     }
            pit++;
          }
     }
   vector<order>::iterator vit=deletedOrders.begin();
   while(vit!=deletedOrders.end())
     {
        delete_order_book_entry(*vit, a_bidBook, a_askBook);
        vit++;
     }
   //cerr<<endl<<"finished uncrossing";
   //cin.get();
   return;
}

void delete_dead_orders(order a_executedOrder, map<uint32_t, map<uint64_t, order> >&a_bidBook,
                              map<uint32_t, map<uint64_t, order> >&a_askBook)
{
   vector<order> deadOrders;

	if(a_executedOrder.side == 'B') //Buy side
     {
 	      map<uint32_t, map<uint64_t, order> >::reverse_iterator pit = a_bidBook.rbegin();
         while(pit!=a_bidBook.rend() && pit->first >= a_executedOrder.price)
          {
            map<uint64_t, order>::iterator oit = (pit->second).begin();
            while(oit!=(pit->second).end())
			     {
                if(pit->first > a_executedOrder.price)
                  deadOrders.push_back(oit->second);
                else if(pit->first == a_executedOrder.price && oit->first < a_executedOrder.OID)
                  deadOrders.push_back(oit->second);
                oit++;
			     }
            pit++;
          }
     }
   else//Sell side
     {
         map<uint32_t, map<uint64_t, order> >::iterator pit = a_askBook.begin();
         while(pit!=a_askBook.end() && pit->first <= a_executedOrder.price)
          {
            map<uint64_t, order>::iterator oit = (pit->second).begin();
            while(oit!=(pit->second).end())
			     {
 					 if(pit->first < a_executedOrder.price)
                  deadOrders.push_back(oit->second);
                else if(pit->first == a_executedOrder.price && oit->first < a_executedOrder.OID)
                  deadOrders.push_back(oit->second);
                oit++;
			     }
            pit++;
          }
     }
   vector<order>::iterator vit=deadOrders.begin();
   while(vit!=deadOrders.end())
     {
        delete_order_book_entry(*vit, a_bidBook, a_askBook);
        vit++;
     }
  return;
}


uint64_t execute_shadow_market_order(map<uint32_t, map<uint64_t, order> >&a_bidBook, map<uint32_t, map<uint64_t, order> >&a_askBook,
                                 const marketOrder& a_MO)
{
  uint32_t totalSharesLeft = a_MO.shares;
  uint64_t totalCost = 0;
  uint32_t sharesExecuted = 0;
  if(a_MO.side == 'S')
    {
         map<uint32_t, map<uint64_t, order> >::reverse_iterator pit = a_bidBook.rbegin();
			while(pit != a_bidBook.rend() && totalSharesLeft > 0)
           {
             map<uint64_t, order>::iterator oit = (pit->second).begin();
 				 while(oit != (pit->second).end() && totalSharesLeft > 0)
					{
						sharesExecuted = (totalSharesLeft > (oit->second).shares)?(oit->second).shares:totalSharesLeft;
						totalSharesLeft = (totalSharesLeft > (oit->second).shares)?totalSharesLeft-(oit->second).shares:0;
						totalCost += (pit->first)*sharesExecuted;
						oit++;
					}
				 pit++;
           }
    }
  else
    {
         map<uint32_t, map<uint64_t, order> >::iterator pit = a_askBook.begin();
			while(pit != a_askBook.end() && totalSharesLeft > 0)
           {
             map<uint64_t, order>::iterator oit = (pit->second).begin();
 				 while(oit != (pit->second).end() && totalSharesLeft > 0)
					{
						sharesExecuted = (totalSharesLeft > (oit->second).shares)?(oit->second).shares:totalSharesLeft;
						totalSharesLeft = (totalSharesLeft > (oit->second).shares)?totalSharesLeft-(oit->second).shares:0;
						totalCost += (pit->first)*sharesExecuted;
						oit++;
					}
				 pit++;
           }
    }
  return totalCost;
}



void print_order_book(map<uint32_t, map<uint64_t, order> >&a_bidBook, map<uint32_t, map<uint64_t, order> >&a_askBook)
{
 map<uint32_t, map<uint64_t, order> >::reverse_iterator pit = a_bidBook.rbegin();
 int count = 0;
 cout<<endl<<"============================================================================";
 while(pit!=a_bidBook.rend() && count<10)
   {
      cout<<endl<<(pit->first)<<":";
		map<uint64_t, order>::iterator oit = (pit->second).begin();
      while(oit!=(pit->second).end())
			{
				/*cout<<"("<<(oit->first)<<","<<(oit->second.second)<<"."<<(oit->second.nanoSecond)
            <<","<<(oit->second.price)<<","<<(oit->second.shares)<<","<<(oit->second.side)
            <<")--->";*/
            cout<<"("<<(oit->first)<<","
            <<(oit->second.price)<<","<<(oit->second.shares)<<","<<(oit->second.side)
            <<")--->";
            oit++;
			}

      pit++;
      count++;
   }
 cout<<endl<<"----------------------------------------------------------------------------------------" ;
 map<uint32_t, map<uint64_t, order> >::iterator apit = a_askBook.begin();
 count =0;
 while(apit!=a_askBook.end() && count<10)
   {
      cout<<endl<<(apit->first)<<":";
		map<uint64_t, order>::iterator oit = (apit->second).begin();
      while(oit!=(apit->second).end())
			{
				/*cout<<"("<<(oit->first)<<","<<(oit->second.second)<<"."<<(oit->second.nanoSecond)
            <<","<<(oit->second.price)<<","<<(oit->second.shares)<<","<<(oit->second.side)
            <<")--->";*/
           cout<<"("<<(oit->first)<<","
            <<(oit->second.price)<<","<<(oit->second.shares)<<","<<(oit->second.side)
            <<")--->";
            oit++;
			}

      apit++;
      count++;
   }
  cout<<endl<<"============================================================================";
 return;
}

levelOneBook bookToLevelOne(map<uint32_t, map<uint64_t, order> >&a_bidBook, map<uint32_t, map<uint64_t, order> >&a_askBook)
{
  levelOneBook a_lob;
  a_lob.bidPrice = a_bidBook.rbegin()->first;
  a_lob.askPrice = a_askBook.begin()->first;
  a_lob.bidShares = a_lob.askShares = 0;
  map<uint32_t, map<uint64_t, order> >::reverse_iterator pit = a_bidBook.rbegin();
  if(pit!=a_bidBook.rend())
   {
      map<uint64_t, order>::iterator oit = (pit->second).begin();
      while(oit!=(pit->second).end())
			{
            a_lob.bidShares += (oit->second).shares;
            oit++;
			}
   }

  map<uint32_t, map<uint64_t, order> >::iterator apit = a_askBook.begin();
  if(apit!=a_askBook.end())
   {
		map<uint64_t, order>::iterator oit = (apit->second).begin();
      while(oit!=(apit->second).end())
			{
				a_lob.askShares += (oit->second).shares;
            oit++;
			}
    }

  return a_lob;
}




