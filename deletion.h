#include <iostream>

node_t * check_delete_case_2(thread_data_t * data, node_t * wRootChildCopy, long key, oprec_t * O){
	node_t * curNode = wRootChildCopy;

	node_t * nextNode = NULL;
	if(key <= curNode->key){
		nextNode = (node_t *)get_child(curNode->lChild);
	}
	else{
		nextNode = (node_t *)get_child(curNode->rChild);
	}

	//dataNode_t * nextDNode = (dataNode_t *)extract_dnode_from_ptrnode(*(nextNode));
	
	if(is_external_node(nextNode)){
		// external node encountered... return
	//std::cout << "a" << std::endl;
		return(NULL);
		//return(nextNode);
	}
	
	#ifdef DEBUG
	if(nextNode->seq != O->seq){
		std::cout << "Diff_25d" << std::endl;	
	}
	#endif
	int depth = 1; 
	
	
	
	while(depth <= 2){
		//nextNode = (dataNode_t *)extract_dnode_from_ptrnode(*(nextNode));
		if(nextNode->color == BLACK){
			// Current node is black. Check if it has a red child or a red grandchild	
			
			if(is_external_node(nextNode)){
				// external node encountered... return
				//std::cout << "b" << std::endl;
				return(NULL);
			}
			// find next node on access path
			node_t * lchildNode = NULL;
			//node_t * lchildDataNode = NULL;
			node_t * rchildNode = NULL;
			//node_t * rchildDataNode = NULL;
			
			
			node_t * lgchildNode = NULL;
			//dataNode_t * lgchildDataNode = NULL;
			node_t * rgchildNode = NULL;
			//dataNode_t * rgchildDataNode = NULL;	
			
			lchildNode = (node_t *)get_child(nextNode->lChild);
			
			if(lchildNode != NULL){
				#ifdef DEBUG
				//lchildDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(lchildPtrNode));
				if(lchildNode->seq != O->seq && !is_external_node(lchildNode)){
					std::cout << "Diff_60d" << std::endl;
				}
				#endif
				if(lchildNode->color == RED){
					/// Invariant satisfied. Lock nextNode
					nextNode->opData = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,NOT_GUARDIAN);
					nextNode->markedRoot = get_parent_of_window_root(data, nextNode, wRootChildCopy);
					return(nextNode);
				}
			
				else{
					lgchildNode = (node_t *)get_child(lchildNode->lChild);
					if(lgchildNode != NULL){
						#ifdef DEBUG
						if(lgchildNode->seq != O->seq && !is_external_node(lgchildNode)){
							std::cout << "Diff_76d" << std::endl;
				
						}
						#endif
						if(lgchildNode->color == RED){
							
							// set status of lchildPtrNode and nextNode to owned
							(lchildNode->opData) = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,NOT_GUARDIAN);
							nextNode->opData = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,NOT_GUARDIAN);
							node_t * guardian = get_parent_of_window_root(data, nextNode, wRootChildCopy);
							
							lchildNode->markedRoot = guardian;
							nextNode->markedRoot = guardian;
							
							return nextNode;
						}
					}
				
					rgchildNode = (node_t *)get_child(lchildNode->rChild);
					if(rgchildNode != NULL){
						//rgchildDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(rgchildPtrNode));
						if(rgchildNode->color == RED){
							#ifdef DEBUG
							if(rgchildNode->seq != O->seq && !is_external_node(rgchildNode))
								std::cout << "Diff_100d" << std::endl;
							#endif
							// set status of lchildPtrNode and rgchildPtrNode to SLOCK
							(lchildNode->opData) = combine_oprec_status_child_opdata(O,OWNED,RIGHT,NOT_INITIAL,NOT_GUARDIAN);
							nextNode->opData = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,NOT_GUARDIAN);
							node_t * guardian = get_parent_of_window_root(data, nextNode, wRootChildCopy);
							
							lchildNode->markedRoot = guardian;
							nextNode->markedRoot = guardian;
							return nextNode;
						}
					}
							
				}
			}	
			
			rchildNode = (node_t *)get_child(nextNode->rChild);
			if(rchildNode != NULL){
				#ifdef DEBUG
				if(rchildNode->seq != O->seq && !is_external_node(rchildNode))	
					std::cout << "Diff_120d" << std::endl;
				#endif
				if(rchildNode->color == RED){
					
					nextNode->opData = combine_oprec_status_child_opdata(O,OWNED,RIGHT,NOT_INITIAL,NOT_GUARDIAN);
					node_t * guardian = get_parent_of_window_root(data, nextNode, wRootChildCopy);
					nextNode->markedRoot = guardian;
					return(nextNode);
				}
			
				else{
					lgchildNode = (node_t *)get_child(rchildNode->lChild);
					if(lgchildNode != NULL){
						#ifdef DEBUG
						if(lgchildNode->seq != O->seq && !is_external_node(lgchildNode))
								std::cout << "Diff_136d" << std::endl;
						#endif	
						//lgchildDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(lgchildPtrNode));
						if(lgchildNode->color == RED){
							
							(rchildNode->opData) = combine_oprec_status_child_opdata(O,OWNED,LEFT,NOT_INITIAL,NOT_GUARDIAN);
							nextNode->opData = combine_oprec_status_child_opdata(O,OWNED,RIGHT,NOT_INITIAL,NOT_GUARDIAN);
							node_t * guardian = get_parent_of_window_root(data, nextNode, wRootChildCopy);
							
							rchildNode->markedRoot = guardian;
							nextNode->markedRoot = guardian;
							return nextNode;
						}
					}
				
					rgchildNode = (node_t *)get_child(rchildNode->rChild);
					if(rgchildNode != NULL){
							#ifdef DEBUG
							if(rgchildNode->seq != O->seq && !is_external_node(rgchildNode))
								std::cout << "Diff_156d" << std::endl;
							#endif
						
						if(rgchildNode->color == RED){
							
							(rchildNode->opData) = combine_oprec_status_child_opdata(O,OWNED,RIGHT,NOT_INITIAL,NOT_GUARDIAN);
							nextNode->opData = combine_oprec_status_child_opdata(O,OWNED,RIGHT,NOT_INITIAL,NOT_GUARDIAN);
							node_t * guardian = get_parent_of_window_root(data, nextNode, wRootChildCopy);
							
							rchildNode->markedRoot = guardian;
							nextNode->markedRoot = guardian;
							return nextNode;
						}
					}
							
				}
			}
			depth++;
			if(key <= nextNode->key){
				nextNode = (node_t *)get_child(nextNode->lChild);
			}
			else{
				nextNode = (node_t *)get_child(nextNode->rChild);
			}
		}
		
		else{
			if(is_external_node(nextNode)){
				// external node encountered... return. 
				//std::cout << "h" << std::endl;
				return(NULL);
			}
			// current node is red. Return
			//std::cout << "i" << std::endl;
			return(nextNode);	
			
		}
	}
	
	return NULL;	
}

void * balance_shortness(thread_data_t * data, node_t * nextWindowRootNode ,node_t* curNode, node_t * wRoot, node_t * wRootChildCopy, node_t * wRootChild, AO_t casField, oprec_t * O){
		
		//std::cout << "cbs" << std::endl;
		// root of window can never be short	
		
		node_t * cptr = NULL;
		
		//dataNode_t * cdata = NULL;
		
		Word nextWord; 
			
		//if(curPtrNode != NULL){

	
		// Current node is short
	
		node_t * parentNode = NULL;
		//dataNode_t * parentDataNode = NULL;
		
		//dataNode_t * curDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(curPtrNode));
		
		
		
				
		if(curNode->parent == NULL){
			// Current node is child of window root
			parentNode = wRootChildCopy;
		}
		else{
			parentNode = curNode->parent;			
			//parentDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(parentPtrNode));
			
			
		}
		#ifdef DEBUG	
		if(parentNode->seq != O->seq){
			std::cout << "diff_Seq_236" << std::endl;
		}
		#endif
			node_t * siblingNode;
			//dataNode_t * siblingDataNode;
			
			if((node_t *)get_child(parentNode->lChild) == curNode){
				siblingNode = (node_t *)get_child(parentNode->rChild);
			}
			else if((node_t *)get_child(parentNode->rChild) == curNode){
				siblingNode = (node_t *)get_child(parentNode->lChild);
			}
			#ifdef DEBUG
			else{
				std::cout << "Error. Deletion.h 215" << std::endl;
				exit(0);
			}
			
			
			//if(siblingPtrNode != NULL){
			//	siblingDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(siblingPtrNode));
			//}
			if(siblingNode->seq != O->seq && !is_external_node(siblingNode))
				std::cout << "diff_Seq_256" << std::endl;
			#endif

		if(parentNode->color == RED){
			// Possible cases are 5d, 5e and 5f

			// 1. Case 5.d
			
			
			//node_t * sLChildNode = NULL;
			//dataNode_t * sLChildDataNode = NULL ;
	
			//node_t * sRChildNode = NULL;
			//dataNode_t * sRChildDataNode = NULL ;		
	
			//if((node_t *)get_child(siblingNode->lChild) != NULL){
				node_t * sLChildNode = (node_t *)get_child(siblingNode->lChild);
			//	sLChildDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(sLChildPtrNode));
			//}

			//if(siblingDataNode->rChild != NULL){
				node_t * sRChildNode = (node_t *)get_child(siblingNode->rChild);
			//	sRChildDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(sRChildPtrNode));
			//}
			
			// If both children of sibling are black, then case 5 d is satisfied.		
			#ifdef DEBUG
			if(sLChildNode->seq != O->seq && !is_external_node(sLChildNode))
				std::cout << "diff_Seq_282" << std::endl;
			if(sRChildNode->seq != O->seq && !is_external_node(sRChildNode))
				std::cout << "diff_Seq_284" << std::endl;
			#endif	
			if(sLChildNode->color == BLACK && sRChildNode->color == BLACK){
				parentNode->color = BLACK;
				siblingNode->color = RED;
				
								
				
		
			//if(nextWindowRootPtrNode != NULL){
		 	//	nextWindowRootPtrNode->field.AO_val1 = combine_move_and_procid(NULL,pid);
		 	//}
		 		// update opData field of nextWindowRootDataNode
		 		
		 		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);
		 			
					
					//wRootDataNode->opData = O;
					//wRootDataNode->wop2 = 4270;
					//wRootDataNode->rootCount = rootCount;	
					
					// update next field of wRootDataNode
				#ifdef DEBUG
					wRootChildCopy->mover = 267;
				#endif
					wRootChildCopy->move = move;
					
				 	//AO_double_t localRootPtr = (casField);
				 	//AO_double_t newRootPtrNode;
				 	//newRootPtrNode.AO_val1 = combine_move_and_procid(nextWindowRootPtrNode, pid);
			//newRootPtrNode.AO_val2 = combine_dnode_and_flag(wRootDataNode, extract_flag_from_ptrNode_field2(localRootPtr.AO_val2));

			
			int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,294);
			
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode,DONE));
		
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
						//free_window_on_dcas_failure(data,pid);
						//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
				
				
				
				
				return NULL;
			}

			else{
				
						// Either case 5e or 5f
				if(siblingNode == (node_t *)get_child(parentNode->rChild)){
					node_t * sibrChildNode = (node_t *)get_child(siblingNode->rChild);
					node_t * siblChildNode = (node_t *)get_child(siblingNode->lChild);
					//dataNode_t * sibrChildDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(sibrChildPtrNode));
		
						//dataNode_t * siblChildDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(siblChildPtrNode));
					#ifdef DEBUG
					if(siblChildNode->seq != O->seq && !is_external_node(siblChildNode))
							std::cout << "diff_Seq_353" << std::endl;
					if(sibrChildNode->seq != O->seq && !is_external_node(sibrChildNode))
							std::cout << "diff_Seq_355" << std::endl;
					#endif
					if(sibrChildNode->color == RED){
						
						//pointerNode_t * newParentPtrNode = (pointerNode_t *)get_new_pointerNode(data);
						
						
			
					
						
		//newParentPtrNode->field.AO_val1 = combine_move_and_procid(NULL, 0);
		//newParentPtrNode->field.AO_val2 = combine_dnode_and_flag(parentDataNode, 0);		
						//*(newParentPtrNode) = combine_dnode_status_ptrnode(parentDataNode, FREE);		
						// TODO: Set other parameters of parent ptr node.
						parentNode->rChild = siblingNode->lChild;
						siblingNode->color = parentNode->color;
						parentNode->color = BLACK;
						siblingNode->lChild = create_child_word(parentNode,OFIN);
						sibrChildNode->color = BLACK;
										
						if(parentNode == wRootChildCopy){
						
					
		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);
		
		 	
					//siblingDataNode->opData = O;
					
					//	wRootDataNode->opData = NULL;
					#ifdef DEBUG
						siblingNode->mover = 339;
					#endif
						siblingNode->move = move;
					//siblingDataNode->wop2 = 4351;
					//siblingDataNode->rootCount = rootCount;	
				 	//AO_double_t localRootPtr = (casField);
				 	//AO_t newRootPtrNode;
				 	
				 	//newRootPtrNode.AO_val1 = combine_move_and_procid(nextWindowRootPtrNode, pid);
			//newRootPtrNode.AO_val2 = combine_dnode_and_flag(siblingDataNode, extract_flag_from_ptrNode_field2(localRootPtr.AO_val2));
			//newRootPtrNode.AO_val2 = combine_dnode_and_flag();
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(siblingDataNode,DONE));
		
		int result0 = complete_cas(data, casField, wRoot, wRootChild, siblingNode,O,369);
		
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
						//free_window_on_dcas_failure(data,pid);
						//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
						
								
							return NULL;			
		}
		else{
			//parentPtrNode->field.AO_val2 = combine_dnode_and_flag(siblingDataNode, extract_flag_from_ptrNode_field2(parentPtrNode->field.AO_val2));
			//*(parentPtrNode) = combine_dnode_status_ptrnode(siblingDataNode, FREE);
			//parentPtrNode = siblingNode;		
			replace_node(data,parentNode,siblingNode,wRootChildCopy);
		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);
		/*if(nextWindowRootNode != NULL){
			//nextWindowRootPtrNode->field.AO_val1 = combine_move_and_procid(NULL,pid);
			dataNode_t * nextDNode = (dataNode_t *)extract_dnode_from_ptrnode(*(nextWindowRootPtrNode));
			nextDNode->opData = O;
			*(nextWindowRootPtrNode) = combine_dnode_status_ptrnode(nextDNode,PLOCK);
		 	nextWord = (uintptr_t)nextWindowRootPtrNode;
		 	
		 	
		}
		else{
			nextWord = NULL;
		}*/
					//wRootDataNode->op = -1;
					//wRootDataNode->opData = O;
					//wRootDataNode->wop2 = 4398;
					//wRootDataNode->rootCount = rootCount;
				 	//AO_double_t localRootPtr = (casField);
				 	//AO_double_t newRootPtrNode;
					#ifdef DEBUG
				 	wRootChildCopy->mover = 398;
					#endif
				 	wRootChildCopy->move = move;
				 	//newRootPtrNode.AO_val1 = combine_move_and_procid(nextWindowRootPtrNode, pid);
			//newRootPtrNode.AO_val2 = combine_dnode_and_flag(wRootDataNode, extract_flag_from_ptrNode_field2(localRootPtr.AO_val2));
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode,DONE));
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,421);
		
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
						//free_window_on_dcas_failure(data,pid);
						//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
							
														
							return NULL;				
		}
							
							
					}
				else if(siblChildNode->color == RED){
						//	pointerNode_t * newParentPtrNode = (pointerNode_t *)get_new_pointerNode(data);
							
			
					
						//newParentPtrNode->field.AO_val1 = combine_move_and_procid(NULL, 0);
						//newParentPtrNode->field.AO_val2 = combine_dnode_and_flag(parentDataNode, 0);
							//	*(newParentPtrNode) = combine_dnode_status_ptrnode(parentDataNode, FREE);			
							// TODO: Set other parameters of parent ptr node.
							parentNode->rChild = siblChildNode->lChild;
							siblingNode->lChild = siblChildNode->rChild;
							siblChildNode->lChild = create_child_word(parentNode,OFIN);
							siblChildNode->rChild = create_child_word(siblingNode,OFIN);
							siblChildNode->color = parentNode->color;
							parentNode->color = BLACK;

	if(parentNode == wRootChildCopy){
							
		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);			
		/*if(nextWindowRootNode != NULL){
		 	//nextWindowRootPtrNode->field.AO_val1 = combine_move_and_procid(NULL,pid);
		 	dataNode_t * nextDNode = (dataNode_t *)extract_dnode_from_ptrnode(*(nextWindowRootPtrNode));
		 	nextDNode->opData = O;
		 	*(nextWindowRootPtrNode) = combine_dnode_status_ptrnode(nextDNode,PLOCK);
		 	nextWord = (uintptr_t)nextWindowRootPtrNode;
		 }
		 else{
		 	nextWord = NULL;
		 }*/
		
			//siblChildDataNode->opData = O;
					
			//wRootDataNode->opData = NULL;
					#ifdef DEBUG
			siblChildNode->mover = 462;
			#endif		
			siblChildNode->move = move;
					//siblChildDataNode->wop2 = 4472;
					//siblChildDataNode->rootCount = rootCount;
				 	//AO_double_t localRootPtr = (casField);
				 	//AO_double_t newRootPtrNode;
				 	
				 	//newRootPtrNode.AO_val1 = combine_move_and_procid(nextWindowRootPtrNode, pid);
			//newRootPtrNode.AO_val2 = combine_dnode_and_flag(siblChildDataNode, extract_flag_from_ptrNode_field2(localRootPtr.AO_val2));
		//int result = atomic_cas_full1(data,wRootPtrNode,casField, combine_dnode_status_ptrnode(siblChildDataNode,DONE));
		
		int result0 = complete_cas(data, casField, wRoot, wRootChild, siblChildNode, O,491);
		
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
					//free_window_on_dcas_failure(data,pid);
					//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
								
								
								return NULL;
		}
		else{
			//parentPtrNode->field.AO_val2 = combine_dnode_and_flag(siblChildDataNode, extract_flag_from_ptrNode_field2(parentPtrNode->field.AO_val2));
			//*(parentPtrNode) = combine_dnode_status_ptrnode(siblChildDataNode,FREE);					
			replace_node(data,parentNode,siblChildNode,wRootChildCopy);				
		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);
		
					//wRootDataNode->opData = O;
					//wRootDataNode->wop2 = 4517;
				//	wRootDataNode->rootCount = rootCount;
				#ifdef DEBUG
					wRootChildCopy->mover = 505;
				#endif
					wRootChildCopy->move = move;
				 	//AO_double_t localRootPtr = (casField);
				 	//AO_double_t newRootPtrNode;
				 	//newRootPtrNode.AO_val1 = combine_move_and_procid(nextWindowRootPtrNode, pid);
			//newRootPtrNode.AO_val2 = combine_dnode_and_flag(wRootDataNode, extract_flag_from_ptrNode_field2(localRootPtr.AO_val2));
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode,DONE));
		
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,531);
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
					//free_window_on_dcas_failure(data,pid);
					//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
								
											
								return NULL;
							}
	
						}
						
						
						
		}
					else if(siblingNode == (node_t *)get_child(parentNode->lChild)){
			
						node_t * sibrChildNode = (node_t *)get_child(siblingNode->rChild);
						node_t * siblChildNode = (node_t *)get_child(siblingNode->lChild);
					#ifdef DEBUG
					
					if(sibrChildNode->seq != O->seq && !is_external_node(sibrChildNode))
						std::cout << "Diff_seq_592" << std::endl;
					if(siblChildNode->seq != O->seq && !is_external_node(siblChildNode))
						std::cout << "Diff_seq_594" << std::endl;
					#endif
					if(siblChildNode->color == RED){
							//pointerNode_t * newParentPtrNode = (pointerNode_t *)get_new_pointerNode(data);
							
							
			
					
							
	//newParentPtrNode->field.AO_val1 = combine_move_and_procid(NULL, 0);						
	//newParentPtrNode->field.AO_val2 = combine_dnode_and_flag(parentDataNode, 0);			
							//*(newParentPtrNode) = combine_dnode_status_ptrnode(parentDataNode, FREE);
							parentNode->lChild = siblingNode->rChild;
							siblingNode->color = parentNode->color;
							parentNode->color = BLACK;
							siblingNode->rChild = create_child_word(parentNode,OFIN);
							siblChildNode->color = BLACK;				
							if(parentNode == wRootChildCopy){
							
					node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);		
						
		
		//			siblingDataNode->opData = O;
					
		//			wRootDataNode->opData = NULL;
					//siblingDataNode->wop2 = 4601;
					#ifdef DEBUG
					siblingNode->mover = 571;
					#endif
					siblingNode->move = move;
				 	//AO_double_t localRootPtr = (casField);
				 	//AO_double_t newRootPtrNode;
				 	//newRootPtrNode.AO_val1 = combine_move_and_procid(nextWindowRootPtrNode, pid);
			//newRootPtrNode.AO_val2 = combine_dnode_and_flag(siblingDataNode, extract_flag_from_ptrNode_field2(localRootPtr.AO_val2));
			int result0 = complete_cas(data, casField, wRoot, wRootChild, siblingNode, O,595);
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(siblingDataNode,DONE));
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
					
					//free_window_on_dcas_failure(data,pid);
					//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
								
								return NULL;				
							}
							else{
			//parentPtrNode->field.AO_val2 = combine_dnode_and_flag(siblingDataNode, extract_flag_from_ptrNode_field2(parentPtrNode->field.AO_val2));
			//*(parentPtrNode) = combine_dnode_status_ptrnode(siblingDataNode, FREE);		
		
		replace_node(data,parentNode,siblingNode,wRootChildCopy);				
		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);
		
				//wRootDataNode->opData = O;
					//wRootDataNode->wop2 = 4644;
					#ifdef DEBUG
					wRootChildCopy->mover = 609;
					#endif
					wRootChildCopy->move = move;
				 //	AO_double_t localRootPtr = (casField);
				 //	AO_double_t newRootPtrNode;
				 //	newRootPtrNode.AO_val1 = combine_move_and_procid(nextWindowRootPtrNode, pid);
			//newRootPtrNode.AO_val2 = combine_dnode_and_flag(wRootDataNode, extract_flag_from_ptrNode_field2(localRootPtr.AO_val2));
		//int result = atomic_cas_full1(data,wRootPtrNode,casField, combine_dnode_status_ptrnode(wRootDataNode, DONE));
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,634);
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
						
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
					
				//	free_window_on_dcas_failure(data,pid);
				//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
								
								
								
								return NULL;				
							}
						}
			else if(sibrChildNode->color == RED){
				//node_t * newParentPtrNode = (pointerNode_t *)get_new_pointerNode(data);
						
			
			//			*(newParentPtrNode) = combine_dnode_status_ptrnode(parentDataNode,FREE);			
	//newParentPtrNode->field.AO_val1 = combine_move_and_procid(NULL, 0);						
	//newParentPtrNode->field.AO_val2 = combine_dnode_and_flag(parentDataNode, 0);			
							parentNode->lChild = sibrChildNode->rChild;
							siblingNode->rChild = sibrChildNode->lChild;
							sibrChildNode->rChild = create_child_word(parentNode,OFIN);
							sibrChildNode->lChild = create_child_word(siblingNode,OFIN);
							sibrChildNode->color = parentNode->color;
							parentNode->color = BLACK;

							if(parentNode == wRootChildCopy){
							
							
		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);
		
					//sibrChildDataNode->opData = O;
					
					//wRootDataNode->opData = NULL;
					//sibrChildDataNode->wop2 = 4717;
					#ifdef DEBUG
					sibrChildNode->mover = 665;
					#endif
					sibrChildNode->move = move;
				 	//AO_double_t localRootPtr = (casField);
				 	//AO_double_t newRootPtrNode;
				 	//newRootPtrNode.AO_val1 = combine_move_and_procid(nextWindowRootPtrNode, pid);
			///newRootPtrNode.AO_val2 = combine_dnode_and_flag(sibrChildDataNode, extract_flag_from_ptrNode_field2(localRootPtr.AO_val2));
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(sibrChildDataNode,DONE));
		int result0 = complete_cas(data, casField, wRoot, wRootChild, sibrChildNode, O,690);
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
	//	flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
					
					//free_window_on_dcas_failure(data,pid);
					//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
								
								return NULL;		
							}
							else{
			//parentPtrNode->field.AO_val2 = combine_dnode_and_flag(sibrChildDataNode, extract_flag_from_ptrNode_field2(parentPtrNode->field.AO_val2));	
				//*(parentPtrNode) = combine_dnode_status_ptrnode(sibrChildDataNode, FREE);					
					replace_node(data,parentNode,sibrChildNode,wRootChildCopy);				
		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);		
		
					//wRootDataNode->opData = O;
					//wRootDataNode->wop2 = 4759;
					#ifdef DEBUG
					wRootChildCopy->mover = 702;
					#endif
					wRootChildCopy->move = move;
				 	//AO_double_t localRootPtr = (casField);
				 	//AO_double_t newRootPtrNode;
				 	//newRootPtrNode.AO_val1 = combine_move_and_procid(nextWindowRootPtrNode, pid);
			//newRootPtrNode.AO_val2 = combine_dnode_and_flag(wRootDataNode, extract_flag_from_ptrNode_field2(localRootPtr.AO_val2));
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode,DONE));
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,727);
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
					//free_window_on_dcas_failure(data,pid);
					//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
								
												
								return NULL;
							}
	
						}
						else{
							std::cout << "Both children of sibling are black. We should not be here_1" << std::endl;
							exit(0);
						}		

					}
					else{
						std::cout << "pointers incorrectly assigned. Balance shortness function. Location 6" << std::endl;
						exit(0);
					}

			}
			
		}
		else{
			// Parent is Black. Possible cases are 5b, 5c, 5e and 5f

			//1. Check Case 5.b
			if(siblingNode->color == BLACK){ 
				// Cases 5b, 5e and 5f
				node_t * sLChildNode = (node_t *)get_child(siblingNode->lChild);
				//dataNode_t * sLChildDataNode = NULL ;
	
				node_t * sRChildNode = (node_t *)get_child(siblingNode->rChild);
				//dataNode_t * sRChildDataNode = NULL ;		
					#ifdef DEBUG
				
					if(sLChildNode->seq != O->seq && !is_external_node(sLChildNode))
						std::cout << "Diff_seq_809" << std::endl;
					if(sRChildNode->seq != O->seq && !is_external_node(sRChildNode))
						std::cout << "Diff_seq_811" << std::endl;
				#endif
			
				// If both children of sibling are black, then case 5 b is satisfied.		
			
				if(sLChildNode->color == BLACK && sRChildNode->color == BLACK){
					parentNode->color = BLACK;
					siblingNode->color = RED;
					
					// Parent is short. Rebalance
					
					if(parentNode == wRootChildCopy){
						// parent is root of window
								
					node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);
								
						//wRootDataNode->opData = O;
						//wRootDataNode->wop2 = 4847;
					#ifdef DEBUG
						wRootChildCopy->mover = 776;
					#endif
						wRootChildCopy->move = move;
			int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,796);
		//int result = atomic_cas_full1(data,wRootPtrNode,casField, combine_dnode_status_ptrnode(wRootDataNode, DONE));
			if(result0 == 1){						
				// cas success
				if(data->numDelete != -1){
					data->numActualDelete++;
					data->numDelete = -1;
				}
			//flush_window_on_dcas_success(data,pid);
				return NULL;
			
			} 
			else{
				// cas failure. Some other node executed window
				//free_window_on_dcas_failure(data,pid);
				//data->fdelCas++;
				data->numDelete = -1;
				return NULL;
			}
		}
				balance_shortness(data, nextWindowRootNode, parentNode, wRoot, wRootChildCopy, wRootChild, casField, O);
					
				}
				else{
					// Cases 5e and 5f
					if(siblingNode == (node_t *)get_child(parentNode->rChild)){
						node_t * sibrChildNode = (node_t *)get_child(siblingNode->rChild);
						node_t * siblChildNode = (node_t *)get_child(siblingNode->lChild);
						
					#ifdef DEBUG
						
					if(siblChildNode->seq != O->seq && !is_external_node(siblChildNode))
						std::cout << "Diff_seq_862" << std::endl;
					if(siblChildNode->seq != O->seq && !is_external_node(siblChildNode))
						std::cout << "Diff_seq_864" << std::endl;
					#endif

					if(sibrChildNode->color == RED){
					//pointerNode_t * newParentPtrNode = (pointerNode_t *)get_new_pointerNode(data);
							
							
							
							parentNode->rChild = siblingNode->lChild;
							siblingNode->color = parentNode->color;
							parentNode->color = BLACK;
							siblingNode->lChild = create_child_word(parentNode,OFIN);
							sibrChildNode->color = BLACK;				
				if(parentNode == wRootChildCopy){
						
								
		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);
								
						//wRootDataNode->opData = O;
						//wRootDataNode->wop2 = 4847;
					#ifdef DEBUG
					siblingNode->mover = 825;
					#endif
					siblingNode->move = move;
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(siblingDataNode, DONE));
		int result0 = complete_cas(data, casField, wRoot, wRootChild, siblingNode, O,846);
		if(result0 == 1){						
			// cas success
			if(data->numDelete != -1){
				data->numActualDelete++;
				data->numDelete = -1;
			}
			//flush_window_on_dcas_success(data,pid);
			return NULL;
		} 
		else{	
			// cas failure. Some other node executed window
			//free_window_on_dcas_failure(data,pid);
			//data->fdelCas++;
			data->numDelete = -1;
			return NULL;
		}
						
	return NULL;			
	}
	else{
						
			replace_node(data,parentNode,siblingNode,wRootChildCopy);				
						
	//*(parentPtrNode) = combine_dnode_status_ptrnode(siblingDataNode, FREE);							
		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);
					
		
					//wRootDataNode->opData = O;
					//wRootDataNode->wop2 = 4351;
					#ifdef DEBUG
					wRootChildCopy->mover = 858;
					#endif
					wRootChildCopy->move = move;
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode, DONE));
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,879);
		if(result0 == 1){						
			// cas success
			if(data->numDelete != -1){
				data->numActualDelete++;
				data->numDelete = -1;
			}
			//flush_window_on_dcas_success(data,pid);
			return NULL;
		} 
		else{
			// cas failure. Some other node executed window
			//	free_window_on_dcas_failure(data,pid);
			//data->fdelCas++;
			data->numDelete = -1;
			return NULL;
		}
								
								
									
			return NULL;			
			}
		}
		else if(siblChildNode->color == RED){
			//pointerNode_t * newParentPtrNode = (pointerNode_t *)get_new_pointerNode(data);
							
			//*(newParentPtrNode) = combine_dnode_status_ptrnode(parentDataNode, FREE);					
			parentNode->rChild = siblChildNode->lChild;
			siblingNode->lChild = siblChildNode->rChild;
			siblChildNode->lChild = create_child_word(parentNode,OFIN);
			siblChildNode->rChild = create_child_word(siblingNode,OFIN);
			siblChildNode->color = parentNode->color;
			parentNode->color = BLACK;

		if(parentNode == wRootChildCopy){
						
				node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);				
					#ifdef DEBUG
					siblChildNode->mover = 898;
					#endif
					siblChildNode->move = move;
		int result0 = complete_cas(data, casField, wRoot, wRootChild, siblChildNode, O,918);
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(siblChildDataNode, DONE));
		
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
					
					//free_window_on_dcas_failure(data,pid);
					//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
								
								return NULL;		
			}
		else{
		//parentPtrNode->field.AO_val2 = combine_dnode_and_flag(siblChildDataNode, extract_flag_from_ptrNode_field2(parentPtrNode->field.AO_val2));		
		//*(parentPtrNode) = combine_dnode_status_ptrnode(siblChildDataNode, FREE);						
		
		replace_node(data,parentNode,siblChildNode,wRootChildCopy);
		
		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);
		
					//wRootDataNode->opData = O;
					//wRootDataNode->wop2 = 41081;
					#ifdef DEBUG
					wRootChildCopy->mover = 934;
					#endif	
					wRootChildCopy->move = move;

		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode, DONE));
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,956);
		
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
					//free_window_on_dcas_failure(data,pid);
					//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
								
								
								return NULL;			
							}
	
						}
						else{
							std::cout << "Both children of sibling are black. We should not be here_2" << std::endl;
							exit(0);
						}	
						
						
					}
					else if(siblingNode == (node_t *)get_child(parentNode->lChild)){
			
						node_t * sibrChildNode = (node_t *)get_child(siblingNode->rChild);
						node_t * siblChildNode = (node_t *)get_child(siblingNode->lChild);
						
					#ifdef DEBUG
					if(siblChildNode->seq != O->seq && !is_external_node(siblChildNode))
					std::cout << "Diff_seq_1035" << std::endl;
					if(sibrChildNode->seq != O->seq && !is_external_node(sibrChildNode))
					std::cout << "Diff_seq_1037" << std::endl;
					#endif
					//dataNode_t * sibrChildDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(sibrChildPtrNode));
					//dataNode_t * siblChildDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(siblChildPtrNode));
					if(siblChildNode->color == RED){
					//pointerNode_t * newParentPtrNode = (pointerNode_t *)get_new_pointerNode(data);
					//*(newParentPtrNode) = combine_dnode_status_ptrnode(parentDataNode, FREE);
		
							parentNode->lChild = siblingNode->rChild;
							siblingNode->color = parentNode->color;
							parentNode->color = BLACK;
							siblingNode->rChild = create_child_word(parentNode,OFIN);
							siblChildNode->color = BLACK;				
					if(parentNode == wRootChildCopy){
							
								
		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);
					#ifdef DEBUG
		
					siblingNode->mover = 991;
					#endif
					siblingNode->move = move;
		
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(siblingDataNode, DONE));
		int result0 = complete_cas(data, casField, wRoot, wRootChild, siblingNode, O,1013);
		
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
					//free_window_on_dcas_failure(data,pid);
					//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
								
								return NULL;	
							}
							else{
//parentPtrNode->field.AO_val2 = combine_dnode_and_flag(siblingDataNode, extract_flag_from_ptrNode_field2(parentPtrNode->field.AO_val2));
			//*(parentPtrNode) = combine_dnode_status_ptrnode(siblingDataNode, FREE);								
		replace_node(data,parentNode,siblingNode,wRootChildCopy);
		
		node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);		
					#ifdef DEBUG
					wRootChildCopy->mover = 1023;
					#endif
					wRootChildCopy->move = move;
			//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode, DONE));
			int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,1044);
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
				//	free_window_on_dcas_failure(data,pid);
				//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
								
								
											
								return NULL;
							}
						}
						else if(sibrChildNode->color == RED){
				//			pointerNode_t * newParentPtrNode = (pointerNode_t *)get_new_pointerNode(data);
			//*(newParentPtrNode) = combine_dnode_status_ptrnode(parentDataNode, FREE);
			
							parentNode->lChild = sibrChildNode->rChild;
							siblingNode->rChild = sibrChildNode->lChild;
							sibrChildNode->rChild = create_child_word(parentNode,OFIN);
							sibrChildNode->lChild = create_child_word(siblingNode,OFIN);
							sibrChildNode->color = parentNode->color;
							parentNode->color = BLACK;

			if(parentNode == wRootChildCopy){
							
								
			node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);
					#ifdef DEBUG
					sibrChildNode->mover = 1065;
					#endif
					sibrChildNode->move = move;
		
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(sibrChildDataNode,DONE));
		int result0 = complete_cas(data, casField, wRoot, wRootChild, sibrChildNode, O,1087);
		
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
					//free_window_on_dcas_failure(data,pid);
					//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
								
								return NULL;			
							}
			else{
		//*(parentPtrNode) = combine_dnode_status_ptrnode(sibrChildDataNode, FREE);						
		replace_node(data,parentNode,sibrChildNode,wRootChildCopy);
			node_t * move = lock_new_ipath(data, nextWindowRootNode, wRootChildCopy,O);					
					#ifdef DEBUG
			
				wRootChildCopy->mover = 1096;
					#endif
					wRootChildCopy->move = move;
	
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode, DONE));
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,1118);
					if(result0 == 1){						
						// cas success
						if(data->numDelete != -1){
							data->numActualDelete++;
							data->numDelete = -1;
						}
		//flush_window_on_dcas_success(data,pid);
						return NULL;
		
					} 
					else{
						// cas failure. Some other node executed window
					//free_window_on_dcas_failure(data,pid);
					//data->fdelCas++;
						data->numDelete = -1;
						return NULL;
					}
								
											
								return NULL;
							}
	
						}
					
					#ifdef DEBUG
						else{
							std::cout << "Both children of sibling are black. We should not be here_3" << std::endl;
							exit(0);
						}
					#endif		
	
					}
				
					#ifdef DEBUG
					else{
						std::cout << "pointers incorrectly assigned. Balance shortness function. Location 6" << std::endl;
						exit(0);
					}
					#endif	
					
				}
			}
			else{
				// sibling is RED. Case 5c.
				if((node_t *)get_child(parentNode->lChild) == curNode){
					
				//pointerNode_t * newParentPtrNode = (pointerNode_t *)get_new_pointerNode(data);
					
					//*(newParentPtrNode) = combine_dnode_status_ptrnode(parentDataNode, FREE);
					
					parentNode->rChild = siblingNode->lChild;
					
					cptr = (node_t *)get_child(parentNode->rChild);
					
					//cdata = (dataNode_t *)extract_dnode_from_ptrnode(*(cptr));
					
					cptr->parent = parentNode;
					
					curNode->parent = parentNode;
					
					siblingNode->lChild = create_child_word(parentNode,OFIN);
					
					siblingNode->color = BLACK;
					parentNode->color = RED;
					
					node_t * sibChildNode = NULL;
					//dataNode_t * sibChildDataNode = NULL;
					
					
					if(parentNode == wRootChildCopy){
						parentNode->parent = NULL;
						sibChildNode = (node_t *)get_child(siblingNode->rChild);
						if(sibChildNode != NULL){
							//sibChildDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(sibChildPtrNode));
							
							sibChildNode->parent = NULL;
						}	
						//siblingNode->opData = O;
						// parent is root of window. 
						balance_shortness(data, nextWindowRootNode,curNode, wRoot, siblingNode,wRootChild, casField, O);

					}
					else{
						if(parentNode->parent == NULL){
							// parent is Child of window root
							parentNode->parent = siblingNode;
							if((node_t *)get_child(wRootChildCopy->lChild) == parentNode){
								wRootChildCopy->lChild = create_child_word(siblingNode,OFIN);
							}
							else if((node_t *)get_child(wRootChildCopy->rChild) == parentNode){
								wRootChildCopy->rChild = create_child_word(siblingNode,OFIN);

							}
					#ifdef DEBUG
							else{
								std::cout << "pointers incorrectly assigned. Balance shortness function. Location 2" << std::endl;
								exit(0);
							}
					#endif	
							balance_shortness(data, nextWindowRootNode, curNode, wRoot, wRootChildCopy, wRootChild, casField, O);
							
							
						}
						else{
						node_t * pParentNode = parentNode->parent;
						//dataNode_t * pParentDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(pParentPtrNode));
							
							parentNode->parent = siblingNode;
							
							if((node_t *)get_child(pParentNode->lChild) == parentNode){
								pParentNode->lChild = create_child_word(siblingNode,OFIN);
							}
							else if((node_t *)get_child(pParentNode->rChild) == parentNode){
								pParentNode->rChild = create_child_word(siblingNode,OFIN);

							}
					#ifdef DEBUG
							else{
						std::cout << "pointers incorrectly assigned. Balance shortness function. Location 3" << std::endl;
								exit(0);
							}
					#endif		
							balance_shortness(data, nextWindowRootNode, curNode, wRoot, wRootChildCopy, wRootChild, casField, O);
						}
							
					}
					
				}
				else if((node_t *)get_child(parentNode->rChild) == curNode){
				
					//pointerNode_t * newParentPtrNode = (pointerNode_t *)get_new_pointerNode(data);
					
					
					//newParentPtrNode->field.AO_val1 = combine_move_and_procid(NULL, 0);					
					//newParentPtrNode->field.AO_val2 = combine_dnode_and_flag(parentDataNode, 0);
					//*(newParentPtrNode) = combine_dnode_status_ptrnode(parentDataNode, FREE);
					
					parentNode->lChild = siblingNode->rChild;
					
					cptr = (node_t *)get_child(siblingNode->rChild);
					
					//cdata = (dataNode_t *)extract_dnode_from_ptrnode(*(cptr));
					cptr->parent = parentNode;
					
					curNode->parent = parentNode;
					siblingNode->rChild = create_child_word(parentNode,OFIN);
					siblingNode->color = BLACK;
					parentNode->color = RED;
					
					node_t * sibChildNode = NULL;
					//dataNode_t * sibChildDataNode = NULL;
				
					
					if(parentNode == wRootChildCopy){
						parentNode->parent = NULL;
						sibChildNode = (node_t *)get_child(siblingNode->lChild);
						if(sibChildNode != NULL){
							//sibChildDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(sibChildPtrNode));
						
							sibChildNode->parent = NULL;
						}
						//siblingDataNode->opData = O;
						// parent is root of window. 
						balance_shortness(data, nextWindowRootNode, curNode, wRoot, siblingNode, wRootChild, casField, O);
						
					}
					else{
					
						// parent is not root of window
						if(parentNode->parent == NULL){
							// parent is Child of window root
							parentNode->parent = siblingNode;
							if((node_t *)get_child(wRootChildCopy->lChild) == parentNode){
								wRootChildCopy->lChild = create_child_word(siblingNode,OFIN);
							}
							else if((node_t *)get_child(wRootChildCopy->rChild) == parentNode){
								wRootChildCopy->rChild = create_child_word(siblingNode,OFIN);

							}
							else{
						std::cout << "pointers incorrectly assigned. Balance shortness function. Location 4" << std::endl;
								exit(0);
							}	
							balance_shortness(data, nextWindowRootNode, curNode, wRoot, wRootChildCopy, wRootChild, casField, O);
						}
						else{
							node_t * pParentNode = parentNode->parent;
							//dataNode_t * pParentDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(pParentPtrNode));
							parentNode->parent = siblingNode;
							if((node_t *)get_child(pParentNode->lChild) == parentNode){
								pParentNode->lChild = create_child_word(siblingNode,OFIN);
							}
							else if((node_t *)get_child(pParentNode->rChild) == parentNode){
								pParentNode->rChild = create_child_word(siblingNode,OFIN);

							}
							else{
						std::cout << "pointers incorrectly assigned. Balance shortness function. Location 5" << std::endl;
								exit(0);
							}		
							balance_shortness(data, nextWindowRootNode, curNode, wRoot, wRootChildCopy, wRootChild, casField, O);
						}
							
					}

				}
				else{
					std::cout << "pointers incorrectly assigned. Balance shortness function. Location 1" << std::endl;
					exit(0);
				}
			}
		}	
	return NULL;	
}


bool check_delete_case_1(thread_data_t * data, node_t * wRoot, node_t * wRootChildCopy, node_t * wRootChild, long key, AO_t casField, oprec_t * O){
	
	//std::cout << "1" << std::endl;
	
	int depth = 1;
	node_t * nextNode = NULL;
	//dataNode_t * nextDataNode = NULL;
	node_t * nextSNode = NULL;
	//dataNode_t * nextSDataNode = NULL;	
	node_t * childNode = NULL;
	//dataNode_t * childDataNode = NULL;
	node_t * childSNode = NULL;
	//dataNode_t * childSDataNode = NULL;
	
	node_t * cptr = NULL;
	//dataNode_t * cdata = NULL;
	
	//Word nextWord;
	
	
	if(key <= wRootChildCopy->key){
		nextNode = (node_t *)get_child(wRootChildCopy->lChild);
		//nextDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(nextPtrNode));
		nextSNode = (node_t *)get_child(wRootChildCopy->rChild);
		//nextSDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(nextSPtrNode));
	}
	else{
		nextNode = (node_t *)get_child(wRootChildCopy->rChild);
		//nextDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(nextPtrNode));
		nextSNode = (node_t *)get_child(wRootChildCopy->lChild);
		//nextSDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(nextSPtrNode));		
	}
	
	if(is_external_node(nextNode)){
		if(nextNode->key == key){
			
			
			//data->cRecords[pid].push_back(nextDataNode->valData);
			
			data->numDelete = key;
				
			wRootChildCopy->key = nextSNode->key;
			
			
			
			wRootChildCopy->lChild =  nextSNode->lChild;
			wRootChildCopy->rChild =  nextSNode->rChild;
			
			
			//wRootDataNode->valData = nextSDataNode->valData;
			
			
			
			wRootChildCopy->color = nextSNode->color;
			
		
			
			
			//nextSNode->rChild = NULL;
			//nextSNode->lChild = NULL;
			//nextSNode->parent = NULL;
			
			
			
			
		
		
		
		//wRootDataNode->opData = O;
		//wRootDataNode->wop2 = 41665;
					#ifdef DEBUG
		wRootChildCopy->mover = 1377;
			#endif
		wRootChildCopy->move = NULL;
		
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,1398);
		
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode, DONE));
		if(result0 == 1){						
			// cas success
			if(data->numDelete != -1){
				data->numActualDelete++;
				data->numDelete = -1;
			}
		//flush_window_on_dcas_success(data, pid);
		//std::cout << "2" << std::endl;
			return true;
		
		} 
		else{
			// cas failure. Some other node executed window
		//free_window_on_dcas_failure(data,pid);
		//data->fdelCas++;
			data->numDelete = -1;
			//std::cout << "3" << std::endl;
			return true;
		}
			
			
		
			
		}
		else{
			
			
					#ifdef DEBUG
				
			wRootChildCopy->mover = 1411;
			#endif
			wRootChildCopy->move = NULL;
		 
		
			int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,1433);
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode,DONE));
			if(result0 == 1){						
				// cas success
			
					data->numDelete = -1;
				//std::cout << "4" << std::endl;
		//flush_window_on_dcas_success(data,pid);
				return true;
		
			} 
			else{
				// cas failure. Some other node executed window
			
			//free_window_on_dcas_failure(data,pid);
			//data->fdelCas++;
				data->numDelete = -1;
				//std::cout << "5" << std::endl;
				return true;
			}
			
			
		
		}
		
	}
	
	while(depth < (DELETE_WINDOW_SIZE - 2)){
		
		if(key <= nextNode->key){
			childNode = (node_t *)get_child(nextNode->lChild);
			//childDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(childPtrNode));
			if( is_external_node(childNode)){
				if (childNode->key == key){
					data->numDelete = key;
					//data->cRecords[pid].push_back(childDataNode->valData);
					bool flag = false;
					if(nextNode->color == RED)
						flag = true;
					// external node with key is encountered. Delete and rebalance.	
					
					childSNode = (node_t *)get_child(nextNode->rChild);
					//childSDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(childSPtrNode));
					nextNode->key = childSNode->key;
					
					//nextDataNode->valData = childSDataNode->valData;
					nextNode->lChild = childSNode->lChild;
					nextNode->rChild = childSNode->rChild;	
				
					
					
						if(childSNode->color == RED && nextNode->color == BLACK){
							nextNode->color = BLACK;
							flag = true;
						}				
						else{
							nextNode->color = childSNode->color;
						}

					
					
					cptr = (node_t *)get_child(nextNode->lChild);
					if(cptr != NULL){
					//cdata = (dataNode_t *)extract_dnode_from_ptrnode(*(cptr));
					
					cptr->parent = nextNode;
					}
					
					cptr = (node_t *)get_child(nextNode->rChild);
					if(cptr != NULL){
					//cdata = (dataNode_t *)extract_dnode_from_ptrnode(*(cptr));
					
					cptr->parent = nextNode;
					}					
					childSNode->parent = NULL;
					//childSNode->lChild = NULL;
					//childSNode->rChild = NULL;	
					
					childNode->parent = NULL;
					//childNode->lChild = NULL;
					//childNode->rChild = NULL;
					
					
					
					if(!flag){
						balance_shortness(data, NULL, nextNode, wRoot, wRootChildCopy, wRootChild, casField, O);
					}	
					
					else{
						
		
		//wRootDataNode->opData = O;
		//wRootDataNode->wop2 = 41827;
		//wRootChildCopy->mover = 1508;
		wRootChildCopy->move = NULL;
		
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,1529);
//		int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode, DONE));
		if(result0 == 1){						
			// cas success
			if(data->numDelete != -1){
				data->numActualDelete++;
				data->numDelete = -1;
			}
		//flush_window_on_dcas_success(data,pid);
		//std::cout << "6" << std::endl;
			return true;
		
		} 
		else{
			// cas failure. Some other node executed window
		//free_window_on_dcas_failure(data,pid);
		//data->fdelCas++;
			data->numDelete = -1;
			//std::cout << "7" << std::endl;
			return true;
		}
						
						
					}
					//std::cout << "8" << std::endl;
					return true;
				}
				else{
					// Key not found in tree. Return
		
		
				//wRootDataNode->opData = O;
				//wRootDataNode->wop2 = 41871;
					#ifdef DEBUG
				wRootChildCopy->mover = 1544;
					#endif
				wRootChildCopy->move = NULL;

		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,1565);		
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode,DONE));
				if(result0 == 1){						
					// cas success
				
						data->numDelete = -1;
		//flush_window_on_dcas_success(data,pid);
		//std::cout << "9" << std::endl;
					return true;
		
				} 
				else{
					// cas failure. Some other node executed window
			//	free_window_on_dcas_failure(data,pid);
			//data->fdelCas++;
					data->numDelete = -1;
					//std::cout << "10" << std::endl;
					return true;
				}
					
					
				}
			}
			else{
				// child is not external
				nextNode = childNode;
				//nextDataNode = childDataNode;
				
				depth++;
							
	
			}
		}
		else{
			childNode = (node_t *)get_child(nextNode->rChild);
			//childDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(childPtrNode));
			if( is_external_node(childNode)){
				if (childNode->key == key){
					data->numDelete = key;
					//data->cRecords[pid].push_back(childDataNode->valData);
					// external node with key is encountered. Delete and rebalance.	
					bool flag = false;
					if(nextNode->color == RED)
						flag = true;
					node_t * childSNode;
					//dataNode_t * childSDataNode;
					childSNode = (node_t *)get_child(nextNode->lChild);
					//childSDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(childSPtrNode));
					nextNode->key = childSNode->key;
					
					//nextDataNode->valData = childSDataNode->valData;
					nextNode->lChild = childSNode->lChild;
					nextNode->rChild = childSNode->rChild;	
					
					
				
					if(childSNode->color == RED && nextNode->color == BLACK){
						nextNode->color = BLACK;
						flag = true;
					}
					else{
						nextNode->color = childSNode->color;
					}
					
					cptr = (node_t *)get_child(nextNode->lChild);
					if(cptr != NULL){
					//cdata = (dataNode_t *)extract_dnode_from_ptrnode(*(cptr));
					
					cptr->parent = nextNode;
					}
					
					cptr = (node_t *)get_child(nextNode->rChild);
					if(cptr != NULL){
					//cdata = (dataNode_t *)extract_dnode_from_ptrnode(*(cptr));
					
					cptr->parent = nextNode;
					}
					childSNode->parent = NULL;
					//childSNode->lChild = NULL;
					//childSNode->rChild = NULL;	
					
					childNode->parent = NULL;
					//childNode->lChild = NULL;
					//childNode->rChild = NULL;
					
					
					
					if(!flag){
						balance_shortness(data, NULL, nextNode, wRoot, wRootChildCopy, wRootChild, casField, O);
					}	
						
					else{
					
		
		
		//wRootDataNode->opData = O;
		//wRootDataNode->wop2 = 41995;
		#ifdef DEBUG
		wRootChildCopy->mover = 1644;
		#endif		
wRootChildCopy->move = NULL;
		
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,1665);
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode, DONE));
		if(result0 == 1){						
			// cas success
			if(data->numDelete != -1){
				data->numActualDelete++;
				data->numDelete = -1;
			}
	//	flush_window_on_dcas_success(data,pid);
	//std::cout << "11" << std::endl;
			return true;
		
		} 
		else{
			// cas failure. Some other node executed window
			//free_window_on_dcas_failure(data,pid);
			//data->fdelCas++;
			data->numDelete = -1;
			//std::cout << "12" << std::endl;
			return true;
		}
						
						
					}	
				//std::cout << "13" << std::endl;
					return true;
				}
				else{
					// Key not found in tree. Return
			
		
		
			//wRootDataNode->opData = O;
			//wRootDataNode->wop2 = 42041;
			//wRootChildCopy->mover = 1681;
			wRootChildCopy->move = NULL;
		
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,1702);
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDataNode, DONE));
			if(result0 == 1){						
				// cas success
			
					data->numDelete = -1;
				
		//flush_window_on_dcas_success(data,pid);
		//std::cout << "14" << std::endl;
				return true;
		
			} 
			else{
				// cas failure. Some other node executed window
			//data->fdelCas++;
			//	free_window_on_dcas_failure(data,pid);
				data->numDelete = -1;
				//std::cout << "15" << std::endl;
				return true;
			}
					
				//	std::cout << "16" << std::endl;
					return true;
				}
			}
			else{
				// child is not external
				nextNode = childNode;
				//nextDataNode = childDataNode;
				
				depth++;
				
	
			}
		}
				
	}

	// does not satisfy case 1
	//	std::cout << "c2f" << std::endl;
		return(false);
	
	
}

node_t * handle_delete_case_3(thread_data_t * data,  node_t * wRoot, node_t * wRootChildCopy, node_t * wRootChild, long key, AO_t casField, oprec_t * O){
	
	
	
	node_t * curNode = NULL;
	//node_t * curDataNode = NULL;

	if(key <= wRootChildCopy->key){
		curNode = (node_t *)get_child(wRootChildCopy->lChild);
	}
	else{
		curNode = (node_t *)get_child(wRootChildCopy->rChild);
	}		
	
	//curDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(curPtrNode));

	node_t * childNode = NULL;
	//dataNode_t * childDataNode = NULL;
	
	node_t * childSNode = NULL;
	//dataNode_t * childSDataNode = NULL;

	if(key <= curNode->key){
		childNode = (node_t *)get_child(curNode->lChild);
		childSNode = (node_t *)get_child(curNode->rChild);
	}
	else{
		childNode = (node_t *)get_child(curNode->rChild);
		childSNode = (node_t *)get_child(curNode->lChild);
	}	
	
	//childDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(childPtrNode));
	//childSDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(childSPtrNode));

	node_t * gchildNode = NULL;
	//dataNode_t * gchildDataNode = NULL;
	
	node_t * gchildSNode = NULL;
	//dataNode_t * gchildSDataNode = NULL;

	if(key <= childNode->key){
		gchildNode = (node_t *)get_child(childNode->lChild);
		gchildSNode = (node_t *)get_child(childNode->rChild);
	}
	else{
		gchildNode = (node_t *)get_child(childNode->rChild);
		gchildSNode = (node_t *)get_child(childNode->lChild);
	}	
	
	// gChildPtrNode is Z.
	// Color Z and its sibling red. Also color childSibling red.
	
	
	//gchildDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(gchildPtrNode));
	//gchildSDataNode = (dataNode_t *)extract_dnode_from_ptrnode(*(gchildSPtrNode));
	
		

	childSNode->color = RED;
	gchildNode->color = RED;
	gchildSNode->color = RED;

	balance_shortness(data, gchildNode, curNode, wRoot, wRootChildCopy, wRootChild, casField, O);
	
	// Return Z 
	return(gchildNode);
	
}

node_t * get_next_node_on_delete_access_path( thread_data_t * data, long key, node_t * wRoot, node_t * wRootChildCopy, node_t * wRootChild, AO_t casField, oprec_t * O, bool case2flag){
	
	// Perform boundary condition checks
	
	
	if(wRootChildCopy->key == -1){
		// tree is empty
		
		//data->lastdel = 1;
		//wRootDNode->op = -1;
		//wRootDNode->wop2 = 42179;
		#ifdef DEBUG
		wRootChildCopy->mover = 1811;
		#endif
		wRootChildCopy->move = NULL;

		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,1830);
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDNode, DONE));
		if(result0 == 1){						
			// cas success
			//flush_window_on_dcas_success(data,pid);
			return NULL;
		
		} 
		else{
			// cas failure. Some other node executed window
			//free_window_on_dcas_failure(data,pid);
			//data->fdelCas++;
			data->numDelete = -1;
			return NULL;
		}
		
		
	}
	
	if(is_external_node(wRootChildCopy)){
		// tree has only 1 node
		//data->lastdel = 2;
		if(wRootChildCopy->key == key){
			
			data->numDelete = key;
		
			wRootChildCopy->key = -1;
			
			//record_t * valData = wRootDNode->valData;
			//valData->compoundValue.AO_val1 = 0;
			//valData->compoundValue.AO_val2 = 0;
		}
		
		
		
		//wRootDNode->opData = O;
		//wRootDNode->wop2 = 42227;
					#ifdef DEBUG
		wRootChildCopy->mover = 1851;
		#endif	
		wRootChildCopy->move = NULL;
		
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,1870);
//		int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDNode,DONE));
		if(result0 == 1){						
			// cas success
			if(data->numDelete != -1){
				data->numActualDelete++;
				data->numDelete = -1;
			}
		//flush_window_on_dcas_success(data,pid);
			return NULL;
		
		} 
		else{
			// cas failure. Some other node executed window
		//	free_window_on_dcas_failure(data,pid);
		//data->fdelCas++;
			data->numDelete = -1;
			return NULL;
		}
		
		return NULL;
	}
	
	

	
	// check for case 1		
	// i.e. whether an external node is encountered
	if(check_delete_case_1(data, wRoot, wRootChildCopy, wRootChild, key, casField, O)){
	//data->lastdel = 3;
		//std::cout << "c1" << std::endl;
		return(NULL);
	}

		
	if(!case2flag){
	
	 node_t * nextNode = check_delete_case_2(data,wRootChildCopy, key, O);

	 if(nextNode == NULL){
	//	std::cout << "Does not satisfy case 2" << std::endl;
	 }
	 else{
		//std::cout << "Satisfies Case 2" << std::endl;
		
		
	node_t * move = lock_new_ipath(data, nextNode, wRootChildCopy,O);
	/*	if(wRootChildCopy == move){
			std::cout << "Happening1918" << std::endl;
			exit(0);
		}*/	

					#ifdef DEBUG
		if(wRootChildCopy == move){
		wRootChildCopy->mover = 1901;
		}
		else{
			wRootChildCopy->mover = 1902;
		}	
		#endif
		wRootChildCopy->move = move;	
		//wRootChildCopy->moverop = O;
	 	//AO_double_t localRootPtr = (casField);
	 	//AO_double_t newRootPtrNode;
	 	//newRootPtrNode.AO_val1 = combine_move_and_procid(nextNode, pid);
		//newRootPtrNode.AO_val2 = combine_dnode_and_flag(wRootDNode, extract_flag_from_ptrNode_field2(localRootPtr.AO_val2));
		
		int result0 = complete_cas(data, casField, wRoot, wRootChild, wRootChildCopy, O,1934);
		
		//int result = atomic_cas_full1(data,wRootPtrNode, casField, combine_dnode_status_ptrnode(wRootDNode, DONE));
		if(result0 == 1){						
			// cas success
			if(data->numDelete != -1){
				data->numActualDelete++;
				data->numDelete = -1;
			}
			/*if(wRootPtrNode == data->rootOfTree){
				flush_window_on_dcas_success(data,pid);
			}
			else{
				flush_window_on_dcas_success(data,pid);
			}*/
			return NULL;
		
		} 
		else{
			// cas failure. Some other node executed window
			//free_window_on_dcas_failure(data,pid);
			//data->fdelCas++;
			data->numDelete = -1;
			return NULL;
		}
		
		
		return nextNode; 
	 }
	 
	 }
	
	//data->lastdel = 5;
	// satisfies case 3 
	//std::cout << "c3" << std::endl;
	return(handle_delete_case_3(data,  wRoot, wRootChildCopy, wRootChild, key, casField, O));
	
}



	



