 /*A Wait Free Red Black Tree*/


#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include "wfrbt.h"
#include "insertion.h"
#include "deletion.h"
#include "read.h" 
#include "window.h"
#include "invariant.h"
#include "helping.h"
//#include "windowcase2.h"
//#include "gc.h"
//#include "update.h"

#ifdef DEBUG
#define IO_FLUSH                        fflush(NULL)
/* Note: stdio is thread-safe */
#endif

/*
 * Useful macros to work with transactions. Note that, to use nested
 * transactions, one should check the environment returned by
 * stm_get_env() and only call sigsetjmp() if it is not null.
 */
 
#define RO                              1
#define RW                              0

#define DEFAULT_DURATION                1000
#define DEFAULT_TABLE_SIZE              10
#define DEFAULT_NB_THREADS              1
#define DEFAULT_SEED                    0
#define DEFAULT_SEARCH_FRAC             1.0
#define DEFAULT_INSERT_FRAC             0.0
#define DEFAULT_DELETE_FRAC             0.0
#define DEFAULT_READER_THREADS         	0
#define DEFAULT_KEYSPACE1_SIZE          100
#define DEFAULT_KEYSPACE2_SIZE          1000000000
#define KEYSPACE1_PROB           1.0
#define KEYSPACE2_PROB           0.0

#define XSTR(s)                         STR(s)
#define STR(s)                          #s

/* ################################################################### *
 * GLOBALS
 * ################################################################### */

 


 
static volatile AO_t stop;


long total_insert = 0;

timespec t;
/* STRUCTURES */

enum{
Front,
Back
};


long blackCount = -1;
long leafNodes = 0;



/* ################################################################### *
 * Correctness Checking
 * ################################################################### */
 
void check_black_count(node_t * root, int count){
        // 2. That the number of black nodes from the root to the leaf are same
	
	if(is_node_marked(root->opData)){
		std::cout << "Node marked, but not passive__" << root << std::endl;
		exit(0);
	}
	
	
  if (root->color == BLACK) {

    count++;
  }
	node_t * lChild = (node_t *)get_child(root->lChild);
	node_t * rChild = (node_t *)get_child(root->rChild);
	
	if(lChild == root || rChild == root){
		std::cout << "Error. Child pointer is the same as node" << std::endl;
		exit(0);
	}
	
	
  if (lChild != NULL) {

    check_black_count(lChild, count);
  }

  if (rChild != NULL) {
    check_black_count(rChild, count);
  }

  if (lChild == NULL && rChild == NULL) {
    std::cout << " LeafNode Encountered!" << std::endl;
    if (root->key != -1) {
      leafNodes++;
      std::cout << " Actual LeafNode! Key = " << root->key << std::endl;
    } else {
      std::cout << " Empty LeafNode!" << std::endl;
    }
    if (blackCount == -1) {
      blackCount = count;

    } else if (blackCount != count) {
      std::cout << "Black Counts do not match!!!__" << blackCount << "__" << count << std::endl;
      exit(0);
    }

  }
        
}

void check_red_property(node_t * root){
	// 1. To check that no red node has a red child
	
	node_t * lChild = (node_t *)get_child(root->lChild);
	if(lChild != NULL){
	  AO_t state = lChild->opData;
	  if(is_node_marked(state)){
	    std::cout << "Error. Active node is marked" << std::endl;
	  }
		if((root->color == RED) && (lChild->color == RED)){
			std::cout << "ERROR!!! DOUBLE RED CONDITION EXISTS IN THE TREE!!"  << std::endl;
			exit(0);
		}
		check_red_property(lChild); 
	}
	
	node_t * rChild = (node_t *)get_child(root->rChild);
	if(rChild != NULL){
		AO_t state = rChild->opData;
	  if(is_node_marked(state)){
	    std::cout << "Error. Active node is marked" << std::endl;
	  }
	
		if((root->color == RED) && (rChild->color == RED)){
			std::cout << "ERROR!!! DOUBLE RED CONDITION EXISTS IN THE TREE!!" << std::endl;
			exit(0);
		}
		check_red_property(rChild); 
		
	}
	
} 
 
long in_order_visit(node_t * root){
	
	if((node_t *)get_child(root->lChild) != NULL){
	
		long lKey = in_order_visit((node_t *)get_child(root->lChild));
		if(lKey >= root->key){
			std::cout << "Lkey is larger!!__" << lKey << "__ " << root->key << std::endl;
			std::cout << "Sanity Check Failed!!" << std::endl;
			exit(0);
		}
	}
	
	
	if((node_t *)get_child(root->rChild) != NULL){
	
		long rKey = in_order_visit((node_t *)get_child(root->rChild));
		if(rKey < root->key){
			std::cout << "Rkey is smaller!!__" << rKey << "__ " << root->key <<  std::endl;
			std::cout << "Sanity Check Failed!!" << std::endl;
			exit(0);
		}
	}
	return (root->key);
}

/************************************************************************************************/

/************************************************************************************************/

node_t * perform_external_node_replacement(thread_data_t * data, int pid, node_t * curParent, long key){
        std::cout << "Here" << std::endl;	
	// curParent is parent of an external node.
	node_t * curLeaf = NULL;
	if(key < curParent->key){
		curLeaf = (node_t *)get_child(curParent->lChild);
	}
	else{
		curLeaf = (node_t *)get_child(curParent->rChild);
	}
	
	long curKey = curLeaf->key;
	if(key == curKey){
		return NULL;
	}
	else if(curKey == -1){
	  curLeaf->key = key;
	  return curLeaf;
	}
	
	
	node_t * newInt = (node_t *)get_new_node(data);
	node_t * newLeaf = (node_t *)get_new_node(data);
		
  newLeaf->color = BLACK;
	newLeaf->key = key;
				
	if(key < curKey){
	  newInt->key = curKey;
		newInt->lChild = create_child_word(newLeaf,OFIN);
		newInt->rChild = create_child_word(curLeaf,OFIN);
	}
	else{
	  newInt->key = key;
		newInt->rChild = create_child_word(newLeaf,OFIN);
		newInt->lChild = create_child_word(curLeaf,OFIN);
	}
				
	newInt->parent = curParent;
	data->hasInserted = key;
	std::cout << "Successfully inserted" << std::endl;
	return newInt;
}


node_t * insert(thread_data_t * data, int pid, long key, node_t * wRoot, node_t * wRootChild, AO_t casField, oprec_t * O){
  // wRoot is Guardian node. wRootChild is Anchor node.
  // Create a copy of the anchor node.
  AO_t pRootContents = wRootChild->opData;
  AO_t childWord;
  int which_child_of_anchor_node = extract_child_from_opdata(pRootContents);

  node_t * bot = (node_t *)map_word_to_bot_address(1);
  int currentWindowSize = 4;
  bool case2flag = false;
 	//std::cout << "11" << std::endl;
 	node_t * anchor = make_window_copy(data, wRootChild, key, currentWindowSize,  wRoot, casField, O);
  //std::cout << "13" << std::endl;
  if(anchor == (node_t *)map_word_to_bot_address(1)){
  	return (bot);
  }
  if(anchor->parent != NULL){
    std::cout << " Anchor's parent is not NULL" << std::endl;
    exit(0);
  }
  node_t* wRootChildCopy = NULL;
  if(which_child_of_anchor_node == LEFT){
    wRootChildCopy = (node_t *)get_child(anchor->lChild);
  }
  else{
    wRootChildCopy = (node_t *)get_child(anchor->rChild);
  }
  
  if (wRootChildCopy->parent != NULL) {
    std::cout << " wRootChildCopy's parent is not NULL__" << anchor << "__" << wRootChildCopy->parent << std::endl;
    exit(0);
  }

  node_t * lastNode = set_last_node(data, wRootChildCopy, currentWindowSize, key);
  std::cout << "LastNode1 = " << lastNode << std::endl;
  while(data->madeDecision == false){	
    check_current_window(data, key, anchor, currentWindowSize);

		if(wRoot == data->prootOfTree){
		  if(wRootChildCopy->color == RED){
			  wRootChildCopy->color = BLACK;
		  }
		  // make children of root black if they are both red
		
		  node_t * lc = (node_t *)get_child(wRootChildCopy->lChild);
		  node_t * rc = (node_t *)get_child(wRootChildCopy->rChild);
		  if(lc != NULL && rc != NULL){
			  if(lc->color == RED && rc->color == RED){
			  	lc->color = BLACK;
				  rc->color = BLACK;
			  }      
		  }
	  }
	
	  // case 1: Tree is empty
	  if((wRootChildCopy->key == -1)){ 
		
		  wRootChildCopy->key = key;
  		data->hasInserted = key;
  		wRootChildCopy->move = NULL;
		
  		int child = extract_child_from_opdata(casField);
  		int result0;
  		if(child == LEFT){
			  result0 = atomic_cas_full(&wRoot->lChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		  }
		  else{
			  result0 = atomic_cas_full(&wRoot->rChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		  }
		
		  //int result1 = atomic_cas_full1(data, wRoot, casField, combine_oprec_status_child_opdata(O, DONE, child));
		
		  if(result0 == 1){						
			
			  if(data->hasInserted != 0){
				  data->numInsert++;
			  	data->hasInserted = 0;
			  }
			  return NULL;
		  } 
		  else{
			  data->hasInserted = 0;
			  return NULL;
		  }		
	  }
	
	  // tree is not empty
	  if((is_external_node(wRootChildCopy)) && wRoot == data->prootOfTree){
			
			long curKey = wRootChildCopy->key;
			if(curKey == key){
			 	anchor->move = NULL;
			  int child = extract_child_from_opdata(casField);
		    int result0;
		    if(child == LEFT){
			    result0 = atomic_cas_full(&wRoot->lChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		    }
		    else{
			    result0 = atomic_cas_full(&wRoot->rChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		    }
		
				if(result0 == 1){	
					return(NULL);
				} 
				else{
					data->hasInserted = 0;
					return NULL;
				}
			}
			else{
			  std::cout << "Yo" << std::endl;
				node_t * newLeaf = (node_t *)get_new_node(data);
				newLeaf->key = key;
				node_t * newInt = (node_t *)get_new_node(data);
				long curKey = wRootChildCopy->key;
				if(key < curKey){
					newInt->key = curKey;
					newInt->lChild = create_child_word(newLeaf,OFIN);
					newInt->rChild = create_child_word(wRootChildCopy,OFIN);
				}
				else{
					newInt->key = key;
					newInt->rChild = create_child_word(newLeaf,OFIN);
					newInt->lChild = create_child_word(wRootChildCopy,OFIN);
					
				}
				data->hasInserted = key;
			 	newInt->move = NULL;
			 	newInt->color = RED;

			 	if(which_child_of_anchor_node == LEFT){
			 	  anchor->lChild = create_child_word(newInt, OFIN);
			 	}
			 	else{
			 	  anchor->rChild = create_child_word(newInt, OFIN);
			 	}

		    int child = extract_child_from_opdata(casField);
		    int result0;
		    if(child == LEFT){
			    result0 = atomic_cas_full(&wRoot->lChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		    }
		    else{
			    result0 = atomic_cas_full(&wRoot->rChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		    }
		
				if(result0 == 1){
				  if(data->hasInserted != 0){
						data->numInsert++;
						data->hasInserted = 0;
					}
					return(NULL);
				} 
				else{	
					return NULL;
				}		
			}
			return NULL;
	  }

	  /// case 3: Multiple nodes exist in the tree.	
	  // Here we need to find out which one of the insertion cases are satisfied
	  node_t * nextWRoot = get_next_node_on_access_path(data, pid, wRoot, wRootChild, anchor, wRootChildCopy, key, currentWindowSize, casField, case2flag,O);
	  if(nextWRoot == NULL && data->madeDecision == false){
		  currentWindowSize++;
    	lastNode = extend_current_window(data, lastNode, key, currentWindowSize, wRoot, casField,O);
    	std::cout << "LastNode2 = " << lastNode << std::endl;
  	  if(lastNode == bot){
  	    return (bot);
    	}
    	std::cout << "Extending window" << std::endl;
  	  continue;
	  }

    if(nextWRoot != NULL){	
		  // external node reached.
	  	// need to insert node and rebalance 
	  	node_t * dnode = perform_external_node_replacement( data, pid, nextWRoot, key);
      if(dnode != NULL){
		    if(key < nextWRoot->key){
		    	nextWRoot->lChild = create_child_word(dnode, OFIN);
	    	}
		    else{
			    nextWRoot->rChild = create_child_word(dnode, OFIN);
		    }	
		    dnode->color = RED;
        //if(nextWRoot == wRootChildCopy){
        //  dnode->parent = NULL;
        //}
      
		    node_t * finalRootNode = balance_after_insertion(data, dnode, wRootChildCopy);
		
		    // INSERTING HERE
		    if(finalRootNode != NULL){
		      if(which_child_of_anchor_node ==  LEFT){
		        anchor->lChild = create_child_word(finalRootNode, OFIN);
		      }
		      else{
		        anchor->rChild = create_child_word(finalRootNode, OFIN);
		      }


			    anchor->move = NULL;
		      int child = extract_child_from_opdata(casField);
		      int result0;
		      if(child == LEFT){
			       result0 = atomic_cas_full(&wRoot->lChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		      }
		      else{
			     result0 = atomic_cas_full(&wRoot->rChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		      }
		    
				
				  if(result0 == 1){	
					  if(data->hasInserted != 0){
					  	data->numInsert++;
						  data->hasInserted = 0;
					  }
					  return(NULL);
				  } 
			  	else{
			  		data->hasInserted = 0;
				  	return NULL;
			  	}				
		    }
		    else {
		      std::cout << "Fishy__" << std::endl;
          if (which_child_of_anchor_node == LEFT) {
            anchor->lChild = create_child_word(dnode, OFIN);
          } else {
            anchor->rChild = create_child_word(dnode, OFIN);
          }

			    anchor->move = NULL;
	      	int child = extract_child_from_opdata(casField);
		      int result0;
		      if(child == LEFT){
			      result0 = atomic_cas_full(&wRoot->lChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		      }
		      else{
			      result0 = atomic_cas_full(&wRoot->rChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		      }
    		
				  if(result0 == 1){						
					  // cas success
					  if(data->hasInserted != 0){
						  data->numInsert++;
					  	data->hasInserted = 0;
					  }
					  return(NULL);
				  }  
				  else{
					  data->hasInserted = 0;
					  return NULL;
				  }	
			  }
			  return NULL;	
		  }
		
		  anchor->move = NULL;
	  	int child = extract_child_from_opdata(casField);
		  int result0;
	  	if(child == LEFT){
			  result0 = atomic_cas_full(&wRoot->lChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		  }
		  else{
			  result0 = atomic_cas_full(&wRoot->rChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
	  	}
		
		  //int result1 = atomic_cas_full1(data, wRoot, casField, combine_oprec_status_child_opdata(O, DONE, child));
		  if(result0 == 1){						
		    // cas success
		  	data->hasInserted = 0;
			  return NULL;
		  }  
		  else{
		    // cas failure. Some other node executed window
		  	data->hasInserted = 0;
			  return NULL;
		  }	
	  }
  }
} 

/*************************************************************************************************/

int perform_one_window_operation(thread_data_t* data, node_t* pRoot, oprec_t * O, int sucid){
  std::cout << "Performing a window" << std::endl;
  // pRoot is guardian node here.
  AO_t pRootContents = pRoot->opData;
  AO_t childWord;
  int temp;
  if(extract_child_from_opdata(pRootContents) == LEFT){
  //  std::cout << "LEFT" << std::endl;
    childWord = pRoot->lChild;
  }
  else{
   // std::cout << "RIGHT" << std::endl;
    childWord = pRoot->rChild;
  }
  if((oprec_t*)extract_oprec_from_opdata(pRootContents) == O && is_node_guardian(pRootContents)){
 // std::cout << "IN__" << is_node_owned(pRootContents) << std::endl;
    if(is_node_owned(pRootContents) && (get_child_status(childWord) == OFIN)){  
 // std::cout << "IN2" << std::endl;
  //exit(0);
	    /// The node is owned but the window transaction has not been executed
	    node_t * bot = (node_t *)map_word_to_bot_address(1);
	    data->helpCount = 0;
	    long key = map_word_to_key(O->op);

  	  unsigned opn = map_word_to_operation(O->op);
  	  // pRootChild is anchor node
	    node_t * pRootChild = (node_t *)get_child(childWord);
  	  AO_t pRootContents1 = pRoot->opData;
	    AO_t pRootContents0 = add_mark_flag(pRootContents);
	    if((pRootContents1 != pRootContents) && (pRootContents1 != pRootContents0)){
		    //Operation has moved
		    return NULL;
    	    }
	    data->lastCase = 827;	
	    
	    node_t * nextRoot1 = NULL;
	    if(opn == 2){
	      // insert operation
	      temp = 878;
        //std::cout << "going to insert__" << data->id << std::endl;
		    nextRoot1 = insert(data, O->pid, key, pRoot, pRootChild, pRootContents , O);
		 //   std::cout << "Done insert__" << data->id << std::endl;
	    }
	    else if(opn == 1){
	      // delete operation
	      //std::cout << "going to delete__" << data->id << std::endl;
	      temp = 892;
 		    bool case2flag = false;
        node_t * wRootChildCopy = make_delete_window_copy(data, pRootChild, key, pRoot, pRootContents,O);
		    if(wRootChildCopy == (node_t *)map_word_to_bot_address(1)){
			    data->madeDecision = false;
		    }
		    else{
		      if(pRoot == data->prootOfTree){
			      bool flag = false;
				    if(wRootChildCopy->color == BLACK){
				      flag = true;
				    }
		
				    node_t * lc = (node_t *)get_child(wRootChildCopy->lChild);
				    node_t * rc = (node_t *)get_child(wRootChildCopy->rChild);
				    if(lc != NULL && rc != NULL){
				      if(lc->color == BLACK && rc->color == BLACK && flag){
					      wRootChildCopy->color = RED;
					    }	
				    }	
			    }
			   nextRoot1 = get_next_node_on_delete_access_path(data, key, pRoot, wRootChildCopy, pRootChild, pRootContents, O, case2flag);
		    }
		    //std::cout << "Done Delete__" << data->id << std::endl;
      } 
     
  }
  AO_t pRootContents1 = (pRoot->opData);
  update_location(data, pRoot, pRootContents1, 574);
	data->helpCount = 0;
	
	data->madeDecision = false;
	return temp;
	}	
}


/* ################################################################### *
 * Protocol Definitions
 * ################################################################### */


void abort(thread_data_t * data, oprec_t * O){
  AO_t curWord = O->windowLoc;
  if(extract_status_from_oprecord(curWord) != ABORTED){
    std::cout << "Error. Aborting without setting status" << std::endl;      
    exit(0);
  }
  /// release the ownership of all the nodes owned in a bottom-up manner.
  int index = O->sr->length - 2;
  int  i = 0;
  while(index >= 0){
    i++;
    if(i > 1000){
      std::cout << "Hello!!!" << std::endl;
      exit(0);
    }
    AO_t word = O->sr->addresses[index];
    node_t * curNode = (node_t *)get_address_from_addresses(word);
    if(curNode == NULL){
      std::cout << "Unexpected" << std::endl;
      exit(0);
    }
    AO_t state = curNode->opData;
    bool result;
    if((oprec_t *)extract_oprec_from_opdata(state) == O && is_node_owned(state)){
      /// release the ownership of the node.
      AO_t newState = make_node_not_owned(state);
      
      assert(!is_node_owned(newState));
      
      result = atomic_cas_full1(data, curNode, state, newState);
      
      AO_t old_state = curNode->opData;
if((oprec_t *)extract_oprec_from_opdata(old_state) == O
   && is_node_owned(old_state)){
        std::cout << "Fishy11" << std::endl;    
        exit(0);
      }
    }
    else{
      result = true;
    }
    if(result){
      /// ownership successfully released; advance to next 
      --index;
    }
  }
}


int inject(thread_data_t * data, oprec_t * O){
  
  /// value to determine if oprec and seekrec can be reused
	int reuseval = 0;
	
	seekrec_t * R = O->sr;
	
	/// try to acquire ownership of all nodes in the p-path
	int index = 0;
	
	AO_t status = extract_status_from_oprecord(O->windowLoc);
	while(status == TRYING){
	  /// find node whose ownership needs to be acquired next
	  node_t * current = (node_t *)get_address_from_addresses(R->addresses[index]);
    //std::cout << "__" << current <<" __" << R->addresses[0] <<  std::endl; 
	  /// find the next node in the c-path
	  node_t * next = (node_t *)get_address_from_addresses(R->addresses[index+1]);
	  
	  AO_t childWord;
	  node_t * child;
	  /// find the relevant child of the node. Used to verify that the link from current to next still exists
	  int which = get_which_child_from_addresses(R->addresses[index]);
								     
	  if(which == LEFT){
	    childWord = current->lChild;
	  }
	  else{
	    childWord = current->rChild;
	  }
	  
	  child = (node_t *)get_child(childWord);
	  /// read the current state of the node
	  AO_t state = current->opData;
	  
	  /// verify that the operation is still in progress
	  if(extract_status_from_oprecord(O->windowLoc)!= TRYING){
	    break;
	  }

	  /// try to acquire ownership of the node
	  bool result = false;
	  oprec_t * N = (oprec_t *)extract_oprec_from_opdata(state);
	  if(N != O){
	    /// do not own node yet
	    if((child != next) || is_node_marked(state)){
	      /// the link from the current to next does not exist, or current has been marked for removal
	      O->windowLoc = combine_position_status_oprecord(NULL, ABORTED);
	      if(data->id != O->pid)
	        O->changer = 842;
	      break;
	    }
	    
	    if(is_node_owned(state)){
	      /// help the conflicting operation move out of the way
	      help(data, N, current, state, 1282);
	      continue;
	    }
	    
	    if(N != NULL && (is_node_guardian(state))){
	      /// perform the last two steps of window transaction execution if needed
	      update_location(data,current, state, 851);
	    }

	    /// compute the new contents of the state field
	    int g = (index == 0)?IS_GUARDIAN:NOT_GUARDIAN;
	    AO_t newState = combine_oprec_status_child_opdata(O, OWNED, which, INITIAL, g);
	    /// try and install the new state
              //std::cout << "2__" << std::endl;
	    result = atomic_cas_full(&current->opData, state, newState);
              //std::cout << "3__" << R->length << std::endl;
	  }
	  else{
	    result = true;
	  }

	  if(result){
	    /// onership acquired successfully
	    if(index < (R->length - 2)){
	      /// advance to the next node
	      index++;
	    }
	    else{
              //std::cout << "Am here" << std::endl;
	      // Perform a cas to change the status bits to injected.
	      // Needs to be a CAS here, as some other thread may have helped the
	      // operation complete and then this thread sets the windowLoc
	      // to R->addresses[0]
	      
	      int res = atomic_cas_full(&O->windowLoc,combine_position_status_oprecord((node_t*)get_address_from_addresses(R->addresses[0]), TRYING), combine_position_status_oprecord(get_address_from_addresses(R->addresses[0]), INJECTED) );
	     //O->windowLoc = combine_position_status_oprecord(get_address_from_addresses(R->addresses[0]), INJECTED);
	      if(res == 1){
	        if(O->pid == data->id)
	          O->changer = 876;
	        else
	          O->changer = 877;
	      }
  	//node_t * nextRoot = (node_t *)extract_position_from_oprecord(O->windowLoc);
        //std::cout << "powo__" << nextRoot << std::endl; 
	    }
	    
	  }
	  
	}

	AO_t curWord = O->windowLoc;
  if(extract_status_from_oprecord(curWord) == ABORTED){
     /// release ownership of all owned nodes
     //std::cout << "aborted" << std::endl;
     abort(data, O);
  }
	
}


void execute_operation(thread_data_t* data, oprec_t * O){

  unsigned pid = data->id;
  


  AO_t curLoc = O->windowLoc;	
  
  Word operation = O->op;
  unsigned opn = map_word_to_operation(operation);
  
  node_t * nextRoot;
  int iter = 0;
  
        //std::cout << "powo000" << std::endl; 
  
  while( extract_position_from_oprecord(curLoc) != NULL){
 	 iter++;
  	
  	nextRoot = (node_t *)extract_position_from_oprecord(curLoc);
  	
  	perform_one_window_operation(data, nextRoot, O, 1070);
  	
  	if(iter > 1000000){
		
		AO_t word = nextRoot->opData;
		oprec_t * N = (oprec_t*)extract_oprec_from_opdata(word);
  		std::cout << "Iterations Exceeded__1302" << std::endl;
  		exit(0);
  		iter = 0;
  	}
  	
  	  curLoc = O->windowLoc;	

 }
 
 // status is completed. 
 
 
  
}




/* ################################################################### *
 * BARRIER
 * ################################################################### */



void barrier_init(barrier_t *b, int n)
{
  pthread_cond_init(&b->complete, NULL);
  pthread_mutex_init(&b->mutex, NULL);
  b->count = n;
  b->crossing = 0;
}

void barrier_cross(barrier_t *b)
{
  pthread_mutex_lock(&b->mutex);
  /* One more thread through */
  b->crossing++;
  /* If not all here, wait */
  if (b->crossing < b->count) {
    pthread_cond_wait(&b->complete, &b->mutex);
  } else {
    pthread_cond_broadcast(&b->complete);
    /* Reset for next time */
    b->crossing = 0;
  }
  pthread_mutex_unlock(&b->mutex);
}

/* ################################################################### *
 * TEST
 * ################################################################### */

void pre_insert(thread_data_t * data, long key){
	oprec_t * O = (oprec_t *)get_new_opRecord(data);
    	O->op = map_key_to_insert_operation(key);
 	O->pid = data->id;
 	//O->state = combine_position_status_oprecord(d->rootOfTree);
 	//O->seq = data->seqNo++;
 	
 	seekrec_t * R = (seekrec_t *)get_new_seekRecord(data);
 	
 	O->sr = R;
 	
 	int iter = 0;
 	
 
 	while(true){
 		iter++;
 	
		seek(data, O, key,R);
		if(R->contents[3] != 0){
       			std::cout << "Whats this?" << std::endl;
       			exit(0);
       		}
	
	
		if(R->leafKey == key){
		
			// key already present in the tree. Terminate
			
			break;
		
		}
		else{
			
			/*if(!validate(data, O)){
				// validation failed. Restart seek.
				std::cout << "Validation Failed" << std::endl;
				exit(0);
				continue;
			}*/
			
			// validation successful. Inject operation into the tree at R->addresses[0]
			O->windowLoc = combine_position_status_oprecord((node_t *)get_address_from_addresses(R->addresses[0]),TRYING);
			O->changer = 1016;
			int res = inject(data, O);
			
			int curStatus = extract_status_from_oprecord(O->windowLoc);
  	node_t * nextRoot = (node_t *)extract_position_from_oprecord(O->windowLoc);
			
			if(curStatus == INJECTED){
			  execute_operation(data, O);
			}
			else{
			  continue;
			}
		
		if(iter > 100000){
		  std::cout << "Infinite loop__11" << std::endl;
		  exit(0);
		}
			
			break;
	
		}
	}
 	

}


void *testRW(void *data){
  long value;
  double action;
  double ksSelect;
  Word key;
  thread_data_t *d = (thread_data_t *)data;
   
  /* Wait on barrier */
  barrier_cross(d->barrier);

  while (stop == 0) {
  	d->case2 = 0;
  	d->case1 = 0;
    
    // determine what we're going to do
    action = (double)rand_r(&d->seed) / (double) RAND_MAX;
   
  	key = rand_r(&d->seed) % (d->keyspace1_size);
	  while(key == 0){
	    key = rand_r(&d->seed) % (d->keyspace1_size);
	  } 

    if (action <= d->search_frac){
   	  //auto search_start = std::chrono::high_resolution_clock::now();
      search(d, key);
	    //auto search_end = std::chrono::high_resolution_clock::now();
    	//auto search_dur = std::chrono::duration_cast<std::chrono::microseconds>(search_end - search_start);
    	//d->tot_read_time += search_dur.count();
	    //d->tot_reads++;
    }
          
    else if (action > d->search_frac && action <= (d->search_frac + d->insert_frac))
    {
	    // Insert Operation
   	  //auto ins_start = std::chrono::high_resolution_clock::now();
	    long leafKey = update_search(d, key); 
	    if(leafKey != key){
	      // Key is not present in the tree. Should be inserted.
	      oprec_t * O = (oprec_t *)get_new_opRecord(d);
    	  O->op = map_key_to_insert_operation(key);
 	      O->pid = d->id;
        seekrec_t * R = (seekrec_t *)get_new_seekRecord(d);
 	      O->sr = R;
     	  int iter = 0;
 	      int lastCase = 0;
 	
 	      node_t * ld1 = NULL;
 	      node_t * ld0 = NULL;
 	      d->lastAborted = NULL;
 	
	      while(true){
		      iter++;
		      /*if(iter > 100000){
			      std::cout << "Iterations exceeded11__" << lastCase << "__" << d->case1  << "__" << d->case2 << std::endl;
			      exit(0);
		      }*/
		
		      for(int i1 = 0; i1 < 4; i1++){
			      R->addresses[i1] = NULL;
			      R->contents[i1] = 0;
		      }
		
	 	      seek(d, O, key, R);
    		  if(R->leafKey == key){
			      // key is now present in the tree. Terminate
			      //auto ins_end = std::chrono::high_resolution_clock::now();
    			  //auto ins_dur = std::chrono::duration_cast<std::chrono::microseconds>(ins_end - ins_start);
			      //d->tot_update_time += ins_dur.count();
			      //d->tot_update_count++;
			      break;
		      }
		      else{
			      O->windowLoc = combine_position_status_oprecord((node_t *)get_address_from_addresses(R->addresses[0]),TRYING);
			      O->changer = 1193;
			     
			      int res = inject(d, O);
			      int curStatus = extract_status_from_oprecord(O->windowLoc);
			      if(curStatus == INJECTED){
			        execute_operation(d, O);
			        leafNodes = 0;
			        AO_t lcword = d->prootOfTree->lChild;
			        node_t * pRootLC = (node_t *)get_child(lcword);
    	 	check_red_property((node_t*)get_child(pRootLC->lChild));
        blackCount = -1;
        check_black_count((node_t*)get_child(pRootLC->lChild), 0);
        long rootkey =  in_order_visit((node_t*)get_child(pRootLC->lChild));
			      }
			      else{
			        if(curStatus != ABORTED){
			          std::cout << "Should Be Aborted!" << std::endl;
			          exit(0);
			        }
			        // Get new oprec and seekrec
			        O = (oprec_t *)get_new_opRecord(d);
              O->op = map_key_to_delete_operation(key);
       	      O->pid = d->id;
              R = (seekrec_t *)get_new_seekRecord(d);
              O->sr = R;
			        continue;
			      }
			      //auto ins_end = std::chrono::high_resolution_clock::now();
			      //auto ins_dur = std::chrono::duration_cast<std::chrono::microseconds>(ins_end - ins_start);
			      //d->tot_insert_time += ins_dur.count();
			      //d->tot_insert_count++;
			      break;
		      }
	      }
      }
    }
    else{
     // Delete Operation
	   int flag = 0;
	   //auto del_start = std::chrono::high_resolution_clock::now();
	
	   long leafKey = update_search(d, key);
     if(leafKey == key){
       // execute expensive operation
       oprec_t * O = (oprec_t *)get_new_opRecord(d);
    	 O->op = map_key_to_delete_operation(key);
 	     O->pid = d->id;
 	     seekrec_t * R = (seekrec_t *)get_new_seekRecord(d);
 	     O->sr = R;
 	     int lastCase = 0;
 	     int iter = 0;

	     while(true){	
		     iter++;
		     /*if(iter > 100000){
			     std::cout << "Iterations exceeded12__" << lastCase << "__" << d->case1  << "__" << d->case2 << std::endl;
			     exit(0);
		     } */
		
		     for(int i1 = 0; i1 < 4; i1++){
			     R->addresses[i1] = NULL;
			     R->contents[i1] = 0;
		     }
		
		     seek(d, O, key,R);
        
		     if(R->leafKey == key){
	         flag = 1;
			
			     O->windowLoc = combine_position_status_oprecord((node_t *)get_address_from_addresses(R->addresses[0]),TRYING);
			     O->changer = 1353;
			     int res = inject(d, O);
			     int curStatus = extract_status_from_oprecord(O->windowLoc);
			     if(curStatus == INJECTED){
			       execute_operation(d, O);
			     }
			     else{
			       if(curStatus != ABORTED){
			         std::cout << "Should Be Aborted!" << std::endl;
			         exit(0);
			       }
			       // Get new oprec and seekrec
			       d->case2 = O->changer;
			       O = (oprec_t *)get_new_opRecord(d);
             O->op = map_key_to_delete_operation(key);
     	       O->pid = d->id;
             R = (seekrec_t *)get_new_seekRecord(d);
             O->sr = R;
			       continue;
			     }
	  	   } 
		     //auto del_end = std::chrono::high_resolution_clock::now();
		     //auto del_dur = std::chrono::duration_cast<std::chrono::microseconds>(del_end - del_start);
		     if(flag == 1){
			     //d->tot_del_wt += O->txns;
			     //d->tot_slowdel_time += del_dur.count();
			     //d->tot_slowdel_count++;	
			     break;
		     }
		     else{
			     //d->tot_fastdel_time += del_dur.count();
			     //d->tot_fastdel_count++; 
			     break;	
		     }
	     }
     }
   }  
   d->ops++;
  }
  return NULL;
}

void catcher(int sig)
{
  printf("CAUGHT SIGNAL %d\n", sig);
}



int main(int argc, char **argv)
{
  struct option long_options[] = {
    // These options don't set a flag
    {"help",                      no_argument,       NULL, 'h'},
    {"table-size",                required_argument, NULL, 't'},
    {"duration",                  required_argument, NULL, 'd'},
    {"num-threads",               required_argument, NULL, 'n'},
    {"seed",                      required_argument, NULL, 's'},
    {"search-fraction",           required_argument, NULL, 'r'},
    {"insert-update-fraction",    required_argument, NULL, 'i'},
    {"delete-fraction",           required_argument, NULL, 'x'},
    {"keyspace1-size",             required_argument, NULL, 'k'},
    {"keyspace2-size",             required_argument, NULL, 'l'},
    {"gc-node-threshold",             required_argument, NULL, 'g'},
    {"reader-threads",             required_argument, NULL, 'a'},
    {NULL, 0, NULL, 0}
  };

//mTable_t** table;
//mTable_t* itable;

//sTable_t** stable;
//sTable_t* istable;

  int i, c;
  char *s;
  unsigned long inserts, deletes, actInsert, actDelete;
  thread_data_t *data;
  pthread_t *threads;
  pthread_attr_t attr;
  barrier_t barrier;
  struct timeval start, end, end1;
  struct timespec timeout;
  
  struct timespec gc_timeout;
  
  int duration = DEFAULT_DURATION;
  
  int tablesize = DEFAULT_TABLE_SIZE;
  int nb_threads = DEFAULT_NB_THREADS;
  double search_frac = DEFAULT_SEARCH_FRAC;
  double insert_frac = DEFAULT_INSERT_FRAC;
  double delete_frac = DEFAULT_DELETE_FRAC;
  long keyspace1_size = DEFAULT_KEYSPACE1_SIZE;
  long keyspace2_size = DEFAULT_KEYSPACE2_SIZE;
  long gcThr = GC_NODE_THRESHOLD;
  int reader_threads = DEFAULT_READER_THREADS;
  int seed = DEFAULT_SEED;
  sigset_t block_set;
  

  while(1) {
    i = 0;
    c = getopt_long(argc, argv, "ht:d:n:s:r:i:x:k:l:g:a:", long_options, &i);

    if(c == -1)
      break;

    if(c == 0 && long_options[i].flag == 0)
      c = long_options[i].val;

    switch(c) {
     case 0:
       // Flag is automatically set 
       break;
     case 'h':
       printf("Double Ended Queue\n"
              "\n"
              "Usage:\n"
              "  deque [options...]\n"
              "\n"
              "Options:\n"
              "  -h, --help\n"
              "        Print this message\n"
              "  -t, --table-size <int>\n"
              "        Number of cells in the hash table (default=" XSTR(DEFAULT_TABLE_SIZE) ")\n"
              "  -d, --duration <int>\n"
              "        Test duration in milliseconds (0=infinite, default=" XSTR(DEFAULT_DURATION) ")\n"
              "  -n, --num-threads <int>\n"
              "        Number of threads (default=" XSTR(DEFAULT_NB_THREADS) ")\n"
              "  -s, --seed <int>\n"
              "        RNG seed (0=time-based, default=" XSTR(DEFAULT_SEED) ")\n"
	      "  -r, --search-fraction <int>\n"
              "        Number of search Threads (default=" XSTR(DEFAULT_SEARCH_FRAC) ")\n"
              "  -i, --insert-update-fraction <int>\n"
              "        Number of insert/update Threads (default=" XSTR(DEFAULT_INSERT_FRAC) ")\n"
              "  -x, --delete-fraction <double>\n"
              "        Fraction of delete operations (default=" XSTR(DEFAULT_DELETE_FRAC) ")\n"
              "  -a, --reader-threads <int>\n"
              "        Number of reader threads (default=" XSTR(DEFAULT_READER_THREADS) ")\n"
              "  -k, --keyspace-size <int>\n"
               "       Number of possible keys (default=" XSTR(DEFAULT_KEYSPACE1_SIZE) ")\n"
              "  -l, --keyspace-size <int>\n"
               "       Number of possible keys (default=" XSTR(DEFAULT_KEYSPACE2_SIZE) ")\n"
              "  -g, --gc <int>\n"
               "       garbage collection threshold (default=" XSTR(GC_NODE_THRESHOLD) ")\n"
         );
       exit(0);
     case 't':
       tablesize = atoi(optarg);
       break;
     case 'd':
       duration = atoi(optarg);
       break;
     case 'n':
       nb_threads = atoi(optarg);
       break;
     case 's':
       seed = atoi(optarg);
       break;
     case 'r':
       search_frac = atof(optarg);
       break;
     case 'i':
       insert_frac = atof(optarg);
       break;
     case 'x':
       delete_frac = atof(optarg);
       break;
     case 'a':
       reader_threads = atoi(optarg);
       break;  
    case 'k':
       keyspace1_size = atoi(optarg);
       break;
     case 'l':
       keyspace2_size = atoi(optarg);
       break;
     case 'g':
       gcThr = atoi(optarg);
       break;
     case '?':
       printf("Use -h or --help for help\n");
       exit(0);
     default:
       exit(1);
    }
  }

  assert(duration >= 0);
  assert(tablesize >= 2);
  assert(nb_threads > 0);
  assert(nb_threads < MAX_PROCESSES);
  assert(reader_threads < nb_threads);


	
  printf("Duration       : %d\n", duration);
  printf("Nb threads     : %d\n", nb_threads);
  printf("Seed           : %d\n", seed);
  printf("Search frac    : %f\n", search_frac); 	
  printf("Insert frac    : %f\n", insert_frac);
  printf("Delete frac    : %f\n", delete_frac);
  printf("Reader Threads : %d\n", reader_threads);
  printf("Update Threads : %d\n", (nb_threads - reader_threads));   
  printf("Keyspace1 size : %d\n", keyspace1_size);
  printf("GC Threshold   : %ld\n", gcThr);


  timeout.tv_sec = duration / 1000;
  timeout.tv_nsec = (duration % 1000) * 1000000;
  
  
  data = new thread_data_t[nb_threads];


  if ((threads = (pthread_t *)malloc(nb_threads * sizeof(pthread_t))) == NULL) {
    perror("malloc");
    exit(1);
  }

  if (seed == 0)
    srand((int)time(0));
  else
    srand(seed);


 
 node_t * pRoot = (node_t *)malloc(sizeof(node_t));
 node_t * pRootRC = (node_t *)malloc(sizeof(node_t));
 node_t * pRootLC = (node_t *)malloc(sizeof(node_t));
 node_t * pRootLCLeaf = (node_t *)malloc(sizeof(node_t));
 node_t * newNode = (node_t *)malloc(sizeof(node_t));

 pRoot->key = (keyspace1_size + 2);	 
 pRoot->color = BLACK;
 pRoot->opData = combine_oprec_status_child_opdata(NULL, NOT_OWNED, LEFT, NOT_INITIAL, NOT_GUARDIAN);
 pRoot->parent = NULL;
 pRoot->lChild = create_child_word(pRootLC,OFIN);
 pRoot->rChild = create_child_word(pRootRC,OFIN);
 pRoot->move = NULL;

 pRootRC->key = (keyspace1_size + 2);	 
 pRootRC->color = BLACK;
 pRootRC->opData = combine_oprec_status_child_opdata(NULL, NOT_OWNED, LEFT, NOT_INITIAL, NOT_GUARDIAN);
 pRootRC->parent = NULL;
 pRootRC->lChild = create_child_word(NULL,OFIN);
 pRootRC->rChild = create_child_word(NULL,OFIN);
 pRootRC->move = NULL;

 pRootLC->key = (keyspace1_size + 1);	 
 pRootLC->color = BLACK;
 pRootLC->opData = combine_oprec_status_child_opdata(NULL, NOT_OWNED, LEFT, NOT_INITIAL, NOT_GUARDIAN);
 pRootLC->parent = NULL;
 pRootLC->lChild = create_child_word(newNode,OFIN);
 pRootLC->rChild = create_child_word(pRootLCLeaf,OFIN);
 pRootLC->move = NULL;
 
 pRootLCLeaf->key = (keyspace1_size + 1);	 
 pRootLCLeaf->color = BLACK;
 pRootLCLeaf->opData = combine_oprec_status_child_opdata(NULL, NOT_OWNED, LEFT, NOT_INITIAL, NOT_GUARDIAN);
 pRootLCLeaf->parent = NULL;
 pRootLCLeaf->lChild = create_child_word(NULL,OFIN);
 pRootLCLeaf->rChild = create_child_word(NULL,OFIN);
 pRootLCLeaf->move = NULL;

  newNode->key = -1;
  newNode->color = BLACK;
  newNode->opData = combine_oprec_status_child_opdata(NULL, NOT_OWNED, LEFT, NOT_INITIAL, NOT_GUARDIAN);
  newNode->parent = pRoot;
  newNode->lChild = NULL;
  newNode->rChild = NULL;
  newNode->move = NULL;
  
  
  //long rootkey2 =  in_order_visit((node_t*)get_child(pRootLC->lChild));
  //std::cout << "Initial sanity check done" << std::endl;
  stop = 0;

//Pre-populate Tree-------------------------------------------------
	int i1 = 0;
	data[i1].id = i1+1;
    data[i1].numThreads = nb_threads;
    data[i1].numInsert = 0;
    data[i1].numDelete = -1;
    data[i1].numActualInsert = 0;
    data[i1].numActualDelete = 0;
    data[i1].seed = rand();
    data[i1].search_frac = 0.0;
    data[i1].insert_frac = 1.0;
    data[i1].delete_frac = 0.0;
    data[i1].keyspace1_size = keyspace1_size;
    data[i1].keyspace2_size = keyspace2_size;
    data[i1].hasInserted = 0;
    data[i1].ops = 0;
    
    data[i1].madeDecision = false;
    data[i1].prootOfTree = pRoot;
    //data[i1].gcInst = 0;
    data[i1].barrier = &barrier;
    data[i1].helpCount = 0;

	 data[i].tot_update_time = 0;
    data[i].tot_insert_time = 0;
    data[i].tot_update_count = 0;
    data[i].tot_insert_count = 0;

    data[i1].tot_fastdel_time = 0;
    data[i1].tot_slowdel_time = 0;
    data[i1].tot_fastdel_count = 0;
    data[i1].tot_slowdel_count = 0;
    data[i1].lastRootCount = 0;
  
    
    data[i1].boCount = 2;
    data[i1].shouldBackOff = false;
	data[i1].nextPid = 1;
	data[i1].nextSPid = 1;
  
    
    data[i1].fastCount = 0;
    data[i1].slowCount = 0;
  
    
    data[i1].readSequenceNumber = 1;
    data[i1].seqNo = 0;
    	 Word key;

  while (data[0].numInsert < (keyspace1_size / 2)) {

    key = rand_r(&data[0].seed) % (keyspace1_size);
    while (key == 0) {
      key = rand_r(&data[0].seed) % (keyspace1_size);
    }
    std::cout << "Inserting --" << key <<  std::endl;
    pre_insert(&data[0], key);
    std::cout << "Done Inserting --" << key << std::endl;
    node_t* temp = (node_t*) get_child(pRoot->lChild);
    node_t* newRoot = (node_t*) get_child(temp->lChild);


    leafNodes = 0;
    check_red_property(newRoot);
    blackCount = -1;
    check_black_count(newRoot, 0);
    long rootkey = in_order_visit(newRoot);

    //long rootkey =  in_order_visit((node_t*)get_child(pRoot->lChild));
  }
    	int pre_inserts = data[0].numInsert;
    		//------------------------------------------------------------------- 
    		std::cout << "Finished pre-population -- " << leafNodes << std::endl;
    		exit(0); 
    		/*check_red_property((node_t*)get_child(pRootLC->lChild));
   blackCount = -1;
   check_black_count((node_t*)get_child(pRootLC->lChild), 0);
   long rootkey1 =  in_order_visit((node_t*)get_child(pRootLC->lChild));
		exit(0); */
  
 
  barrier_init(&barrier, nb_threads + 1);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  
 
    
  for (i = 0; i < nb_threads; i++) {

    data[i].id = i+1;
    data[i].numThreads = nb_threads;
    data[i].numInsert = 0;
    data[i].numDelete = -1;
    data[i].numActualInsert = 0;
    data[i].numActualDelete = 0;
    data[i].seed = rand();
    data[i].search_frac = search_frac;
    data[i].insert_frac = insert_frac;
    data[i].delete_frac = delete_frac;
    data[i].keyspace1_size = keyspace1_size;
    data[i].keyspace2_size = keyspace2_size;
   // data[i].table = &itable;
   // data[i].stable = &istable;
  //  data[i].hplist = hplist;
  //  data[i].maxHpSize = 0;
  //  data[i].maxValidSize = 0;
 // data[i].iretry = 0;
    data[i].hasInserted = 0;
    data[i].ops = 0;
    data[i].madeDecision = false;
    data[i].prootOfTree = pRoot;
   // data[i].gcInst = 0;
    data[i].barrier = &barrier;
    data[i].helpCount = 0;
   data[i].tot_update_time = 0;
    data[i].tot_insert_time = 0;
    data[i].tot_update_count = 0;
    data[i].tot_insert_count = 0;
    
    data[i].tot_fastdel_time = 0;
    data[i].tot_slowdel_time = 0;
    data[i].tot_fastdel_count = 0;
    data[i].tot_slowdel_count = 0;
    data[i].nextPid = 1;
    data[i].nextSPid = 1;
    data[i].lastRootCount = 0;
   
    
    data[i].boCount = 2;
    data[i].shouldBackOff = false;

    data[i].lastCase0 = 0;
    data[i].lastCase1 = 0;
    data[i].lastCase2 = 0;
    
    data[i].fastCount = 0;
    data[i].slowCount = 0;
 
    
    data[i].readSequenceNumber = 1;
    data[i].seqNo = 10000;
    data[i].lastAborted = NULL;
    data[i].lastAbortedContents = NULL;
    data[i].lastAbortMarked = NULL;
 //   data[i].tot_ins_wt = 0;
  //  data[i].tot_del_wt = 0;
    
    
    	if (pthread_create(&threads[i], &attr, testRW, (void *)(&data[i])) != 0) {
    	  fprintf(stderr, "Error creating thread\n");
    	  exit(1);
    	}
 }
  
  pthread_attr_destroy(&attr); 

  /// Catch some signals 
 
  if (signal(SIGHUP, catcher) == SIG_ERR ||
      signal(SIGINT, catcher) == SIG_ERR ||
      signal(SIGTERM, catcher) == SIG_ERR) {
    perror("signal");
    exit(1);
  }

  /// Start threads 
    
    barrier_cross(&barrier);

  
  gettimeofday(&start, NULL);
  if (duration > 0) {
	nanosleep(&timeout, NULL);
	
  } 
  else {
    sigemptyset(&block_set);
    sigsuspend(&block_set);
    }
    
    	AO_store_full(&stop, 1);	
    gettimeofday(&end, NULL);
  
    
  /// Wait for thread completion 
  

   
  for (i = 0; i < nb_threads; i++) {
    if (pthread_join(threads[i], NULL) != 0) {
      fprintf(stderr, "Error waiting for thread completion\n");
      exit(1);
    }
  } 
  
  // CONSISTENCY CHECK
  
  
	
   leafNodes = 0;	
 
 std::cout << "Performing Sanity Check" << std::endl;
  
   check_red_property((node_t*)get_child(pRootLC->lChild));
   blackCount = -1;
   check_black_count((node_t*)get_child(pRootLC->lChild), 0);
   long rootkey =  in_order_visit((node_t*)get_child(pRootLC->lChild));
 	
   unsigned long tot_inserts = 0;
   unsigned long tot_deletes = 0;
   unsigned long tot_ops = 0;
   unsigned long tot_reads = 0;
   double tot_read_time = 0;

   double tot_gc_time = 0;
   unsigned long tot_ptr_dealloc = 0;
   unsigned long tot_rec_dealloc = 0;
   unsigned long tot_data_dealloc = 0;
   unsigned long tot_ptr_free = 0;
   unsigned long tot_data_free = 0;
   unsigned long tot_rec_free = 0;
   unsigned long tot_gc_count = 0;
   
   double tot_update_time = 0;
   
   double tot_insert_time = 0;
   
   double tot_fastdel_time = 0;
   
   double tot_slowdel_time = 0;
   

   unsigned long tot_update_count = 0;
   unsigned long tot_insert_count = 0;
   
   unsigned long tot_fastdel_count = 0;
   unsigned long tot_slowdel_count = 0;
   
   unsigned long tnpc = 0;
   unsigned long tndc = 0; 		
   unsigned long tnrc = 0;
   
unsigned long trpc = 0;
unsigned long trdc = 0;
unsigned long trrc = 0;	
 
    unsigned long fdel = 0;
 
uint64_t tot_fastupdate_count = 0;
 
   uint64_t tot_slowupdate_count = 0;
   	uint64_t inscases = 0;
   	uint64_t retries = 0;
   	   	uint64_t tot_helped = 0;
uint64_t totinswin = 0;
uint64_t totdelwin = 0;

   for(int i = 0; i < nb_threads; i++){
   	tot_inserts += data[i].numInsert;
   	tot_deletes += data[i].numActualDelete;
   	
   	tot_ops += data[i].ops;

   }
   
    tot_inserts+= pre_inserts;
     unsigned long tot_updates = 0;
 for(int i = reader_threads; i < nb_threads; i++){
 	tot_updates += data[i].ops;
 }
    std::cout << "Total Number of Nodes ( " << tot_inserts << ", " << tot_deletes << " ) = " << tot_inserts - tot_deletes << std::endl;
   std::cout << "Total Number of leaf nodes (sanity check) = " << leafNodes << std::endl;
   
   if(leafNodes != (tot_inserts-tot_deletes)){
   	std::cout << "Error. mismatch!!" << std::endl;
   	exit(0);
   }
   
  duration = (end.tv_sec * 1000 + end.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);
  
  std::cout << "*--PERFORMANCE STATISTICS--------*" << std::endl;
  std::cout << "Total Operations per sec = " << (tot_ops * 1000)/duration << std::endl; 
 
  

  
  free(threads);
  delete [] data;
  return 0;
}
