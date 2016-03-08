#pragma once


bool is_external_node(node_t * current){
  if(((node_t *)get_child(current->lChild) == NULL) && ((node_t *)get_child(current->rChild) == NULL)){
    return true;
  }
  return false;
}

bool is_parent_of_external_node(node_t * current, long key){
  node_t * next = (node_t *)get_child(current->lChild);
  node_t * sib = (node_t *)get_child(current->rChild);
#ifdef DEBUG
  if(next == NULL || sib == NULL){
    std::cout << "Error. Leaf node, not parent." << std::endl;
    exit(0);
  }
#endif

  if((is_external_node(next)) || (is_external_node(sib))){
    return true;
  }
  return false;
}

node_t * check_insert_case_2(thread_data_t * data, node_t * wRootChildCopy, long key, int currentWindowSize, oprec_t * O ){
	/// check if a black node with a black child is encountered
	node_t * nextNode;
	if(key < wRootChildCopy->key){
		nextNode = (node_t *)get_child(wRootChildCopy->lChild);
	}
	else{
		nextNode = (node_t *)get_child(wRootChildCopy->rChild);
	}

	if(is_external_node(nextNode)){
		// external node encountered... return
		return(NULL);
	}
#ifdef DEBUG
	if(nextNode->seq != O->seq){
		std::cout << "Diff_seq_58" << std::endl;
	}
#endif

	int depth = 1;
	while(depth < (WINDOW_SIZE - 1)){
#ifdef DEBUG
		if(nextNode->seq != O->seq){
			std::cout << "Diff_seq_70" << std::endl;
		}
#endif
		if(nextNode->color == RED){
			// find next node on access path
			if(key < nextNode->key){
				nextNode = (node_t *)get_child(nextNode->lChild);
			}
			else{
				nextNode = (node_t *)get_child(nextNode->rChild);
			}

			if(is_external_node(nextNode)){
				// external node encountered... return
				return(NULL);
			}
			
			depth++;
			if(depth >= (currentWindowSize - 1)){
			  std::cout << "Window too small!!!" << std::endl;
				return NULL;
			}
			else
				continue;	
		}
		else{
			if(is_external_node(nextNode)){
				// external node encountered... return
				return(NULL);
			}
			node_t * leftChild = (node_t *)get_child(nextNode->lChild);
			node_t * rightChild = (node_t *)get_child(nextNode->rChild);
			
			if(leftChild != NULL){
#ifdef DEBUG
			  if(leftChild->seq != O->seq && !is_external_node(leftChild)){
				  std::cout << "Diff_seq_107" << std::endl;
				}
#endif
				if(leftChild->color == BLACK){
				  // Left child cannot be a leaf node
				  if(is_external_node(leftChild)){
				    return NULL;
				  }

					nextNode->opData = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,NOT_GUARDIAN);
					nextNode->creator = 98+depth;
					nextNode->markedRoot = nextNode->parent;
          if(nextNode->parent == NULL){
            nextNode->markedRoot = wRootChildCopy;
          }
					return nextNode; // one child is black of black node
				}	
			}
			
			if(rightChild != NULL){
#ifdef DEBUG
				if(rightChild->seq != O->seq && !is_external_node(rightChild)){
					std::cout << "Diff_Seq_124" << std::endl;
				}	
#endif
        if (rightChild->color == BLACK) {
          if (is_external_node(rightChild)) {
            return NULL;
          }
				  nextNode->opData = combine_oprec_status_child_opdata(O,OWNED,RIGHT,NOT_INITIAL,NOT_GUARDIAN);
				  nextNode->creator = 117+depth;
					nextNode->markedRoot = nextNode->parent;
          if(nextNode->parent == NULL){
            nextNode->markedRoot = wRootChildCopy;
          }
					return nextNode; // one child is black of black node
				}	
			}
			// both children are red

			if(key < nextNode->key){
				nextNode = leftChild;
			}
			else{
				nextNode = rightChild;
			}
			depth++;
			if(depth >= (currentWindowSize - 1)){
			  std::cout << "Window too small!!!" << std::endl;
				return NULL;
			}
			else
				continue;	
		}
	}
	return NULL;
} 


node_t * check_insert_case_1(node_t * wRootChildCopy, long key, int currentWindowSize, oprec_t * O){
	
	int depth = 1;
	node_t * cur = wRootChildCopy;
	node_t * next = NULL;
	node_t * last = NULL;
	while(depth < (WINDOW_SIZE - 1)){
		if(is_external_node(cur)){
			if(depth > 0){
				return(last);
			}
			else{
				std::cout << "ERROR!! EXTERNAL NODE ENCOUNTERED AT DEPTH 0" << std::endl;
				exit(0);
			}
		}
#ifdef DEBUG
		if(cur->seq != O->seq){
			std::cout << "Diff_seq_186" << std::endl;
		}	
#endif		
		if(key < cur->key){
			next = (node_t *)get_child(cur->lChild);
		}
		else{
			next = (node_t *)get_child(cur->rChild);
		}		
			
		depth++;
		if(depth > (currentWindowSize)){
			std::cout << "Depth = " << depth << "__" << currentWindowSize  << std::endl;
			return NULL; // does not satisfy case 1
		}
		last = cur;
		cur = next;	
	}
	std::cout << "Broke out here" << std::endl;
	return NULL;
	// satisfies case 1
}


node_t * balance_after_insertion(thread_data_t * data, node_t * cur, node_t * wRootChildCopy){
	std::cout << "Calling balance_after_ins " << std::endl;

  node_t * pNode = cur->parent;
	if(pNode == NULL){
	  //std::cout << "Should not be here0!" << std::endl;
	  //exit(0);
	  std::cout << "pNode is wrootRC" << std::endl;
		pNode = wRootChildCopy;
	}
	
	//NOTE: Children of windowRoot also have NULL for parent
	if(pNode->color == BLACK){
	    std::cout << "ret 0" << std::endl;
			return wRootChildCopy;
		}

	// now current node and its parent are red

	node_t * pPNode = pNode->parent; // Parent of parent node
	if(pPNode == NULL){
	  //std::cout << "Should not be here1!" << std::endl;
	  //   exit(0);
		// Parent of parent does not exist.
    if(pNode == wRootChildCopy){
			pNode->color = BLACK;
			std::cout << "ret 1" << std::endl;
			return pNode;
    }
	  else{
			// parent is child of windowRoot
	    std::cout << "ppNode is wrootRC" << std::endl;
			pPNode = wRootChildCopy;
		}
	}

	node_t * pSNode = NULL;
	if((node_t *)get_child(pPNode->lChild) == pNode){
	  pSNode = (node_t *)get_child(pPNode->rChild);
	}
	else if((node_t *)get_child(pPNode->rChild) == pNode){
		pSNode = (node_t *)get_child(pPNode->lChild);
	}
  else{
    std::cout << "The garbage is here" << std::endl;
    exit(0);
  }

	// case 3b
	if(pSNode->color == RED){
	  std::cout << "3b" << std::endl;
  	// both parent and uncle are red
		// color parent and uncle black, color parent(parent) red and proceed upwards
			
		pSNode->color = BLACK;
		pNode->color = BLACK;
		pPNode->color = RED;
		// Only the root of the tree can have two of its children red
		if(pPNode == wRootChildCopy){
		  std::cout << "ret 2" << std::endl;
			return wRootChildCopy;
		}
		
		// otherwise we proceed upwards
		node_t * finalRootNode = balance_after_insertion(data, pPNode, wRootChildCopy);
		return finalRootNode;
	}
	else{
	  /// case 3d and 3e - Uncle is black
		std::cout << "3d,3e" << std::endl;

		node_t * pPPNode = NULL;
		if(pPNode == wRootChildCopy){
		  pPPNode = NULL;
		}
		else{
			pPPNode = pPNode->parent;
			if(pPPNode == NULL){
				pPPNode = (wRootChildCopy);
			}
		}
		
		// case 3d
		if((node_t *)get_child(pNode->lChild) == cur){
		  // curNode is left Child of parent
			// case 3d(a)
			std::cout << "3d(a)" << std::endl;
			if((node_t *)get_child(pPNode->lChild) == pNode){
			  // parent is also left child
				std::cout << "3d(a)LL" << std::endl;
				  pPNode->lChild = pNode->rChild;
					pNode->rChild = create_child_word(pPNode,OFIN);
					pNode->color = BLACK;
					pPNode->color = RED;
					return pNode;
			}
			else{ 
			  // parent is right child
				// parent of parent is root of tree
				std::cout << "3d(a)RL" << std::endl;
				if(pPPNode == NULL){
				  pNode->lChild = cur->rChild;
					pPNode->rChild = cur->lChild;
					cur->rChild = create_child_word(pNode,OFIN);
					cur->lChild = create_child_word(pPNode,OFIN);
					cur->color = BLACK;
					pPNode->color = RED;
					return cur;
				}
				else{
				  std::cout << "3d(a)340" << std::endl;
				  pNode->lChild = cur->rChild;
					pPNode->rChild = cur->lChild;
					cur->rChild = create_child_word(pNode,OFIN);
					cur->lChild = create_child_word(pPNode,OFIN);
					cur->color = BLACK;
					pPNode->color = RED;
					if((node_t *)get_child(pPPNode->lChild) == pPNode){
						pPPNode->lChild = create_child_word(cur,OFIN);
					}
					else if((node_t *)get_child(pPPNode->rChild) == pPNode){
					  pPPNode->rChild = create_child_word(cur,OFIN);
					}
					else{
						std::cout << "This should never happen. Failure in Case LR " << std::endl;
						exit(0);
					}
				}
		  }
		}
		else{ // current node is right child of parent
		  if((node_t *)get_child(pPNode->lChild) == pNode){
				// case 3e
				std::cout << "3e(a)" << std::endl;
				if(pPPNode == NULL){
					pNode->rChild = cur->lChild;
					pPNode->lChild = cur->rChild;
					cur->lChild = create_child_word(pNode,OFIN);
					cur->rChild = create_child_word(pPNode,OFIN);
					cur->color = BLACK;
					pPNode->color = RED;
					return cur;
				}
				else{
					std::cout << "3e(b)" << std::endl;
					pNode->rChild = cur->lChild;
					pPNode->lChild = cur->rChild;
					cur->lChild = create_child_word(pNode,OFIN);
					cur->rChild = create_child_word(pPNode,OFIN);
					cur->color = BLACK;
					pPNode->color = RED;
					if((node_t *)get_child(pPPNode->lChild) == pPNode){
						pPPNode->lChild = create_child_word(cur,OFIN);
					}
					else if((node_t *)get_child(pPPNode->rChild) == pPNode) {
						pPPNode->rChild = create_child_word(cur,OFIN);
					}
					else{
						std::cout << "This should never happen. Failure in Case RL " << std::endl;
						exit(0);
					}
				}
			}
			else if((node_t *)get_child(pPNode->rChild) == pNode){ 
			  // parent is right child
				std::cout << "3eRR" << std::endl;
					pPNode->rChild = pNode->lChild;
					pNode->lChild = create_child_word(pPNode,OFIN);
					pNode->color = BLACK;
					pPNode->color = RED;
					return pNode;

			}
			else{
				std::cout << "Aha! What new garbage is this?" << std::endl;
				exit(0);
			}
		}
	}
	return NULL;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
node_t * handle_case_3(thread_data_t * data, int pid, node_t * wRoot, node_t * wRootChild, node_t * anchor, node_t * wRootChildCopy, long key, int currentWindowSize, AO_t casField, oprec_t * O){
	node_t * lc = (node_t *)get_child(wRootChildCopy->lChild);
	node_t * rc = (node_t *)get_child(wRootChildCopy->rChild);
#ifdef DEBUG
	if(lc->seq != O->seq || rc->seq != O->seq){
		std::cout << "Diff_seq_543" << std::endl;
	}
#endif
	int depth = 0;
	node_t * nextNode;
	
	if(key < wRootChildCopy->key){
		nextNode = lc;
	}
	else{
		nextNode = rc;
	}
	
	if(nextNode->color == BLACK){
		depth = 1; // Y is X as per fig 4 of Tarjan's paper
		while(depth <= 5){//  (WINDOW_SIZE - 1)){
			// set nextDNode color to red	
		
			nextNode->color = RED;
			node_t * leftChild = (node_t *)get_child(nextNode->lChild);
			node_t * rightChild = (node_t *)get_child(nextNode->rChild);
		
#ifdef DEBUG
			if(leftChild->seq != O->seq && !is_external_node(leftChild)){
				std::cout << "Diff_seq_565" << std::endl;
			}
			if(rightChild->seq != O->seq && !is_external_node(rightChild)){
				std::cout << "Diff_seq_568" << std::endl;
			}
#endif	
	
			leftChild->color = BLACK;
			rightChild->color = BLACK;
			
			depth+= 2;
			if(depth < WINDOW_SIZE){
			  if(key < nextNode->key){
				  if(key < leftChild->key){
				  	nextNode = (node_t *)get_child(leftChild->lChild);
			  	}
				  else{
				  	nextNode = (node_t *)get_child(leftChild->rChild);
				  }
		  	}
			  else{
				  if(key < rightChild->key){
					  nextNode = (node_t *)get_child(rightChild->lChild);
				  }
				  else{
					  nextNode = (node_t *)get_child(rightChild->rChild);
				  }
			  }
#ifdef DEBUG
			  if(nextNode->seq != O->seq){
				  std::cout << "Diff_Seq_597" << std::endl;
		  	}
#endif	
			}
		}
	
		nextNode->color = RED;
		node_t * leftChild1 = (node_t *)get_child(nextNode->lChild);
		node_t * rightChild1 = (node_t *)get_child(nextNode->rChild);
#ifdef DEBUG
			
		if(leftChild1->seq != O->seq){
			std::cout << "Diff_Seq_610" << std::endl;
		}	
		if(rightChild1->seq != O->seq){
			std::cout << "Diff_Seq_613" << std::endl;	
		}
#endif		
		leftChild1->color = BLACK;
		rightChild1->color = BLACK;
		
		if(key < nextNode->key){
			nextNode = leftChild1;
		}
		else{
			nextNode = rightChild1;
		}
		
	 	if(nextNode != NULL){
      
      /// Lock nodes in p-path(nextNode), i.e. nextRoot and nextNode		 		
		 		
				node_t * nextRoot = nextNode->parent;
				if((node_t *)get_child(nextRoot->lChild) == nextNode){
					nextRoot->opData = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,IS_GUARDIAN);
				}
				else if((node_t *)get_child(nextRoot->rChild) == nextNode){
					nextRoot->opData = combine_oprec_status_child_opdata(O,OWNED,RIGHT,NOT_INITIAL, IS_GUARDIAN);
				}
				else{
					std::cout << "Parent Pointer incorrectly assigned. 576" << std::endl;
					exit(0);
				}
				
				anchor->move = nextRoot;
				
				nextNode->opData = 	combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,NOT_GUARDIAN);
				nextNode->creator = 519;
				nextNode->markedRoot = nextRoot;	
		 		
		 	}
		else{
			anchor->move = NULL;
		}
						
		
		int child = extract_child_from_opdata(casField);
		int result0;
		if(child == LEFT){
			result0 = atomic_cas_full(&wRoot->lChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		}
		else{
			result0 = atomic_cas_full(&wRoot->rChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		}
		
		return NULL;				
	  }
	
	else{
		// Y is the child of X
		node_t * Y = nextNode;
		
		if(key < nextNode->key){
			nextNode = (node_t *)get_child(nextNode->lChild);
		}
		else{
			nextNode = (node_t *)get_child(nextNode->rChild);
		}
		node_t * Y1 = nextNode;
		depth = 2;
		
		while(depth <= 6){
			// set nextDNode color to red	
			nextNode->color = RED;
			node_t * leftC = (node_t *)get_child(nextNode->lChild);
			node_t * rightC = (node_t *)get_child(nextNode->rChild);
			
			leftC->color = BLACK;
			rightC->color = BLACK;
			depth+= 2;
			if(depth < WINDOW_SIZE){
			  if(key < nextNode->key){
			  	if(key < leftC->key){
			  		nextNode = (node_t *)get_child(leftC->lChild);
			  	}
				  else{
				  	nextNode = (node_t *)get_child(leftC->rChild);
			  	}
			  }
			  else{
				  if(key < rightC->key){
				  	nextNode = (node_t *)get_child(rightC->lChild);
			  	}
				  else{
				  	nextNode = (node_t *)get_child(rightC->rChild);
				  }
			  }
			}
		}
		
		
		nextNode->color = RED;
		node_t * leftC1 = (node_t *)get_child(nextNode->lChild);
		node_t * rightC1 = (node_t *)get_child(nextNode->rChild);
		
			leftC1->color = BLACK;
			rightC1->color = BLACK;
			
		if(key < nextNode->key){
			nextNode = leftC1;
		}
		else{
			nextNode = rightC1;
		}	
		
		// TODO: Set status of child of nextNode to SLOCK and opData to O
		
		node_t * finalWRoot = balance_after_insertion(data, Y1, wRootChildCopy);
		
		if (finalWRoot != NULL) {
      // Update anchor node's child to point to finalWRoot
      int which_child_of_anchor = extract_child_from_opdata(wRootChild->opData);
      if (which_child_of_anchor == LEFT) {
        anchor->lChild = create_child_word(finalWRoot, OFIN);
      } else {
        anchor->rChild = create_child_word(finalWRoot, OFIN);
      }
	 	
	 		if(nextNode != NULL){
				
				node_t * nextRoot = nextNode->parent;
				if((node_t *)get_child(nextRoot->lChild) == nextNode){
					nextRoot->opData = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,IS_GUARDIAN);
				}
				else if((node_t *)get_child(nextRoot->rChild) == nextNode){
					nextRoot->opData = combine_oprec_status_child_opdata(O,OWNED,RIGHT,NOT_INITIAL,IS_GUARDIAN);
				}
				else{
					std::cout << "Parent Pointer incorrectly assigned. 728" << std::endl;
					exit(0);
				}
				anchor->move = nextRoot;
				nextNode->opData = 	combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,NOT_GUARDIAN);
				nextNode->creator = 637;
				nextNode->markedRoot = nextRoot;
		 	}
		 	else{
		 		anchor->move = NULL;
		 	}
		  int child = extract_child_from_opdata(casField);
		  int result0;
		  if(child == LEFT){
			  result0 = atomic_cas_full(&wRoot->lChild, create_child_word(wRootChild,OFIN),create_child_word(finalWRoot,ONEED));
		  }
		  else{
			  result0 = atomic_cas_full(&wRoot->rChild, create_child_word(wRootChild,OFIN),create_child_word(finalWRoot,ONEED));
		  }
			return NULL;		
		}
		else{
	 		if(nextNode != NULL){
				node_t * nextRoot = nextNode->parent;
				if((node_t *)get_child(nextRoot->lChild) == nextNode){
					nextRoot->opData = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,IS_GUARDIAN);
				}
				else if((node_t *)get_child(nextRoot->rChild) == nextNode){
					nextRoot->opData = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,IS_GUARDIAN);
				}
				else{
					std::cout << "Parent Pointer incorrectly assigned. 806" << std::endl;
					exit(0);
				}
				
				anchor->move = nextRoot;
				nextNode->opData = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,NOT_GUARDIAN);
				nextNode->creator = 672;
				nextNode->markedRoot = nextRoot;
		 	}
		 	else{
		 		anchor->move = NULL;
		 	}	
		
		  int child = extract_child_from_opdata(casField);
		  int result0;
		  if(child == LEFT){
			  result0 = atomic_cas_full(&wRoot->lChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		  }
		  else{
			  result0 = atomic_cas_full(&wRoot->rChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		  }
		  return NULL;				
		}
	}
	
}



node_t * get_next_node_on_access_path(thread_data_t * data, int pid, node_t * wRoot, node_t * wRootChild, node_t * anchor, node_t * wRootChildCopy, long key, int currentWindowSize, AO_t casField, bool case2flag, oprec_t * O){
	
	// CHECK THAT ROOT OF WINDOW IS BLACK
	
	node_t * nextNode;
	if(!case2flag ){
		
	  nextNode = check_insert_case_2(data, wRootChildCopy, key, currentWindowSize-1, O);
	  if(nextNode == NULL){
		  //std::cout << "case 2 fail" << std::endl;
	  }
	  else{
	 	  //std::cout << "case 2 succeed" << std::endl;
 		  node_t * nextRoot = nextNode->parent;
 		  if(nextRoot == NULL){
		    //nextRoot = wRootChildCopy;
 		    // Should never reach here. In this case, the anchor should become the guardian of the next window
 		    std::cout << "Anchor node should be next guardian" << std::endl;
 		    exit(0);
		  }
		 		
		  if((node_t *)get_child(nextRoot->lChild) == nextNode){
		    nextRoot->opData = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,IS_GUARDIAN);
		  }
		  else if((node_t *)get_child(nextRoot->rChild) == nextNode){
		    nextRoot->opData = combine_oprec_status_child_opdata(O,OWNED,RIGHT,NOT_INITIAL,IS_GUARDIAN);
		  }
		  else{
			  std::cout << "Parent Pointer incorrectly assigned. Ins.h 857" << std::endl;
		  	exit(0);
		  }
	 	
	 	  anchor->move = nextRoot;
	 		
		  int child = extract_child_from_opdata(casField);
		  int result0;
	  	if(child == LEFT){
			  result0 = atomic_cas_full(&wRoot->lChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		  }
		  else{
			  result0 = atomic_cas_full(&wRoot->rChild, create_child_word(wRootChild,OFIN),create_child_word(anchor,ONEED));
		  }

      data->madeDecision = true;
      return NULL;
	  } 
  }
	
	// check for case 1		
	// i.e. whether an external node is encountered
	nextNode = check_insert_case_1(wRootChildCopy, key, currentWindowSize-1,O);
	
	if(nextNode != NULL){
		data->madeDecision = true;
		return (nextNode);
	}
//	Does not satisfy cases 1 and 2. May need to increase the size of the window

	if(currentWindowSize < 11 ){
		return NULL;
	}
	
	//satisfies case 3 - because every black node has 2 red children and an external node was not encountered on the acces path
	 
	 // extend the current window by 1 if Y is not X
	 
	if(key < wRootChildCopy->key){
		nextNode = (node_t *)get_child(wRootChildCopy->lChild);
	}
	else{
		nextNode = (node_t *)get_child(wRootChildCopy->rChild);
	}
	
	if(nextNode->color == RED){
	 
		node_t * lastPtrNode = set_last_node(data, wRootChildCopy, currentWindowSize-1, key);
	  lastPtrNode = extend_current_window(data, lastPtrNode, key, currentWindowSize-1, wRoot, casField,O);
		node_t * bot = (node_t *)map_word_to_bot_address(1);
		if(lastPtrNode == bot){
		  data->madeDecision = true;
			return NULL;	 
		}	
	}		
	nextNode = handle_case_3(data, pid, wRoot, wRootChild,anchor, wRootChildCopy, key, WINDOW_SIZE - 1, casField, O);
	std::cout << "****HANDLED CASE 3****" << std::endl;
	data->madeDecision = true;
	return NULL;
}

