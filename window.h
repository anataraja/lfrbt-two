#pragma once

#include <iostream>


bool is_current_node_owned(thread_data_t * data, AO_t localCopy, node_t * pNodePtr, int helpCount, node_t * wRoot, AO_t casField, oprec_t * O){
	
	oprec_t * helpeeOprec = (oprec_t *)extract_oprec_from_opdata(localCopy);
	
	if((helpeeOprec == NULL) || (helpeeOprec == O)){
	  /// Same Operation
	  return false;
	}
	
	else{
		// An operation has terminated or is present in this node.
		oprec_t * N = (oprec_t *)extract_oprec_from_opdata(localCopy);	
		if(N == NULL){
			std::cout << "Error. No oprec found!" << std::endl;		
			exit(0);
		}
		
		if((node_t *)extract_position_from_oprecord(N->windowLoc) == NULL){
		  // Operation has finished
		  return false;
		}
		
		if(helpCount < HELPING_THRESHOLD){
		    return true;
		}
		help_within_window(data, N, pNodePtr, localCopy, 27);
		
		return true;
	}
}


node_t * in_order_traverse(thread_data_t *data, node_t * pNodePtr, int depth, int windowDepth, long key, node_t * wRoot, AO_t casField, oprec_t * O){
	
	if(depth > (WINDOW_SIZE - 1) || (depth > (windowDepth - 1))){
		return NULL;
	}	
	
	 // TODO: UNCOMMENT THIS FOR NOT COPYING LEAF NODE
		//if(is_external_node(pNodePtr)){
		//	return NULL;
		//}
	
	node_t * bot = (node_t *)map_word_to_bot_address(1);
	
	
	AO_t checkContents = (wRoot->opData);
		
	if((checkContents != casField)){
		// check if root node has been marked.
		if(!is_node_marked(casField)){
			if(checkContents != add_mark_flag(casField)){
				return (bot);
			}
			else{
				casField = add_mark_flag(casField);
			}
		}
		else{
			return (bot);
		}
		// window has been replaced. Hazard!
	}
		
	// mark pNodePtr using SB instruction
		mark_node(&pNodePtr->opData);
		pNodePtr->creator=700+depth;
		pNodePtr->markedRoot = wRoot;
		
	/*-----------------------------------------------------------------*/
	/*-----------------------------------------------------------------*/
		// Make local copy local(u) of pointer(u)
		AO_t localRootPtr = (pNodePtr->opData); 
		
	/*-----------------------------------------------------------------*/
	/*-----------------------------------------------------------------*/	
	
	checkContents = (wRoot->opData);
	if((checkContents != casField)){
		// check if root node has been marked.
		if(!is_node_marked(casField)){
			if(checkContents != add_mark_flag(casField)){
				return (bot);
			}
			else{
				casField = add_mark_flag(casField);
			}
		}
		else{
			return (bot);
		}
		// window has been replaced. Hazard!
	}
	
	int kount = 0;
	int helpCount = 0;
	bool tempFlag = true;
	int boCount = 2;
	bool shouldBackOff = false;
	
	int bcount = 0;
	
	while(tempFlag){
		
		tempFlag = is_current_node_owned(data, localRootPtr, pNodePtr, helpCount, wRoot, casField, O);
		
		if(shouldBackOff == false){
			double backOff = (double)rand_r(&data->seed) / (double) RAND_MAX;
			if(backOff > 0.5){
				shouldBackOff = true;
			}
		}
		
		if(tempFlag == true){
			
			localRootPtr = (pNodePtr->opData); 
	
			checkContents = (wRoot->opData);
			if((checkContents != casField)){
				// check if root node has been marked.
				if(!is_node_marked(casField)){
					if(checkContents != add_mark_flag(casField)){
						return (bot);
					}
					else{
						casField = add_mark_flag(casField);
					}
				}
				else{
					return (bot);
				}
				// window has been replaced. Hazard!
			}
			
			
			if(shouldBackOff == false){
					helpCount = HELPING_THRESHOLD; // go ahead and help the other thread
			}
			else{
				int currentRange;
				currentRange = (boCount)*((double)rand_r(&data->seed) / (double) RAND_MAX);
				if(currentRange > HELPING_THRESHOLD){
					currentRange = HELPING_THRESHOLD;	
				}
				for(int bo = 0; bo < currentRange; bo++){	
					pthread_yield();
				}	
				boCount = boCount * 2;
				if(boCount > HELPING_THRESHOLD){
					helpCount = HELPING_THRESHOLD;
				}
			}
			
		}
		bcount++;	
		
	}
	
	checkContents = (wRoot->opData);
			if((checkContents != casField)){
				// check if root node has been marked.
				if(!is_node_marked(casField)){
					if(checkContents != add_mark_flag(casField)){
						return (bot);
					}
					else{
						casField = add_mark_flag(casField);
					}
				}
				else{
					return (bot);
				}
			}
	
	if(key < pNodePtr->key){
		//make copy of left subtree
		node_t * newLC = NULL;
		node_t * newRC = NULL;
		
			
		if((node_t *)get_child(pNodePtr->lChild) != NULL){
				newLC = in_order_traverse(data,(node_t *)get_child(pNodePtr->lChild), depth+1, windowDepth, key, wRoot, casField,O);
		}
		
		if(newLC == bot){
			return (bot);
		}
		
		node_t * newNode = (node_t *)get_new_node(data);
		
		(newNode->opData) = combine_oprec_status_child_opdata(NULL,NOT_OWNED,LEFT, NOT_INITIAL, NOT_GUARDIAN);
			
		newNode->key = pNodePtr->key;
		
		newNode->color = pNodePtr->color;
		if(newLC == NULL){ 
			newNode->lChild = pNodePtr->lChild;
		}
		
		else{
			newNode->lChild = create_child_word(newLC,OFIN);
			newLC->parent = (newNode);
		}
		
		int nextDepth;
		if(depth == windowDepth -1){
			nextDepth = windowDepth;
		}
		else{
			nextDepth = windowDepth - 1;
		}
		
		if((node_t *)get_child(pNodePtr->rChild) != NULL){
				newRC = in_order_traverse(data, (node_t *)get_child(pNodePtr->rChild), nextDepth, windowDepth, key, wRoot, casField,O);
		}
		if(newRC == bot){
			return (bot);
		}
		
		
		
		if(newRC == NULL){
			newNode->rChild = pNodePtr->rChild;
		}
		
		else{
			newNode->rChild = create_child_word(newRC,OFIN);
			newRC->parent = (newNode);
		}
		
		return ((newNode));
	}
	
	else{
		// make copy of right subtree
		
		node_t * newLC = NULL;
		node_t * newRC = NULL;
		
		int nextDepth;
		if(depth == windowDepth -1){
			nextDepth = windowDepth;
		}
		else{
			nextDepth = windowDepth - 1;
		}	
	
		if((node_t *)get_child(pNodePtr->lChild) != NULL){
				newLC = in_order_traverse(data, (node_t *)get_child(pNodePtr->lChild), nextDepth, windowDepth, key, wRoot, casField,O);
		}	
		if(newLC == bot){
			return (bot);
		}
		
		
		
		node_t * newNode = (node_t *)get_new_node(data);
		
		(newNode->opData) = combine_oprec_status_child_opdata(NULL,NOT_OWNED,LEFT, NOT_INITIAL, NOT_GUARDIAN);
			
		newNode->key = pNodePtr->key;
		newNode->color = pNodePtr->color;
		if(newLC == NULL){ 
			newNode->lChild = pNodePtr->lChild;
		}
		
		else{
			newNode->lChild = create_child_word(newLC,OFIN);
			newLC->parent = (newNode);
		}
		node_t * testlc = (node_t *)get_child(newNode->lChild);
		if((node_t *)get_child(pNodePtr->rChild) != NULL){
				newRC = in_order_traverse(data, (node_t *)get_child(pNodePtr->rChild), depth+1, windowDepth, key, wRoot, casField,O);
		}
		if(newRC == bot){
			return (bot);
		}
		
		if(newRC == NULL){
			newNode->rChild = pNodePtr->rChild;
		}
		
		else{
			newNode->rChild = create_child_word(newRC,OFIN);
			newRC->parent = (newNode);
		}
		
		return ((newNode));
	}

}

node_t * set_last_node(thread_data_t * data, node_t * wRootChild, int windowDepth, long key){
	int depth = 1;
	
	if(is_external_node(wRootChild)){
		return NULL;
	}
	
	while(depth < windowDepth){
		
		if(key < wRootChild->key){
			wRootChild = (node_t *)get_child(wRootChild->lChild);
		}
		else{
			wRootChild = (node_t *)get_child(wRootChild->rChild);
		}
		
		if(wRootChild == NULL){
			return NULL;
		}
		
		depth++;
	}
	
		
	return wRootChild;
}


node_t* make_window_copy(thread_data_t * data, node_t * wRootChild, long key, int windowDepth, node_t * wRoot, AO_t casField, oprec_t * O){

	node_t * newLC = NULL;
	node_t * newRC = NULL;
	
	node_t * bot = (node_t *)map_word_to_bot_address(1);
	
	AO_t checkContents = (wRoot->opData);
		
	if((checkContents != casField)){
		// check if root node has been marked.
		if(!is_node_marked(casField)){
			if(checkContents != add_mark_flag(casField)){
				return ((node_t *)map_word_to_bot_address(1));
			}
			else{
				casField = add_mark_flag(casField); // can be marked by a process that found wRoot in its window and is helping the current operation out. So we continue
			}
		}
		else{
			return ((node_t *)map_word_to_bot_address(1));
		}
		// window has been replaced. Hazard!
	}
		
	// Mark child of Window Root
	mark_node(&wRootChild->opData);
	wRootChild->creator=602;
	if(key < wRootChild->key){
		int state = LEAF;
		if((node_t *)get_child(wRootChild->lChild) != NULL){
				newLC = in_order_traverse(data, (node_t *)get_child(wRootChild->lChild), 1, windowDepth, key, wRoot, casField,O);
		}
		
		if(newLC == bot){
			return ((node_t *)map_word_to_bot_address(1));
		}
		
		node_t * newNode = (node_t *)get_new_node(data);
		
		newNode->key = wRootChild->key;
		newNode->opData = NULL;

		if(newLC == NULL){
			newNode->lChild = wRootChild->lChild;
		}
		else{
			newNode->lChild = create_child_word(newLC, OFIN);
		}
		
		newNode->color = wRootChild->color;
		state = LEAF;
		
		if((node_t *)get_child(wRootChild->rChild) != NULL){
				newRC = in_order_traverse(data, (node_t *)get_child(wRootChild->rChild), windowDepth -1, windowDepth, key, wRoot, casField,O);
		}

		if(newRC == bot){
			return ((node_t *)map_word_to_bot_address(1));
		}		
		
		if(newRC == NULL){
			newNode->rChild = wRootChild->rChild;
			}
		else{
			newNode->rChild = create_child_word(newRC,OFIN);
		}
	return newNode;
	}
	
	else{
		int state = LEAF;
		if((node_t *)get_child(wRootChild->lChild) != NULL){
			 newLC = in_order_traverse(data, (node_t *)get_child(wRootChild->lChild), windowDepth - 1, windowDepth, key, wRoot, casField,O);
		}
		
		if(newLC == bot){
			return ((node_t *)map_word_to_bot_address(1));
		}
		
		node_t * newNode = (node_t *)get_new_node(data);
	
		newNode->key = wRootChild->key;
		
		newNode->opData = NULL;
		if(newLC == NULL){
			newNode->lChild = wRootChild->lChild;
		}
		else{
			newNode->lChild = create_child_word(newLC,OFIN);
		}
		
		newNode->color = wRootChild->color;
		
		state = LEAF;
		if((node_t *)get_child(wRootChild->rChild) != NULL){
				newRC = in_order_traverse(data, (node_t *)get_child(wRootChild->rChild), 1, windowDepth, key, wRoot, casField,O);
		}
		
		if(newRC == bot){
			return ((node_t *)map_word_to_bot_address(1));
		}
		if(newRC == NULL){
			newNode->rChild = wRootChild->rChild;
		}
		else{
			newNode->rChild = create_child_word(newRC,OFIN);
		}
		
	return newNode;
	}
}


node_t * extend_current_window(thread_data_t * data, node_t * lastNode,  long key, int windowDepth, node_t * wRoot, AO_t casField,oprec_t * O){
		node_t * bot = (node_t *)map_word_to_bot_address(1);
		node_t * newLC = NULL;
		node_t * newRC = NULL;
	
		int state = LEAF;
		
		if((node_t *)get_child(lastNode->lChild) != NULL){
				newLC = in_order_traverse(data, (node_t *)get_child(lastNode->lChild), windowDepth - 1, windowDepth, key, wRoot, casField,O);

				if (newLC == bot) {
				  return (bot);
				}

				if (newLC != NULL) {
				  lastNode->lChild = create_child_word(newLC, OFIN);
				}
				else{
				  // leaf node encountered
				  newLC = (node_t *)get_child(lastNode->lChild);
				}
		}			
		
		
			
		
		state = LEAF;
		if((node_t*)get_child(lastNode->rChild) != NULL){
				newRC = in_order_traverse(data , (node_t *)get_child(lastNode->rChild), windowDepth - 1, windowDepth, key, wRoot, casField,O);
				if(newRC == bot){
				  return (bot);
				}

				if(newRC != NULL){
				  lastNode->rChild = create_child_word(newRC,OFIN);
				}
				else{
				  // leaf node encountered
				  newRC = (node_t*)get_child(lastNode->rChild);
				}
		}
		
		
		
		if(newLC != NULL){
			newLC->parent = lastNode;
		}
		
		if(newRC != NULL){
			newRC->parent = lastNode;
		}
		
		if(key < lastNode->key){
			return newLC;
		}
		else{
			return newRC;
		}
}



node_t * in_order_traverse_delete(thread_data_t *data, node_t * pNodePtr, int depth, node_t * wRoot, AO_t casField,oprec_t * O){
	data->lastCase = 0;

	if(depth > (DELETE_WINDOW_SIZE - 1)){
		return NULL;
	}
	
	// TODO: UNCOMMENT THIS FOR NOT COPYING LEAF NODE
	//	if(is_external_node(pNodePtr)){
	//		return NULL;
	//	}
	
	node_t * bot = (node_t *)map_word_to_bot_address(1);	
		
		AO_t checkContents = (wRoot->opData);
			if((checkContents != casField)){
				// check if root node has been marked.
				if(!is_node_marked(casField)){
					if(checkContents != add_mark_flag(casField)){
						return (bot);
					}
					else{
						casField = add_mark_flag(casField);
					}
				}
				else{
					return (bot);
				}
			}
		
		// mark pNodePtr using SB instruction
			mark_node(&pNodePtr->opData);
			pNodePtr->creator=800+depth;
			pNodePtr->markedRoot = wRoot;
		AO_t localRootPtr = (pNodePtr->opData); 
		
		checkContents = (wRoot->opData);
			if((checkContents != casField)){
				// check if root node has been marked.
				if(!is_node_marked(casField)){
					if(checkContents != add_mark_flag(casField)){
						return (bot);
					}
					else{
						casField = add_mark_flag(casField);
					}
				}
				else{
					return (bot);
				}
			}
	
	int helpCount = 0;
	bool tempFlag = true;
	int boCount = 2;
	bool shouldBackOff = false;
	int bCount = 10000;
	while(tempFlag){
		
		tempFlag = is_current_node_owned(data, localRootPtr, pNodePtr, helpCount, wRoot,casField, O);
		
		if(shouldBackOff == false){
			double backOff = (double)rand_r(&data->seed) / (double) RAND_MAX;
			if(backOff > 0.5){
				shouldBackOff = true;
			}
		}
		
		if(tempFlag == true){
		localRootPtr = pNodePtr->opData;	
		
			checkContents = (wRoot->opData);
			if((checkContents != casField)){
				// check if root node has been marked.
				if(!is_node_marked(casField)){
					if(checkContents != add_mark_flag(casField)){
						return (bot);
					}
					else{
						casField = add_mark_flag(casField);
					}
				}
				else{
					return (bot);
				}
			}
			
			
			
			if(shouldBackOff == false){
					helpCount = HELPING_THRESHOLD; // go ahead and help the other thread
			}
			else{
				int currentRange;
				currentRange = (boCount)*((double)rand_r(&data->seed) / (double) RAND_MAX);
				if(currentRange > HELPING_THRESHOLD){
					currentRange = HELPING_THRESHOLD;
				}	
				for(int bo = 0; bo < currentRange; bo++){	
					pthread_yield();
				}	
				boCount = boCount * 2;
				if(boCount > HELPING_THRESHOLD){
					helpCount = HELPING_THRESHOLD;
				}
			}
			
		}
		bCount++;	
		
	}	
	checkContents = (wRoot->opData);
			if((checkContents != casField)){
				// check if root node has been marked.
				if(!is_node_marked(casField)){
					if(checkContents != add_mark_flag(casField)){
						return (bot);
					}
					else{
						casField = add_mark_flag(casField);
					}
				}
				else{
					return (bot);
				}
				// window has been replaced. Hazard!
			}
	
	
	
	
	node_t * newLC = NULL;
	node_t * newRC = NULL;
	
	
		if((node_t *)get_child(pNodePtr->lChild) != NULL){
				newLC = in_order_traverse_delete(data, (node_t *)get_child(pNodePtr->lChild), depth+1, wRoot, casField,O);
		}	
		
		if(newLC ==bot){
			return (bot);
		}
		node_t * newNode = (node_t *)get_new_node(data);
		(newNode->opData) = combine_oprec_status_child_opdata(NULL,NOT_OWNED,LEFT, NOT_INITIAL, NOT_GUARDIAN);
			
		newNode->key = pNodePtr->key;
		
		newNode->color = pNodePtr->color;
		if(newLC == NULL){ 
			newNode->lChild = pNodePtr->lChild;
		}
		
		else{
			newNode->lChild = create_child_word(newLC,OFIN);
			newLC->parent = (newNode);
		}
		
		if((node_t *)get_child(pNodePtr->rChild) != NULL){
				newRC = in_order_traverse_delete(data, (node_t *)get_child(pNodePtr->rChild), depth+1, wRoot, casField,O);
		}
		
		if(newRC == bot){
			return (bot);
		}
		
		if(newRC == NULL){
			newNode->rChild = pNodePtr->rChild;
		}
		
		else{
			newNode->rChild = create_child_word(newRC,OFIN);
			newRC->parent = (newNode);
		}
		return ((newNode));
}


node_t* make_delete_window_copy(thread_data_t * data, node_t * wRootChild, long key, node_t * wRoot, AO_t casField, oprec_t * O){
	node_t * bot = (node_t *)map_word_to_bot_address(1);
	node_t * newLC = NULL;
	node_t * newRC = NULL;
	AO_t checkContents = (wRoot->opData);
			if((checkContents != casField)){
				// check if root node has been marked.
				if(!is_node_marked(casField)){
					if(checkContents != add_mark_flag(casField)){
						return ((node_t *)map_word_to_bot_address(1));
					}
					else{
						casField = add_mark_flag(casField);
					}
				}
				else{
					return ((node_t *)map_word_to_bot_address(1));
				}
			}
		
	
	mark_node(&wRootChild->opData);
	wRootChild->creator=1197;
	
	
if(key < wRootChild->key){
	int state = LEAF;
	if((node_t *)get_child(wRootChild->lChild) != NULL){
			newLC = in_order_traverse_delete(data, (node_t *)get_child(wRootChild->lChild), 1, wRoot, casField,O);
	}
	
	if(newLC == bot){
		return ((node_t *)map_word_to_bot_address(1));
	}
	
	node_t * newNode = (node_t *)get_new_node(data);
		
	newNode->key = wRootChild->key;
	newNode->opData = combine_oprec_status_child_opdata(NULL,NOT_OWNED,LEFT, NOT_INITIAL, NOT_GUARDIAN);
	if(newLC != NULL){
		newNode->lChild = create_child_word(newLC,OFIN);
	}
	else{
		newNode->lChild = wRootChild->lChild;
	}	
	newNode->color = wRootChild->color;
	
	state = LEAF;
	if((node_t *)get_child(wRootChild->rChild) != NULL){
			newRC = in_order_traverse_delete(data,  (node_t *)get_child(wRootChild->rChild), DELETE_WINDOW_SIZE - 3, wRoot, casField,O);
	}
	
	if(newRC == bot){
		return ((node_t *)map_word_to_bot_address(1));
	}
	
	
	if(newRC != NULL){
		newNode->rChild = create_child_word(newRC,OFIN);
			if(get_child_status(newNode->rChild) == ONEED ){
				std::cout << "ONEED_1330" << std::endl;
				exit(0);
			}
	}
	else{
		newNode->rChild = wRootChild->rChild;
	}	
	return newNode;
}
else{
	int state = LEAF;
	if((node_t *)get_child(wRootChild->lChild) != NULL){
			newLC = in_order_traverse_delete(data, (node_t *)get_child(wRootChild->lChild), DELETE_WINDOW_SIZE - 3, wRoot, casField,O);
	}
	
	if(newLC == bot){
		return ((node_t *)map_word_to_bot_address(1));
	}
	
	node_t * newNode = (node_t *)get_new_node(data);
			
	newNode->key = wRootChild->key;
	newNode->opData =combine_oprec_status_child_opdata(NULL,NOT_OWNED,LEFT, NOT_INITIAL, NOT_GUARDIAN);
	if(newLC != NULL){
		newNode->lChild = create_child_word(newLC,OFIN);
	}
	else{
		newNode->lChild = wRootChild->lChild;
	}	
	newNode->color = wRootChild->color;
	
	state = LEAF;
	if((node_t *)get_child(wRootChild->rChild) != NULL){
			newRC = in_order_traverse_delete(data, (node_t *)get_child(wRootChild->rChild), 1, wRoot, casField,O);
	}
	
	if(newRC == bot){
		return ((node_t *)map_word_to_bot_address(1));
	}
	
	if(newRC != NULL){
		newNode->rChild = create_child_word(newRC,OFIN);
	}
	else{
		newNode->rChild = wRootChild->rChild;
	}

	return newNode;
}
}

void check_current_window(thread_data_t *data, int key, node_t * root, int windowSize){

  if(root == NULL){
    return;
  }
  if(is_node_marked(root->opData)){
    std::cout << "Error. Root is marked" << std::endl;
  }
  int depth = 0;
  while(depth < windowSize){
    if(is_external_node(root)){
      return;
    }
    node_t *lchild = (node_t *)get_child(root->lChild);
    node_t *rchild = (node_t *)get_child(root->rChild);

    if( is_node_marked(lchild->opData)){
      std::cout << "Error. Left Child is marked" << std::endl;
      exit(0);
    }
    if (is_node_marked(rchild->opData)) {
      std::cout << "Error. Right Child is marked" << std::endl;
      exit(0);
    }
    if(key < root->key){
      root = lchild;
    }
    else{
      root = rchild;
    }
    depth++;
  }

}

