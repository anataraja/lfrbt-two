#include <iostream>
#include <fstream>
#include <stdint.h>
#include <setjmp.h>
#include <vector>
#include <algorithm>
//#include <chrono>
#include "/home/naravind/research/atomic_ops/include/atomic_ops.h"

#define WINDOW_SIZE 10
#define MAX_PROCESSES 512
 
#define DELETE_WINDOW_SIZE 5 

#define PTRNODE_RESERVED_BITS 58
#define DATANODE_RESERVED_BITS 63
#define HELPING_THRESHOLD 1024

#define PREGEN_FACTOR 0.75
#define MAX_SLEEP_TIME 100
#define WAITFREE_THRESHOLD 1000
#define FAST_UPDATE_THRESHOLD 1000 // used in phase 3 of update algorithm

#define RECYCLED_VECTOR_RESERVE 5000000

#define GC_NODE_THRESHOLD 1000000

#define MOST_RECENT_COUNT 4

#define MAX_INSERT_DEPTH 9

#include <pthread.h>



enum {BLACK,RED};


typedef uintptr_t Word;
const unsigned WORD_RESERVED_BITS = 2;


/*-------------------------------------------------------------------------------------------------------------
				STRUCTURES USED BY OUR ALGORITHM
-------------------------------------------------------------------------------------------------------------*/
typedef struct Node node_t;

//typedef AO_t pointerNode_t; // dnode|flag; dnode is 63 bits, flag is 1 bit: owned or free.

typedef struct seekRecord{
	Word addresses[4]; // addresses of pointernodes on the invariant path|which child is the next node
	Word contents[4]; // contents of opdata fields of the nodes on the invariant path
	long leafKey; // key found in leaf node
  long length; // length of c-path 
} seekrec_t;

typedef struct operationRecord{
	seekrec_t * sr;
	
	Word op; // key|type; type is 2 bits (ins,search,del)
	
	Word volatile windowLoc; // position|status; init is 2 bit indicating status i.e. TRYING, INJECTED, ABORTED
	long changer;
	long pid;
	#ifdef DEBUG
	uint64_t seq;
	long lastCas;
	
	AO_t txns; // keep track of number of window transactions.
	long changer1;
	int isLeft; //is wRootChild the lchild of wRoot?
	#endif
	
} oprec_t;


struct Node
{	
	
	AO_t volatile opData; 
	//node_t * windowRoot;
	node_t * markedRoot;
	long key;
	int color;
	//valrec_t* valData;
	//oprec_t* opData;
	
	// addr_of_oprec|state|markbit


	//uint64_t pid;
	
	node_t * parent; // used only while modifying the local window
	AO_t volatile lChild; // address|bit-INTERNAL or LEAF|bit-ONEED or OFIN(default)
	AO_t volatile rChild; // address|bit-INTERNAL or LEAF|bit-ONEED or OFIN(default)
	node_t * move; // move			
	//uint64_t rootCount;
	#ifdef DEBUG
	int mover;
	oprec_t * moverop;
	//int balcase;
	//AO_t contents;
	//oprec_t * lastOp;
	//pointerNode_t * lastP;
 	int slocker;
	long cseq;
 	//AO_t mop;
 	//int depth;
 	//int windowdepth;
 	uint64_t seq;
	int creator;
	#endif
}; 

/*-------------------------------------------------------------------------------------------------------------
***************************************************************************************************************
-------------------------------------------------------------------------------------------------------------*/


typedef struct barrier {
  pthread_cond_t complete;
  pthread_mutex_t mutex;
  int count;
  int crossing;
} barrier_t;


typedef struct thread_data {
  sigjmp_buf env;
  int id;
  unsigned long numThreads;
  unsigned long numInsert;
  long numDelete;
  unsigned long numActualInsert;
  unsigned long numActualDelete;
  unsigned long ops;
  unsigned int seed;
  double search_frac;
  double insert_frac;
  double delete_frac;
  long keyspace1_size;
  long keyspace2_size;
  //sTable_t** stable;
  //mTable_t** table;
 // utable_t ** utable;
//  hpList_t ** hplist;		
//  int lastdel;
  int lastCase0;
  int lastCase1;
  int lastCase2;
  
  int hasInserted;
  bool madeDecision;
  node_t* prootOfTree; // parent of rootOfTree
  barrier_t *barrier;
 int helpCount;
 
 double tot_read_time; 
  
   double tot_update_time;
  double tot_insert_time;
  long tot_update_count;
  long tot_insert_count;
  
  double tot_fastdel_time;
  double tot_slowdel_time;
  long tot_fastdel_count;
  long tot_slowdel_count;
    
  long tot_reads;
  
  int lastCase;
  
  /*double tot_gc_time_sec;
  double tot_gc_time_usec;
  
  unsigned long gcCount;
  unsigned long gcInst;*/
  
 // long tot_reads;
  uint64_t lastRootCount;
  
  node_t * lastAborted;
  AO_t lastAbortedContents;
 node_t * lastAborted1;
  AO_t lastAbortedContents1;
  int lasthelp;
  oprec_t * lastAbortedOprec;
 
	
  int boCount;
  bool shouldBackOff;		

  uint64_t fastCount;
  uint64_t slowCount;

  //uint64_t maxretry;
 // uint64_t inscase1;

 // uint64_t inswinop;
  
//  long gcThreshold;	

  uint64_t readSequenceNumber;
  
  long nextPid; // next process to help
  oprec_t * nextOpdata; // contents of process table
  
  long nextSPid; // id of next process whose search operation is helped.
  
  uint64_t seqNo;
  
  //uint64_t iretry;
  
//  long maxdc;
 // long maxc1;
  
 // long helped;
 
 //long fdelCas;
 
// long tot_ins_wt;
 //long tot_del_wt;
 
// long samehelpcount;
	int case1;
	int case2;



} thread_data_t;


// Helping function declaration

int perform_one_window_operation(thread_data_t* data, node_t* nextRoot, oprec_t * O, int sucid);
int help_within_window(thread_data_t * data, oprec_t * O, node_t * current, AO_t curContents, int sucid);
node_t * set_last_node(thread_data_t * data, node_t * wRootDataNode, int windowDepth, long key);
node_t * extend_current_window(thread_data_t * data, node_t * lastPtrNode,  long key, int windowDepth, node_t * wrootPtrNode, AO_t casField,oprec_t * O);
 bool is_invariant_satisfied(thread_data_t * data, node_t * current, AO_t curContents, oprec_t * O, seekrec_t * R);
 void upgrade_announcement(thread_data_t * data, oprec_t * O);
 void release_announcement(thread_data_t * data, oprec_t * O);
  bool lock_invariant_path(thread_data_t * data, oprec_t * O);
  bool fastcheck_delete_case_1(thread_data_t * data, long key, node_t * curDNode, oprec_t * O);
  void * check_window(thread_data_t * data, node_t * wRootDataNode, int windowDepth, long key);
void update_oprecord(thread_data_t * data, oprec_t * O, node_t * current, AO_t curContents,int id);

int inject(thread_data_t * data, oprec_t * O);


// Operation - least significant 2 bits give the operation, remaining bits give the key
/* ################################################################### *
 * Mapping Definitions
 * ################################################################### */

AO_INLINE int
AO_compare_double_and_swap_double1(volatile AO_double_t *addr,
                                       AO_t old_val1, AO_t old_val2,
                                       AO_t new_val1, AO_t new_val2)
{
  char result;
  __asm__ __volatile__("lock; cmpxchg16b %0; setz %1"
                                : "=m"(*addr), "=q"(result)
                                        : "m"(*addr),
                                          "d" (old_val2),
                                          "a" (old_val1),
                                          "c" (new_val2),
                                          "b" (new_val1)  : "memory");
  return (int) result;
}


AO_INLINE AO_double_t
atomic_load(const volatile AO_double_t *addr)
{
  
  
    while(true){
  const AO_double_t A =  *(const AO_double_t *)addr;
  const AO_double_t B =  *(const AO_double_t *)addr;
  	if(A.AO_val1 == B.AO_val1 && A.AO_val2 == B.AO_val2){
  		return B;
  	}
  
  }
  
}
AO_INLINE AO_t
AO_load1(const volatile AO_t *addr)
{
  /* Cast away the volatile for architectures like IA64 where   */
  /* volatile adds barrier semantics.                           */
  return (*(const AO_t *)addr);
}

#define MARK_BIT 0

inline bool SetBit(volatile unsigned long *array, int bit) {

   // asm("bts %1,%0" :  "+m" (*array): "r" (bit));
     bool flag; 
     __asm__ __volatile__("lock bts %2,%1; setb %0" : "=q" (flag) : "m" (*array), "r" (bit)); return flag; 
   return flag;
}

/*bool mark_node(volatile AO_t * word){
	return (SetBit(word, MARK_BIT));
}*/

bool mark_node(volatile AO_t * word){
	__sync_or_and_fetch(word,1);
}


#define atomic_cas_full(addr, old_val, new_val) (AO_compare_and_swap_full((volatile AO_t *)(addr), (AO_t)(old_val), (AO_t)(new_val)))

//#define atomic_cas_full(addr, old_val, new_val) __sync_bool_compare_and_swap(addr, old_val, new_val);


#define SEARCH_SET 0u
#define INSERT_SET 2u
#define DELETE_SET 1u
#define AND_VALUE 3u
#define STATUS_AND_VALUE 7u


enum {FREE,ANNOUNCE,SLOCK,PLOCK,ABORT,DONE}; // status of a node - 3 bits: 0 = FREE; 1 = ANNOUNCE; 2 = SLOCK; 3 = PLOCK; 4 = ABORT; 5 = DONE
enum {WAITING,INPROGRESS,COMPLETED};
enum {COLLATERAL,PRINCIPAL};
enum {LEFT,RIGHT};
enum {OFIN,ONEED}; // update oprec after execution of window transaction
enum {LEAF,INTERNAL};
enum {NOT_GUARDIAN,IS_GUARDIAN};
enum {NOT_INITIAL,INITIAL};
enum {NOT_OWNED,OWNED};

enum {TRYING,INJECTED,ABORTED};

/*-------------------------------------------------------------------------------------------------------------
				STRUCTURES USED BY OUR ALGORITHM
-------------------------------------------------------------------------------------------------------------*/
	
	
//#define combine_oprec_status_child_opdata(addr,status,child) ((((uintptr_t)addr) << 5) + (status << 2) + (child << 1)) // 1 mark bit

#define combine_oprec_status_child_opdata(record,own,child,initial,guardian) (((uintptr_t)record << 5) + (own << 4) + (child << 3) + (initial << 2) + (guardian << 1)) // 1 mark bit 

//#define combine_oprec_status_child_mark_opdata(addr,status,child) ((((uintptr_t)addr) << 5) + (status << 2) + (child << 1) + 1) // 1 mark bit

#define combine_oprec_status_child_mark_opdata(record,own,child,initial,guardian) (((uintptr_t)record << 5) + (own << 4) + (child << 3) + (initial << 2) + (guardian << 1) + 1) // 1 mark bit 

#define extract_oprec_from_opdata(word) (word >> 5)

//#define extract_status_from_opdata(word) ((word >> 2) & STATUS_AND_VALUE)
#define is_node_owned(word) (((word >> 4) & 1 ==  OWNED)? true:false)

#define extract_child_from_opdata(word) ((word >> 3) & 1)

#define is_initial_txn(word) (((word >> 2) & 1) == INITIAL ? true:false)

#define is_node_guardian(word) (((word >> 1) & 1) == IS_GUARDIAN ? true:false)

#define is_node_marked(word) ((word & 1) == 1 ?true:false)

#define add_mark_flag(word) (word + 1)

#define make_node_not_owned(word) (word & ~(1 << 4))

/* #define create_child_word(addr,type,status) (((uintptr_t)addr << 2) + (type << 1) + status)

#define get_child_status(word) (word & 1)

#define is_ext(word) ((word & 3) >> 1 == LEAF?true:false)

#define get_child(word) (word >> 2) */

#define create_child_word(addr,status) (((uintptr_t)addr << 1) + status)

#define get_child_status(word) (word & 1)

//#define is_ext(word) ((word & 3) >> 1 == LEAF?true:false)

#define get_child(word) (word >> 1)

			//OPRECORD MACROS

#define combine_position_status_oprecord(addr,status) ((((uintptr_t)addr) << 2 ) + status)

#define extract_position_from_oprecord(word) (word >> 2)

#define extract_status_from_oprecord(word) (word & AND_VALUE)

			//OPWORD MACROS
  
#define map_key_to_search_operation(key) ((key << WORD_RESERVED_BITS) + SEARCH_SET)

#define map_key_to_insert_operation(key) ((key << WORD_RESERVED_BITS) + INSERT_SET)

#define map_key_to_delete_operation(key) ((key << WORD_RESERVED_BITS) + DELETE_SET)

//#define add_flag_to_operation(key,flagBit) ((key << 1) + flagBit)

//#define extract_flag_from_operation(word) (word & 1)

#define map_word_to_key(word) (word >> 2)

#define map_word_to_operation(word) ((word) & AND_VALUE)

#define map_word_to_bot_address(addr) ((void *)addr)

// Seek record MACROS
#define get_which_child_from_addresses(word) (word & 1)

#define get_address_from_addresses(word) ((node_t*)(word >> 1))

#define combine_address_which_addresses(addr, which) (((uintptr_t)(addr) << 1) + which)

/*-------------------------------------------------------------------------------------------------------------
***************************************************************************************************************
-------------------------------------------------------------------------------------------------------------*/

/*
AO_t  get_new_pointerNode(thread_data_t * data){
	pointerNode_t * newPtrNode;
	
		
	
		newPtrNode = (pointerNode_t *)malloc(sizeof(pointerNode_t));
		return (uintptr_t)newPtrNode;
	
}
*/

AO_t  get_new_node(thread_data_t * data){
	
	node_t * newNode;
	
	/*if(data->recycledDataNodes.empty()){
		data->ndc++;*/
		 newNode = (node_t *)malloc(sizeof(node_t));
		 newNode->opData = combine_oprec_status_child_opdata(NULL, NOT_OWNED, LEFT, NOT_INITIAL, NOT_GUARDIAN);
		 //newNode->windowRoot = NULL;
		 newNode->markedRoot = NULL;
		 newNode->parent = NULL;
		 newNode->lChild = NULL;
		 newNode->rChild = NULL;
		 newNode->move = NULL;
		 newNode->color = BLACK;
		#ifdef DEBUG
		 newNode->mover = 0;
		 newNode->moverop = NULL;
		 newNode->slocker = 0;
		 newNode->cseq = 0;
		newNode->seq = 0;
		#endif
		//newDNode->rootCount = 0;
		//newDNode->creator = 0;
		//newDNode->balcase = 0;
		//newDNode->contents = 0;
		//newDNode->lastOp = NULL;
		//newDNode->lastP = NULL;
		//newDataNode->testp = NULL;
		//newDataNode->createProc = data->id;
		return (uintptr_t)newNode;
	/*}
	else{
		data->rdc++;
		newDataNode =  (dataNode_t *)extract_dnode_from_ptrNode_field2(data->recycledDataNodes.back());
		//newDataNode->wop2 = 0;
		//newDataNode->testp = NULL;
		//newDataNode->windowopn = 0;
		//newDataNode->freeProc = data->id;
	
		data->recycledDataNodes.pop_back();
			
		return (uintptr_t)newDataNode;

	}*/
}

AO_t get_new_opRecord(thread_data_t * data){
	oprec_t * newRec;
	

		
	/*if(data->recycledRecords.empty()){
		data->nrc++;*/
		newRec = (oprec_t *)malloc(sizeof(oprec_t));
		//newRec->count = 0;
		///newRec->compoundValue.AO_val1 = 0;
		//newRec->compoundValue.AO_val2 = 0;
		newRec->changer = 0;
		#ifdef DEBUG
		newRec->lastCas = 0;
		
		newRec->changer1 = 0;

		newRec->txns = 0;
		#endif
		return (uintptr_t)newRec;
	/*}
	else{

		data->rrc++;	
		newRec =  (record_t *)extract_recordaddr_from_word(data->recycledRecords.back());
		newRec->count = 0;
		newRec->compoundValue.AO_val1 = 0;
		newRec->compoundValue.AO_val2 = 0;

		data->recycledRecords.pop_back();	
		return (uintptr_t)newRec;

	}*/
}



AO_t get_new_seekRecord(thread_data_t * data){
	seekrec_t * newRec;
	

		
	/*if(data->recycledRecords.empty()){
		data->nrc++;*/
		newRec = (seekrec_t *)malloc(sizeof(seekrec_t));
		
		// initialize IPATH-INFO and DCOPY-INFO objects
		
		for(int i = 0; i < 4; i++){
			newRec->addresses[i] = NULL;
			newRec->contents[i] = 0;
			//newRec->dcopies[i] = NULL;
		}
		
		//newRec->count = 0;
		//newRec->compoundValue.AO_val1 = 0;
		//newRec->compoundValue.AO_val2 = 0;
		return (uintptr_t)newRec;
	/*}
	else{
		data->rrc++;	
		newRec =  (record_t *)extract_recordaddr_from_word(data->recycledRecords.back());
		newRec->count = 0;
		newRec->compoundValue.AO_val1 = 0;
		newRec->compoundValue.AO_val2 = 0;
		data->recycledRecords.pop_back();	
		return (uintptr_t)newRec;

	}*/
}

int atomic_cas_full1(thread_data_t * data, node_t * wRoot, AO_t expVal, AO_t newVal){
	
	/*std::ofstream ofile;
		
		char fname[50];
		char title[20] = "file"; 
		 sprintf(fname,"%s_%d",title,data->id);
		ofile.open(fname, std::ios::app);*/
		
	
	if(!is_node_marked(expVal)){
		// node is initially not marked
		int result1 = atomic_cas_full(&wRoot->opData, expVal, newVal);
		if(result1 == 1){
			// cas success
			//ofile << data->seqNo << "_1_" << addr << "\n";
			return result1;
		}
		else{
			//std::cout << "Error. Cas Failure" << std::endl;
			//exit(0);
			// node may have been marked in the meantime.
			int result2 = atomic_cas_full(&wRoot->opData, add_mark_flag(expVal), add_mark_flag(newVal));
			//if(result2 == 1){
			//	ofile << data->seqNo << "_2_" << addr << "\n";
			//}
			return result2;
		}	
	}
	else{
		
		int result3 = atomic_cas_full(&wRoot->opData, expVal, add_mark_flag(newVal));
		//if(result3 == 1){
		// 	ofile << data->seqNo << "_3_" << addr << "\n";
		//}
		
		return result3;
	}

}

void replace_node(thread_data_t * data, node_t * curNode, node_t* newNode, node_t * wRootChildCopy){

	if(curNode == NULL || newNode == NULL){
		std::cout << "NULL Encountered in replace_node" << std::endl;
		exit(0);
	}
	
	node_t * par = curNode->parent;
	
	if(par == NULL){
		par = wRootChildCopy;
	}
	
	if((node_t *)get_child(par->lChild) == curNode){
		par->lChild = create_child_word(newNode, OFIN);
	}
	else if((node_t *)get_child(par->rChild) == curNode){
		par->rChild = create_child_word(newNode, OFIN);
	}
	else{
		std::cout << "Pointers incorreclty assigned: replace_node" << std::endl;
		exit(0);
	}
	

}

node_t * get_parent_of_window_root(thread_data_t * data, node_t * nextWindowRootNode, node_t * wRootChildCopy){
  node_t * par = nextWindowRootNode->parent;
	if(par == NULL){
		par = wRootChildCopy;
	}
	assert(par != NULL);
	return par;
}


node_t * lock_new_ipath(thread_data_t * data, node_t * nextWindowRootNode, node_t * wRootChildCopy, oprec_t * O){
	#ifdef DEBUG
	if(nextWindowRootNode == wRootChildCopy){
		std::cout << "How is this happening" << std::endl;
		exit(0);
	}	
	#endif
	
	if(nextWindowRootNode == NULL){
		return NULL;
	}
	
	#ifdef DEBUG
	nextWindowRootNode->slocker = 8597;
	#endif	 		
	
	node_t * par = get_parent_of_window_root(data, nextWindowRootNode, wRootChildCopy);
	
	if((node_t *)get_child(par->lChild) == nextWindowRootNode){
		par->opData = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,IS_GUARDIAN);
	}
	else if((node_t *)get_child(par->rChild) == nextWindowRootNode){
		par->opData = combine_oprec_status_child_opdata(O,OWNED,RIGHT,NOT_INITIAL,IS_GUARDIAN);
	}
#ifdef DEBUG
	else{
		std::cout << "Error. Pointers incorrectly assigned. wfrbt.h 666" << std::endl;
		exit(0);
	}
#endif
  nextWindowRootNode->markedRoot = par;
	return par;
}

int complete_cas(thread_data_t * data, AO_t casField, node_t * wRoot, node_t * wRootChild, node_t * newNode, oprec_t * O, int id){
	int child = extract_child_from_opdata(casField);
		#ifdef DEBUG	
		if(wRoot == data->prootOfTree && child == RIGHT){
			std::cout << "Something's fishy here." << std::endl;
			exit(0);
		}
		
		if(newNode == NULL || newNode == wRootChild){
			std::cout << "Error. Rootchild is NULL" << std::endl;
			exit(0);
		}
		#endif
		int result0;
		if(child == LEFT){
			result0 = atomic_cas_full(&wRoot->lChild, create_child_word(wRootChild,OFIN),create_child_word(newNode,ONEED));
		}
		else{
			result0 = atomic_cas_full(&wRoot->rChild, create_child_word(wRootChild,OFIN),create_child_word(newNode,ONEED));
		}
		
		//int result1 = 
		//atomic_cas_full1(data, wRoot, casField, combine_oprec_status_child_opdata(O, DONE, child));
		
		if(result0 == 1){
			//wRoot->cseq = id;
			AO_t childWord;
			if(child == LEFT){
				childWord = wRoot->lChild;
			}
			else{
				childWord = wRoot->rChild;
			}
			#ifdef DEBUG
			if((node_t *)get_child(childWord) == O->sr->addresses[1]){
				std::cout << "complete_cas_659" << std::endl;
				exit(0);
			}	
			#endif					
		}
	return result0;
}
/*
void update_oprecord(thread_data_t * data, oprec_t * O, node_t * current, AO_t curContents,int id){

	AO_t childWord;
	int child = extract_child_from_opdata(curContents);
	if(child == LEFT){
		childWord = current->lChild; 
	}
	else{
		childWord = current->rChild;
	}
			
	if(get_child_status(childWord) == ONEED){
		
		if(current->opData == curContents){
			// oprecord position needs to be updated
			node_t * childNode = (node_t *)get_child(childWord);
			AO_t posWord = O->windowLoc;
			if((node_t *)extract_position_from_oprecord(posWord) == current){
				
				int result = atomic_cas_full(&O->windowLoc, posWord, combine_position_init_oprecord(childNode->move, COLLATERAL));
#ifdef DEBUG			
if(result == 1){
	
	
	if(O->pid == data->id){
	O->changer1 = id;
	}
	else{
	O->changer1 = id+10000;

	}	
}

#endif				
}
			int result1;
			if(child == LEFT){
				result1 = atomic_cas_full(&current->lChild, childWord, create_child_word(childNode,OFIN));
			}
			else{
				result1 = atomic_cas_full(&current->rChild, childWord, create_child_word(childNode,OFIN));
			}
 		}	
 	}
 }			
*/
