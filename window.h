#include <iostream>


bool is_current_node_owned(thread_data_t * data, AO_t localCopy, node_t * pNodePtr, int helpCount, node_t * wRoot, AO_t casField, oprec_t * O){
	//std::cout << "Helping__" << pNodePtr << "__" << O << std::endl;
	
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
		//std::cout << "Helped" << std::endl;
		help_within_window(data, N, pNodePtr, localCopy, 27);
		
		return true;
	}
}


node_t * in_order_traverse(thread_data_t *data, node_t * pNodePtr, int depth, int windowDepth, long key, node_t * wRoot, AO_t casField, oprec_t * O){
	
	if(depth > (WINDOW_SIZE - 1) || (depth > (windowDepth - 1))){
		return NULL;
	}	
	
	 // TODO: UNCOMMENT THIS FOR NOT COPYING LEAF NODE
		if(is_external_node(pNodePtr)){
			return NULL;
		} 
	
	// add_to_hp_list(data, (uintptr_t)pNodePtr, false,0);
	
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
	//	remove_from_hp_list(data, (uintptr_t)pNodePtr);
		
		//return (bot);
	}
	
		
		
	// mark pNodePtr using SB instruction
		mark_node(&pNodePtr->opData);
		pNodePtr->markedRoot = wRoot;
		//pNodePtr->mop = casField;
		
	/*-----------------------------------------------------------------*/
	/*-----------------------------------------------------------------*/
		// Make local copy local(u) of pointer(u)
		AO_t localRootPtr = (pNodePtr->opData); //atomic_load_pointerNode(data, pNodePtr, casField, wrootPtrNode, popcasField, popPtrNode);
		
		//if(localRootPtr.AO_val1 == 0 && localRootPtr.AO_val2 == 0){
			//remove_from_hp_list(data, (uintptr_t)pNodePtr);
		//	return(bot);
		//}
		
		
	/*-----------------------------------------------------------------*/
	/*-----------------------------------------------------------------*/	
	
	
	//add_to_hp_list(data, localRootPtr.AO_val2, false,0);
	
	checkContents = (wRoot->opData);
	if((checkContents != casField)){
		// check if root node has been marked.
		if(!is_node_marked(casField)){
			if(checkContents != add_mark_flag(casField)){
				//pNodePtr->mop = checkContents;
				return (bot);
			}
			else{
				casField = add_mark_flag(casField);
			}
		}
		else{
			//pNodePtr->mop = 122;
			return (bot);
		}
		// window has been replaced. Hazard!
	//	remove_from_hp_list(data, (uintptr_t)pNodePtr);
		
		//return (bot);
	}
	
	
	
	//pNodePtr->depth = depth;
	//pNodePtr->windowdepth = windowDepth;
	//table_t *table = *(data->table);
	
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
			//remove_from_hp_list(data, localRootPtr.AO_val2);
			
			localRootPtr = (pNodePtr->opData); //atomic_load_pointerNode(data, pNodePtr, casField, wrootPtrNode, popcasField, popPtrNode);
			//dataNode_t * curDNode = (dataNode_t *)extract_dnode_from_ptrnode(localRootPtr);
			//if(localRootPtr.AO_val1 == 0 && localRootPtr.AO_val2 == 0){
				//remove_from_hp_list(data, (uintptr_t)pNodePtr);
			//	return(bot);
			//}	
			
		//	add_to_hp_list(data, localRootPtr.AO_val2, false,0);
	
			checkContents = (wRoot->opData);
			if((checkContents != casField)){
				// check if root node has been marked.
				if(!is_node_marked(casField)){
					if(checkContents != add_mark_flag(casField)){
						//pNodePtr->mop = 172;
						return (bot);
					}
					else{
						casField = add_mark_flag(casField);
					}
				}
				else{
					//pNodePtr->mop = 180;
					return (bot);
				}
				// window has been replaced. Hazard!
			//	remove_from_hp_list(data, (uintptr_t)pNodePtr);
		
				//return (bot);
			}
			
			
			if(shouldBackOff == false){
					helpCount = HELPING_THRESHOLD; // go ahead and help the other thread
			}
			else{
				int currentRange;
				currentRange = (boCount)*((double)rand_r(&data->seed) / (double) RAND_MAX);
				//if(data->id == 2 && currentRange > 2560)
				//	std::cout << "CR__" << currentRange << std::endl;
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
	
	//remove_from_hp_list(data, localRootPtr.AO_val2);
	
	//localRootPtr =  (pNodePtr->opData); //atomic_load_pointerNode(data, pNodePtr, casField, wrootPtrNode, popcasField, popPtrNode);
	
	//if(localRootPtr.AO_val1 == 0 && localRootPtr.AO_val2 == 0){
		//remove_from_hp_list(data, (uintptr_t)pNodePtr);
	//	return(bot);
	//}
	
	
	
	//add_to_hp_list(data, localRootPtr.AO_val2, false,0);
	
		
	checkContents = (wRoot->opData);
			if((checkContents != casField)){
				// check if root node has been marked.
				if(!is_node_marked(casField)){
					if(checkContents != add_mark_flag(casField)){
						//pNodePtr->mop = 230;
						return (bot);
					}
					else{
						casField = add_mark_flag(casField);
					}
				}
				else{
					//pNodePtr->mop = 238;
					return (bot);
				}
				// window has been replaced. Hazard!
			//	remove_from_hp_list(data, (uintptr_t)pNodePtr);
		
				//return (bot);
			}
	
	//dataNode_t * dNodePtr = (dataNode_t *)extract_dnode_from_ptrnode(localRootPtr);
	
	
	
	if(key <= pNodePtr->key){
		//make copy of left subtree
		node_t * newLC = NULL;
		node_t * newRC = NULL;
		
			
		if((node_t *)get_child(pNodePtr->lChild) != NULL){
			//if(is_ext(pNodePtr->lChild)){
				// do not make copy of the node
			//	newLC = NULL;
			//}
			//else{
				newLC = in_order_traverse(data,(node_t *)get_child(pNodePtr->lChild), depth+1, windowDepth, key, wRoot, casField,O);
			//}
		}
		
		if(newLC == bot){
			//remove_from_hp_list(data, (uintptr_t)pNodePtr);
			//remove_from_hp_list(data, localRootPtr.AO_val2);
			//pNodePtr->mop = 270;
			return (bot);
		}
			//std::cout << "iot0__" << newLC << std::endl;
		// make copies of both pointer and data nodes
		//data->cPtrnodes[pid].push_back(pNodePtr);
		//pointerNode_t *newRootPNode = (pointerNode_t *)get_new_pointerNode(data);
		//data->newPtrnodes[pid].push_back(newRootPNode);
	
		
		
		node_t * newNode = (node_t *)get_new_node(data);
		//data->newDatanodes[pid].push_back(newRootDNode);
		//newRootDNode->creator = 215;
		
		//std::cout << "iot1__" << newNode << std::endl;
		//data->cDatanodes[pid].push_back(dNodePtr);
				
		//newRootPNode->field.AO_val1 = combine_move_and_procid(NULL, 0);
		
		//newRootPNode->field.AO_val2 = combine_dnode_and_flag(newRootDNode, 0);
		
		(newNode->opData) = combine_oprec_status_child_opdata(NULL,NOT_OWNED,LEFT, NOT_INITIAL, NOT_GUARDIAN);
			
		newNode->key = pNodePtr->key;
		
		//newRootDNode->op = operation;
		newNode->color = pNodePtr->color;
		//newRootDNode->valData = dNodePtr->valData;
		
                //newRootDNode->parent = NULL;
                //newNode->seq = O->seq;
		//newNode->creator = 305;
		if(newLC == NULL){ 
			newNode->lChild = pNodePtr->lChild;
			//if(get_child_status(newNode->lChild) == ONEED){
			//	std::cout << "Status is ONEED_309" << std::endl;
			//	exit(0);
			//}	
		}
		
		else{
			newNode->lChild = create_child_word(newLC,OFIN);
			//dataNode_t * lCDNode = (dataNode_t *)extract_dnode_from_ptrnode(*(newLCP));
			newLC->parent = (newNode);
			//newRootDNode->contents = localRootPtr;
			//newRootDNode->lastOp = dNodePtr->opData;
			//newRootDNode->lastP = pNodePtr;
			//newRootDNode->rootCount = 243;
		}
		
		//node_t * testlc = (node_t *)get_child(newNode->lChild);
		/*if(testlc != NULL){
		if(is_node_marked(testlc->opData) && (casField == wRoot->opData)){
			std::cout << "CHILD is marked__323" << std::endl;
			exit(0);
		}
		}*/
		int nextDepth;
		if(depth == windowDepth -1){
			nextDepth = windowDepth;
		}
		else{
			nextDepth = windowDepth - 1;
		}
		
		if((node_t *)get_child(pNodePtr->rChild) != NULL){
			//if(is_ext(pNodePtr->rChild)){
				// do not make copy of the node
			//	newRC = NULL;
			//}
			//else{
				newRC = in_order_traverse(data, (node_t *)get_child(pNodePtr->rChild), nextDepth, windowDepth, key, wRoot, casField,O);
			//}
		}
		//std::cout << "iot2__" << newRC << std::endl;
		if(newRC == bot){
			//remove_from_hp_list(data, (uintptr_t)pNodePtr);
		//remove_from_hp_list(data, localRootPtr.AO_val2);
		//pNodePtr->mop = 340;
			return (bot);
		}
		
		
		
		if(newRC == NULL){
			newNode->rChild = pNodePtr->rChild;
			//if(get_child_status(newNode->rChild) == ONEED){
			//	std::cout << "Status is ONEED 360" << std::endl;
			//	exit(0);
			//}
		}
		
		else{
			newNode->rChild = create_child_word(newRC,OFIN);
			//dataNode_t * rCDNode = (dataNode_t *)extract_dnode_from_ptrnode(*(newRCP));
			newRC->parent = (newNode);
			//newRootDNode->contents = localRootPtr;
			//newRootDNode->lastOp = dNodePtr->opData;
			//newRootDNode->lastP = pNodePtr;
			//newRootDNode->rootCount = 272;
		}
		//node_t * testrc = (node_t *)get_child(newNode->rChild);
		/*if(testrc != NULL){
		if(is_node_marked(testrc->opData) && (casField == wRoot->opData)){
			std::cout << "CHILD is marked__366" << std::endl;
			exit(0);
		}
		}*/
		// remove_from_hp_list(data, (uintptr_t)pNodePtr);
		//remove_from_hp_list(data, localRootPtr.AO_val2);
		
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
			//if(!is_ext(pNodePtr->lChild)){
				newLC = in_order_traverse(data, (node_t *)get_child(pNodePtr->lChild), nextDepth, windowDepth, key, wRoot, casField,O);
			//}	
		}	
		//std::cout << "iot3__" << newLC << std::endl;
		if(newLC == bot){
		//	remove_from_hp_list(data, (uintptr_t)pNodePtr);
		//remove_from_hp_list(data, localRootPtr.AO_val2);
		//	pNodePtr->mop = 394;
			return (bot);
		}
		
		
		//pointerNode_t *newRootPNode = (pointerNode_t *)get_new_pointerNode(data);
		//data->cPtrnodes[pid].push_back(pNodePtr);
		//data->newPtrnodes[pid].push_back(newRootPNode);
		
		node_t * newNode = (node_t *)get_new_node(data);
		//newRootDNode->creator = 308;
		//std::cout << "iot4__" << newNode << std::endl;
		//data->newDatanodes[pid].push_back(newRootDNode);
	
		//data->cDatanodes[pid].push_back(dNodePtr);
				
		//newRootPNode->field.AO_val1 = combine_move_and_procid(NULL, 0);
			
		//newRootPNode->field.AO_val2 = combine_dnode_and_flag(newRootDNode, 0);
		
		(newNode->opData) = combine_oprec_status_child_opdata(NULL,NOT_OWNED,LEFT, NOT_INITIAL, NOT_GUARDIAN);
			
		newNode->key = pNodePtr->key;
		//newRootDNode->next = ;
		//newRootDNode->opData = NULL;
		newNode->color = pNodePtr->color;
		//newRootDNode->valData = dNodePtr->valData;
		//newNode->seq = O->seq;	
              //  newRootDNode->parent = NULL;
                //newNode->creator = 439;
		if(newLC == NULL){ 
			newNode->lChild = pNodePtr->lChild;
			//if(get_child_status(newNode->lChild) == ONEED){
			//	std::cout << "status is ONEED_444" << std::endl;
			//	exit(0);	
			//}
			//newRootDNode->contents = localRootPtr;
			//newRootDNode->lastOp = dNodePtr->opData;
			//newRootDNode->lastP = pNodePtr;
			//newRootDNode->rootCount = 330;
		}
		
		else{
			newNode->lChild = create_child_word(newLC,OFIN);
			//dataNode_t * lCDNode = (dataNode_t *)extract_dnode_from_ptrnode(*(newLCP));
			newLC->parent = (newNode);
			//newRootDNode->contents = localRootPtr;
			//newRootDNode->lastOp = dNodePtr->opData;
			//newRootDNode->lastP = pNodePtr;
			//newRootDNode->rootCount = 337;
		}
		node_t * testlc = (node_t *)get_child(newNode->lChild);
		/*if(testlc != NULL){
		if(is_node_marked(testlc->opData) ){
			if(casField == wRoot->opData){
			std::cout << "CHILD is marked__445" << std::endl;
			exit(0);
		}
		}
		}*/
		
		if((node_t *)get_child(pNodePtr->rChild) != NULL){
			//if(!is_ext(pNodePtr->rChild)){
				newRC = in_order_traverse(data, (node_t *)get_child(pNodePtr->rChild), depth+1, windowDepth, key, wRoot, casField,O);
			//}
		}
		//std::cout << "iot5__" << newRC << std::endl;
		if(newRC == bot){
		//	remove_from_hp_list(data, (uintptr_t)pNodePtr);
		//remove_from_hp_list(data, localRootPtr.AO_val2);
		//pNodePtr->mop = 455;
			return (bot);
		}
		
		if(newRC == NULL){
			newNode->rChild = pNodePtr->rChild;
			//if(get_child_status(newNode->rChild) == ONEED){
			//	std::cout << "status is ONEED1_488" << std::endl;
			//	exit(0);	
			//}
			//newRootDNode->contents = localRootPtr;
			//newRootDNode->lastOp = dNodePtr->opData;
			//newRootDNode->lastP = pNodePtr;
			//newRootDNode->rootCount = 360;
		}
		
		else{
			newNode->rChild = create_child_word(newRC,OFIN);
			//dataNode_t * rCDNode = (dataNode_t *)extract_dnode_from_ptrnode(*(newRCP));
			newRC->parent = (newNode);
			//newRootDNode->contents = localRootPtr;
			//newRootDNode->lastOp = dNodePtr->opData;
			//newRootDNode->lastP = pNodePtr;
			//newRootDNode->rootCount = 367;
		}
		
		/*node_t * testrc = (node_t *)get_child(newNode->rChild);
		if(testrc != NULL){
		if(is_node_marked(testrc->opData)&& (casField == wRoot->opData)){
			std::cout << "CHILD is marked__480" << std::endl;
			exit(0);
		}
		}*/
		//remove_from_hp_list(data, (uintptr_t)pNodePtr);
		//remove_from_hp_list(data, localRootPtr.AO_val2);
		return ((newNode));
	}
	
	
	
	

}

node_t * set_last_node(thread_data_t * data, node_t * wRootChild, int windowDepth, long key){
	int depth = 1;
	
	//node_t * nextNode;
	
	if(is_external_node(wRootChild)){
		return NULL;
	}
	
	while(depth < windowDepth){
		
		if(key <= wRootChild->key){
			wRootChild = (node_t *)get_child(wRootChild->lChild);
		}
		else{
			wRootChild = (node_t *)get_child(wRootChild->rChild);
		}
		
		if(wRootChild == NULL){
			return NULL;
		}
		//wRootDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(nextNode));
		
		
		depth++;
	}
	
		
	return wRootChild;
}

/*void * check_window(thread_data_t * data, node_t * wRootChild, int windowDepth, long key){
	int depth = 1;
} */



node_t* make_window_copy(thread_data_t * data, node_t * wRootChild, long key, int windowDepth, node_t * wRoot, AO_t casField, oprec_t * O){

	//table_t *table = *(data->table);
	node_t * newLC = NULL;
	node_t * newRC = NULL;
	
	/*if(!data->cDatanodes[pid].empty()){
		
		while(!data->cDatanodes[pid].empty()){
			data->cDatanodes[pid].pop_back();
		}
	}*/
	/*int status = extract_status_from_opdata(wRootChild->opData);
	if(status != SLOCK){
		std::cout << "Status is not slock!!615__" << status << std::endl;	
		exit(0);
	} */
	
	node_t * bot = (node_t *)map_word_to_bot_address(1);
	
	
	//add_to_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode), false,0);
	
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
	//	remove_from_hp_list(data, (uintptr_t)pNodePtr);
		
		//return (bot);
	}
		
	//std::cout << "mwc0" << std::endl;	
	// Mark child of Window Root
	//mark_node(&wRootChild->opData);
	
	if(key <= wRootChild->key){
		int state = LEAF;
		if((node_t *)get_child(wRootChild->lChild) != NULL){
			//if(!is_ext(wRootChild->lChild)){
			//	state = INTERNAL;
				newLC = in_order_traverse(data, (node_t *)get_child(wRootChild->lChild), 1, windowDepth, key, wRoot, casField,O);
				//std::cout << "mwc1__" << newLC << std::endl;
			 //}
			//else{
				
				//newLC = (node_t *)get_child(wRootChild->lChild);
			//}
		}
		
		if(newLC == bot){
		//	remove_from_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode));
			return ((node_t *)map_word_to_bot_address(1));
		}
		
		node_t * newNode = (node_t *)get_new_node(data);
		
		//std::cout << "mwc2__" << newNode << std::endl;
		
		//data->newDatanodes[pid].push_back(newRootDNode);
		//data->cDatanodes[pid].push_back(rootDNode);
	
		newNode->key = wRootChild->key;
		newNode->opData = NULL;

		//newNode->seq = O->seq;
		//newNode->creator = 679;
		//newRootDNode->parent = NULL;
		if(newLC == NULL){
			newNode->lChild = wRootChild->lChild;
			//if(get_child_status(newNode->lChild) == ONEED ){
			//	std::cout << "ONEED_693" << std::endl;
			//	exit(0);	
			//}
		}
		else{
			newNode->lChild = create_child_word(newLC, OFIN);
		}
		
		/*node_t * testlc = (node_t *)get_child(newNode->lChild);
		if(testlc != NULL){
		if(is_node_marked(testlc->opData)&& (casField == wRoot->opData)){
			std::cout << "CHILD is marked__660" << std::endl;
			exit(0);
		}
		}*/
		newNode->color = wRootChild->color;
		state = LEAF;
		
		if((node_t *)get_child(wRootChild->rChild) != NULL){
			//if(!is_ext(wRootChild->rChild)){
			//	state = INTERNAL;
				newRC = in_order_traverse(data, (node_t *)get_child(wRootChild->rChild), windowDepth -1, windowDepth, key, wRoot, casField,O);
				//std::cout << "mwc3__" << newRC << std::endl;
			//}
			//else{
				
			//	newRC = (node_t *)get_child(wRootChild->rChild);
			//}
		}

		if(newRC == bot){
			//remove_from_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode));
			return ((node_t *)map_word_to_bot_address(1));
		}		
		
		if(newRC == NULL){
			newNode->rChild = wRootChild->rChild;
			//if(get_child_status(newNode->rChild) == ONEED ){
			//	std::cout << "ONEED_731" << std::endl;
			//}
			}
		else{
			newNode->rChild = create_child_word(newRC,OFIN);
		}
        	
        	/*node_t * testrc = (node_t *)get_child(newNode->rChild);
		if(testrc != NULL){
		if(is_node_marked(testrc->opData)&& (casField == wRoot->opData)){
			std::cout << "CHILD is marked__686" << std::endl;
			exit(0);
		}
        	}*/
		//remove_from_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode));
	return newNode;
	}
	
	else{
		int state = LEAF;
		if((node_t *)get_child(wRootChild->lChild) != NULL){
			//if(!is_ext(wRootChild->lChild)){
			 newLC = in_order_traverse(data, (node_t *)get_child(wRootChild->lChild), windowDepth - 1, windowDepth, key, wRoot, casField,O);
			// std::cout << "mwc4__" << newLC << std::endl;
			//	state = INTERNAL;
			//}
			//else{
			//	newLC = (node_t *)get_child(wRootChild->rChild);
			//}
	
		}
		
		if(newLC == bot){
			//remove_from_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode));
			return ((node_t *)map_word_to_bot_address(1));
		}
		
		node_t * newNode = (node_t *)get_new_node(data);
		//std::cout << "mwc5__" << newNode << std::endl;
		//data->newDatanodes[pid].push_back(newRootDNode);
		//data->cDatanodes[pid].push_back(rootDNode);
	
		newNode->key = wRootChild->key;
		
		newNode->opData = NULL;
		//newNode->seq = O->seq;
		//newNode->creator = 761;
		//newRootDNode->parent = NULL;
		if(newLC == NULL){
			newNode->lChild = wRootChild->lChild;
		}
		else{
			newNode->lChild = create_child_word(newLC,OFIN);
		}
		
		/*node_t * testlc = (node_t *)get_child(newNode->lChild);
		if(testlc != NULL){
		if(is_node_marked(testlc->opData)&& (casField == wRoot->opData)){
			std::cout << "CHILD is marked__725" << std::endl;
			exit(0);
		}
		}*/
		newNode->color = wRootChild->color;
		
		state = LEAF;
		if((node_t *)get_child(wRootChild->rChild) != NULL){
			//if(!is_ext(wRootChild->rChild)){
				newRC = in_order_traverse(data, (node_t *)get_child(wRootChild->rChild), 1, windowDepth, key, wRoot, casField,O);
				//std::cout << "mwc6__" << newRC << std::endl;
			//	state = INTERNAL;
			//}
			//else{
			//	newRC = (node_t *)get_child(wRootChild->rChild);
			//}
		}
		
		if(newRC == bot){
			//remove_from_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode));
			return ((node_t *)map_word_to_bot_address(1));
		}
		if(newRC == NULL){
			newNode->rChild = wRootChild->rChild;
		}
		else{
			newNode->rChild = create_child_word(newRC,OFIN);
		}
		
		/*node_t * testrc = (node_t *)get_child(newNode->rChild);
		if(testrc != NULL){
		if(is_node_marked(testrc->opData)&& (casField == wRoot->opData)){
			std::cout << "CHILD is marked__757" << std::endl;
			exit(0);
		}
		}*/
        	//remove_from_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode));

	return newNode;
	}
}


node_t * extend_current_window(thread_data_t * data, node_t * lastNode,  long key, int windowDepth, node_t * wRoot, AO_t casField,oprec_t * O){
		//std::cout << "Extend" << std::endl;
		node_t * bot = (node_t *)map_word_to_bot_address(1);
		node_t * newLC = NULL;
		node_t * newRC = NULL;
	
		
		
		//node_t * lastDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(lastPtrNode)); // still in local window
		
		
		
		int state = LEAF;
		
		if((node_t *)get_child(lastNode->lChild) != NULL){
			//if(!is_ext(lastNode->lChild)){
				newLC = in_order_traverse(data, (node_t *)get_child(lastNode->lChild), windowDepth - 1, windowDepth, key, wRoot, casField,O);
			//	state = INTERNAL;
			//}
			//else{
			//	newLC = (node_t *)get_child(lastNode->lChild);
			//}
		}			
		
		if(newLC == bot){
			return (bot);
		}
		
		if(newLC != NULL){
			lastNode->lChild = create_child_word(newLC,OFIN);
		}
			
		
		state = LEAF;
		if((node_t*)get_child(lastNode->rChild) != NULL){
			//if(!is_ext(lastNode->rChild)){
				newRC = in_order_traverse(data , (node_t *)get_child(lastNode->rChild), windowDepth - 1, windowDepth, key, wRoot, casField,O);
			//	state = INTERNAL;
			//}
			//else{
			//	newRC = (node_t *)get_child(lastNode->rChild);
			//}
		}
		
		if(newRC == bot){
			return (bot);
		}
		
		if(newRC != NULL){
			lastNode->rChild = create_child_word(newRC,OFIN);
		}
		
		if(newLC != NULL){
			//dataNode_t * newLCD = (dataNode_t *)extract_dnode_from_ptrnode(*(newLCP)); // still in local window
			newLC->parent = lastNode;
		}
		
		if(newRC != NULL){
			//dataNode_t * newRCD = (dataNode_t *)extract_dnode_from_ptrnode(*(newRCP)); // still in local window
			newRC->parent = lastNode;
		}
		
		if(key <= lastNode->key){
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
		if(is_external_node(pNodePtr)){
			return NULL;
		} 
	
	node_t * bot = (node_t *)map_word_to_bot_address(1);	
	
		
		
		// add_to_hp_list(data, (uintptr_t)pNodePtr, false,0);
		
		
		
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
			//	remove_from_hp_list(data, (uintptr_t)pNodePtr);
		
				//return (bot);
			}
		
		// mark pNodePtr using SB instruction

		
			
			
			mark_node(&pNodePtr->opData);
			pNodePtr->markedRoot = wRoot;
		//pNodePtr->mop = 858;
		AO_t localRootPtr = (pNodePtr->opData); 
		
		//pNodePtr->depth = depth;
		//pNodePtr->windowdepth = DELETE_WINDOW_SIZE-1;
		
		//add_to_hp_list(data, localRootPtr.AO_val2, false,0);
		
		checkContents = (wRoot->opData);
			if((checkContents != casField)){
				// check if root node has been marked.
				if(!is_node_marked(casField)){
					if(checkContents != add_mark_flag(casField)){
						//pNodePtr->mop = 859;
						return (bot);
					}
					else{
						casField = add_mark_flag(casField);
					}
				}
				else{
					//pNodePtr->mop = 860;
					return (bot);
				}
				// window has been replaced. Hazard!
			//	remove_from_hp_list(data, (uintptr_t)pNodePtr);
		
				//return (bot);
			}
		
		

	//table_t *table = *(data->table);
	
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
			//remove_from_hp_list(data, localRootPtr.AO_val2);
			
			//localRootPtr = *(pNodePtr); //atomic_load_pointerNode(data, pNodePtr, casField, wrootPtrNode, popcasField, popPtrNode);
		localRootPtr = pNodePtr->opData;	
			//dataNode_t * curDNode = (dataNode_t *)extract_dnode_from_ptrnode(localRootPtr);
			
			//if(localRootPtr.AO_val1 == 0 && localRootPtr.AO_val2 == 0){
				//remove_from_hp_list(data, (uintptr_t)pNodePtr);
			//	return(bot);
			//}		
			
			//add_to_hp_list(data, localRootPtr.AO_val2, false,0);
		
			checkContents = (wRoot->opData);
			if((checkContents != casField)){
				// check if root node has been marked.
				if(!is_node_marked(casField)){
					if(checkContents != add_mark_flag(casField)){
					//pNodePtr->mop = 861;
						return (bot);
					}
					else{
						casField = add_mark_flag(casField);
					}
				}
				else{
					//pNodePtr->mop = 862;
					return (bot);
				}
				// window has been replaced. Hazard!
			//	remove_from_hp_list(data, (uintptr_t)pNodePtr);
		
				//return (bot);
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
	
	//remove_from_hp_list(data, localRootPtr.AO_val2);
	//localRootPtr =  *(pNodePtr); //atomic_load_pointerNode(data, pNodePtr, casField, wrootPtrNode, popcasField, popPtrNode);
	//if(localRootPtr.AO_val1 == 0 && localRootPtr.AO_val2 == 0){
		//remove_from_hp_list(data, (uintptr_t)pNodePtr);
	//	return(bot);
	//}
	
	
	//add_to_hp_list(data, localRootPtr.AO_val2, false,0);
	
	//dataNode_t * dNodePtr = (dataNode_t *)extract_dnode_from_ptrnode(localRootPtr);
	
	 
	
	
		
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
			//	remove_from_hp_list(data, (uintptr_t)pNodePtr);
		
				//return (bot);
			}
	
	
	
	
	node_t * newLC = NULL;
	node_t * newRC = NULL;
	
	
		if((node_t *)get_child(pNodePtr->lChild) != NULL){
			//if(!is_ext(pNodePtr->lChild)){	
				newLC = in_order_traverse_delete(data, (node_t *)get_child(pNodePtr->lChild), depth+1, wRoot, casField,O);
			//}
		}	
		
		if(newLC ==bot){
			//remove_from_hp_list(data, (uintptr_t)pNodePtr);
			//remove_from_hp_list(data, localRootPtr.AO_val2);
			return (bot);
		}
		
	//data->cPtrnodes[pid].push_back(pNodePtr);
		//pointerNode_t *newRootPNode = (pointerNode_t *)get_new_pointerNode(data);
		//data->newPtrnodes[pid].push_back(newRootPNode);
		
		
		
		
		node_t * newNode = (node_t *)get_new_node(data);
		//newRootDNode->creator = 729;
		//data->newDatanodes[pid].push_back(newRootDNode);
		
		
		
		
		//data->cDatanodes[pid].push_back(dNodePtr);		
		//newRootPNode->field.AO_val1 = combine_move_and_procid(NULL, 0);
		
		//newRootPNode->field.AO_val2 = combine_dnode_and_flag(newRootDNode, 0);
		
		(newNode->opData) = combine_oprec_status_child_opdata(NULL,NOT_OWNED,LEFT, NOT_INITIAL, NOT_GUARDIAN);
			
		newNode->key = pNodePtr->key;
		
		//newRootDNode->op = operation;
	//	newRootDNode->rootCount = 99;
		newNode->color = pNodePtr->color;
		//newRootDNode->valData = dNodePtr->valData;
               // newRootDNode->parent = NULL;
              //  newNode->seq = O->seq;
		//newNode->creator = 1130;
		if(newLC == NULL){ 
			newNode->lChild = pNodePtr->lChild;
			//if(get_child_status(newNode->lChild) == ONEED ){
			//	std::cout << "ONEED_1150" << std::endl;	
			//	exit(0);
			//}	
		}
		
		else{
			newNode->lChild = create_child_word(newLC,OFIN);
			//dataNode_t * lCDNode = (dataNode_t *)extract_dnode_from_ptrnode(*(newLCP));
			newLC->parent = (newNode);
		}
		
		/*node_t * testlc = (node_t *)get_child(newNode->lChild);
		if(testlc != NULL){
		if(is_node_marked(testlc->opData)&& (casField == wRoot->opData)){
			std::cout << "CHILD is marked__1132" << std::endl;
			exit(0);
		}
		}*/
		
		if((node_t *)get_child(pNodePtr->rChild) != NULL){
			//if(!is_ext(pNodePtr->rChild)){
				newRC = in_order_traverse_delete(data, (node_t *)get_child(pNodePtr->rChild), depth+1, wRoot, casField,O);
			//}
		}
		
		if(newRC == bot){
			//remove_from_hp_list(data, (uintptr_t)pNodePtr);
			//remove_from_hp_list(data, localRootPtr.AO_val2);
			return (bot);
		}
		
		if(newRC == NULL){
			newNode->rChild = pNodePtr->rChild;
			//if(get_child_status(newNode->rChild) == ONEED ){
			//	std::cout << "ONEED_1181" << std::endl;	
			//	exit(0);
			//}	
		}
		
		else{
			newNode->rChild = create_child_word(newRC,OFIN);
			//dataNode_t * rCDNode = (dataNode_t *)extract_dnode_from_ptrnode(*(newRCP));
			newRC->parent = (newNode);
		}
		/*node_t * testrc = (node_t *)get_child(newNode->rChild);
		if(testrc != NULL){
		if(is_node_marked(testrc->opData)&& (casField == wRoot->opData)){
			std::cout << "CHILD is marked__1162" << std::endl;
			exit(0);
		}
		}*/
		//remove_from_hp_list(data, (uintptr_t)pNodePtr);
		//remove_from_hp_list(data, localRootPtr.AO_val2);
		return ((newNode));
	
	
	

}


node_t* make_delete_window_copy(thread_data_t * data, node_t * wRootChild, long key, node_t * wRoot, AO_t casField, oprec_t * O){
	//table_t *table = *(data->table);
	node_t * bot = (node_t *)map_word_to_bot_address(1);
	node_t * newLC = NULL;
	node_t * newRC = NULL;
	
	/*if(!data->cDatanodes[pid].empty()){
		

		while(!data->cDatanodes[pid].empty()){
			data->cDatanodes[pid].pop_back();
		}
	}*/
	
	// add_to_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode), false,0);
	
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
				// window has been replaced. Hazard!
			//	remove_from_hp_list(data, (uintptr_t)pNodePtr);
		
				//return (bot);
			}
		
	
	//mark_node(&wRootChild->opData);
	
	
	
if(key <= wRootChild->key){
	int state = LEAF;
	if((node_t *)get_child(wRootChild->lChild) != NULL){
		//if(!is_ext(wRootChild->lChild)){
			newLC = in_order_traverse_delete(data, (node_t *)get_child(wRootChild->lChild), 1, wRoot, casField,O);
		//	state = INTERNAL;
		//}
		//else{
		//	newLC = (node_t *)get_child(wRootChild->lChild);
		//}
	
	}
	
	if(newLC == bot){
		//remove_from_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode));
		return ((node_t *)map_word_to_bot_address(1));
	}
	
	node_t * newNode = (node_t *)get_new_node(data);
	//newRootDNode->creator = 834;
	//data->newDatanodes[pid].push_back(newRootDNode);
	
	
	//data->cDatanodes[pid].push_back(rootDNode);
		
	
		
	newNode->key = wRootChild->key;
	//newRootDNode->rootCount = 99;
	newNode->opData = combine_oprec_status_child_opdata(NULL,NOT_OWNED,LEFT, NOT_INITIAL, NOT_GUARDIAN);
	//newRootDNode->parent = NULL;
	//newNode->seq = O->seq;
	//newNode->creator = 1266;
	if(newLC != NULL){
		newNode->lChild = create_child_word(newLC,OFIN);
	}
	else{
		newNode->lChild = wRootChild->lChild;
			//if(get_child_status(newNode->lChild) == ONEED ){
			//	std::cout << "ONEED_1295" << std::endl;
			//	exit(0);
			//}
	}	
	
	/*node_t * testlc = (node_t *)get_child(newNode->lChild);
		if(testlc != NULL){
		if(is_node_marked(testlc->opData)&& (casField == wRoot->opData)){
			std::cout << "CHILD is marked__1258" << std::endl;
			exit(0);
		}
		}*/
	//newRootDNode->valData = rootDNode->valData;
	newNode->color = wRootChild->color;
	
	state = LEAF;
	if((node_t *)get_child(wRootChild->rChild) != NULL){
		//if(!is_ext(wRootChild->rChild)){
			newRC = in_order_traverse_delete(data,  (node_t *)get_child(wRootChild->rChild), DELETE_WINDOW_SIZE - 3, wRoot, casField,O);
		//	state = INTERNAL;
		//}
		//else{
		//	newRC = (node_t *)get_child(wRootChild->rChild);
		//}
	}
	
	if(newRC == bot){
		//remove_from_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode));
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
		/*node_t * testrc = (node_t *)get_child(newNode->rChild);
		if(testrc != NULL){
		if(is_node_marked(testrc->opData)&& (casField == wRoot->opData)){
			std::cout << "CHILD is marked__725" << std::endl;
			exit(0);
		}    
		} */       
        //remove_from_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode));

	return newNode;
}
else{
	int state = LEAF;
	if((node_t *)get_child(wRootChild->lChild) != NULL){
		//if(!is_ext(wRootChild->lChild)){
			newLC = in_order_traverse_delete(data, (node_t *)get_child(wRootChild->lChild), DELETE_WINDOW_SIZE - 3, wRoot, casField,O);
		//	state = INTERNAL;
		//}
		//else{
	//		newLC = (node_t *)get_child(wRootChild->lChild);
	//	}
	
	}
	
	if(newLC == bot){
		//remove_from_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode));
		return ((node_t *)map_word_to_bot_address(1));
	}
	
	node_t * newNode = (node_t *)get_new_node(data);
	//newRootDNode->creator = 877;
	//data->newDatanodes[pid].push_back(newRootDNode);
	 
	
	
	
			
	newNode->key = wRootChild->key;
	//newRootDNode->rootCount = 99;
	newNode->opData =combine_oprec_status_child_opdata(NULL,NOT_OWNED,LEFT, NOT_INITIAL, NOT_GUARDIAN);
	//newRootDNode->parent = NULL;
	//newNode->seq = O->seq;
	///newNode->creator = 1349;
	if(newLC != NULL){
		newNode->lChild = create_child_word(newLC,OFIN);
	}
	else{
		newNode->lChild = wRootChild->lChild;
			//if(get_child_status(newNode->lChild) == ONEED ){
			//	std::cout << "ONEED_1386" << std::endl;
			//	exit(0);	
			//}
	}	
	
	/*node_t * testlc = (node_t *)get_child(newNode->lChild);
		if(testlc != NULL){
		if(is_node_marked(testlc->opData)&& (casField == wRoot->opData)){
			std::cout << "CHILD is marked__1338" << std::endl;
			exit(0);
		}
		}*/
	//newRootDNode->valData = rootDNode->valData;
	newNode->color = wRootChild->color;
	
	state = LEAF;
	if((node_t *)get_child(wRootChild->rChild) != NULL){
		//if(!is_ext(wRootChild->rChild)){
			newRC = in_order_traverse_delete(data, (node_t *)get_child(wRootChild->rChild), 1, wRoot, casField,O);
		//	state = INTERNAL;
		//}
		//else{
		//	newRC = (node_t *)get_child(wRootChild->rChild);
		//}
	}
	
	if(newRC == bot){
		//remove_from_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode));
		return ((node_t *)map_word_to_bot_address(1));
	}
	
	if(newRC != NULL){
		newNode->rChild = create_child_word(newRC,OFIN);
	}
	else{
		newNode->rChild = wRootChild->rChild;
			//if(get_child_status(newNode->rChild) == ONEED ){
			//	std::cout << "ONEED_1423" << std::endl;
			//	exit(0);	
			//}
	}

			            
       // remove_from_hp_list(data, map_datanode_address_to_hplist_entry(rootDNode));
	/*node_t * testrc = (node_t *)get_child(newNode->rChild);
		if(testrc != NULL){
		if(is_node_marked(testrc->opData)&& (casField == wRoot->opData)){
			std::cout << "CHILD is marked__1370" << std::endl;
			exit(0);
		}
		}*/
	return newNode;
}
	
	
	
}

