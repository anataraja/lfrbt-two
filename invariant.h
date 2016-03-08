#pragma once

/// Determine nodes on cpath and populate seekRecord.

bool check_ins_inv(thread_data_t * data, node_t * pcurrent, AO_t pcurContents, seekrec_t * R, long key) {

  node_t * current;
  bool pwhich = RIGHT; // which from parent to current
  bool cwhich = RIGHT; // which from current to next

  if (key < pcurrent->key) {
    current = (node_t *) get_child(pcurrent->lChild);
    pwhich = LEFT;
  } else {
    current = (node_t *) get_child(pcurrent->rChild);
  }

  if (current == NULL) {
    // pcurrent is a leaf node. Invariant is trivially not satisfied.
    return false;
  }

  AO_t curContents = current->opData;

  if (is_external_node(current)) {
    return false;
  }

  node_t * next;
  node_t * sibling;
  if (key < current->key) {
    next = (node_t *) get_child(current->lChild);
    sibling = (node_t *) get_child(current->rChild);
    cwhich = LEFT;
  } else {
    next = (node_t *) get_child(current->rChild);
    sibling = (node_t *) get_child(current->lChild);
    cwhich = RIGHT;
  }

  if (is_external_node(next) || is_external_node(sibling)) {
    return false;
  }

  if (current->color == BLACK) {
    AO_t nextContents = (next->opData);

    AO_t siblingContents = (sibling->opData);

    if (next->color == BLACK) {
      // invariant satisfied
      R->addresses[0] = combine_address_which_addresses(pcurrent, pwhich);
      R->contents[0] = pcurContents;

      R->addresses[1] = combine_address_which_addresses(current, cwhich);
      R->contents[1] = curContents;
      std::cout << "IPATH__" << pcurrent << "__" << current << std::endl;
      //R->addresses[2] = combine_address_which_addresses(next, LEFT);
      //R->contents[2] = nextContents;

      R->length = 2;

      return true;

    }

    else if (sibling->color == BLACK) {
      // invariant satisfied
      R->addresses[0] = combine_address_which_addresses(pcurrent, pwhich);
      R->contents[0] = pcurContents;

      R->addresses[1] = combine_address_which_addresses(current, !cwhich);
      R->contents[1] = curContents;
      std::cout << "IPATH__" << pcurrent << "__" << current << std::endl;
      //R->addresses[2] = combine_address_which_addresses(sibling, LEFT);
      //R->contents[2] = siblingContents;

      R->length = 2;

      return true;
    }

    else {
      // both children of current are red. Invariant is not satisfied.
      return false;
    }
  }
}
 
bool check_del_inv(thread_data_t * data, node_t * pcurrent, AO_t pcurContents, seekrec_t * R, long key){
 	
	node_t * current;
	bool pwhich = RIGHT; // which from parent to current
	bool cwhich = RIGHT; // which from current to next
	
	if(key < pcurrent->key){
		current = (node_t *)get_child(pcurrent->lChild);
		pwhich = LEFT;
	}
	else{
		current = (node_t *)get_child(pcurrent->rChild);
	}
	
	if(current == NULL){
		return false;
	}
	
	AO_t curContents = current->opData;
	
 	if(current->color == RED){
 		R->addresses[0] = combine_address_which_addresses(pcurrent, pwhich);
 		R->contents[0] = pcurContents;
 			
		R->addresses[1] = combine_address_which_addresses(current, LEFT);
 		R->contents[1] = curContents;
			
 		for(int i = 2; i <=3; i++){
 			R->addresses[i] = NULL;
 			R->contents[i] = 0;
 		}
			
		R->length = 2;
			
 		return true;
 	}
 	else{
 	  
 	  node_t * child;
 	  node_t * lgc; // left grandchild
 	  node_t * rgc; // right grandchild
 	
 	  AO_t childContents;
 	  AO_t lgcContents;
 	  AO_t rgcContents;
 	
 			// current is not red
 			
 			// First search left subtree of current
 			
 			child = (node_t *)get_child(current->lChild);
 			cwhich = LEFT;
 			
 			if(child != NULL){
 				childContents = (child->opData);
 				//dataNode_t * childDNode = (dataNode_t *)extract_dnode_from_ptrnode(childContents);
 				if(child->color == RED){
					R->addresses[0] = combine_address_which_addresses(pcurrent, pwhich);
		 			R->contents[0] = pcurContents;
					
 					R->addresses[1] = combine_address_which_addresses(current, cwhich);
		 			R->contents[1] = curContents;	
		 			
		 			R->addresses[2] = combine_address_which_addresses(child, LEFT);
		 			R->contents[2] = childContents;	
		 			
		 			R->addresses[3] = NULL;
		 			R->contents[3] = 0;	
					
					R->length = 3;
					
					return true;		 			
 				}
 				else{
 					if(child->lChild != NULL && child->rChild != NULL){
 						
 						lgc =  (node_t *)get_child(child->lChild);					
 						AO_t lgcContents = (lgc->opData);
		 				
		 				
		 				rgc =  (node_t *)get_child(child->rChild);
		 				AO_t rgcContents = (rgc->opData);
		 				
		 				if(lgc->color == RED){
		 					// invariant satisfied
		 					R->addresses[0] = combine_address_which_addresses(pcurrent, pwhich);
		 					R->contents[0] = pcurContents;
							
							R->addresses[1] = combine_address_which_addresses(current, cwhich);
		 					R->contents[1] = curContents;
		 					
		 					R->addresses[2] = combine_address_which_addresses(child, LEFT);
		 					R->contents[2] = childContents;
		 					
		 					R->addresses[3] = combine_address_which_addresses(lgc, LEFT);
		 					R->contents[3] = lgcContents;
							
							R->length = 4;
							
		 					return true;
		 					 
		 				}
		 				else if(rgc->color == RED){
		 					// invariant satisfied
		 					R->addresses[0] = combine_address_which_addresses(pcurrent, pwhich);
		 					R->contents[0] = pcurContents;
							
							R->addresses[1] = combine_address_which_addresses(current, cwhich);
		 					R->contents[1] = curContents;
		 					
		 					R->addresses[2] = combine_address_which_addresses(child, RIGHT);
		 					R->contents[2] = childContents;
		 					
		 					R->addresses[3] = combine_address_which_addresses(rgc, LEFT);
		 					R->contents[3] = rgcContents;
							
							R->length = 4;
							
		 					return true;
		 					
		 					
		 				}
		 				else{
		 					// both grandchildren of current are black. Invariant is not satisfied.
		 					return false;
		 				}
 						
 					}
 					else{
 						return false;
 					}
 				}
 			}
 			
 			// Next search right subtree of current
 			child = (node_t *)get_child(current->rChild);
 			cwhich = RIGHT;
 			
 			if(child != NULL){
 				childContents = (child->opData);
 				if(child->color == RED){
 					R->addresses[0] = combine_address_which_addresses(pcurrent, pwhich);
		 			R->contents[0] = pcurContents;
							
					R->addresses[1] = combine_address_which_addresses(current, cwhich);
		 			R->contents[1] = curContents;	
		 			
		 			R->addresses[2] = combine_address_which_addresses(child, LEFT);
		 			R->contents[2] = childContents;	
		 			
		 			R->addresses[3] = NULL;
		 			R->contents[3] = 0;	
					
					R->length = 3;
					
					return true;		 			
 				}
				else{
					if(child->lChild != NULL && child->rChild != NULL){
						
					lgc =  (node_t *)get_child(child->lChild);					
					Word lgcContents = (lgc->opData);
	 				
	 				rgc =  (node_t *)get_child(child->rChild);
	 				Word rgcContents = (rgc->opData);
		 				
	 				if(lgc->color == RED){
	 					// invariant satisfied
						R->addresses[0] = combine_address_which_addresses(pcurrent, pwhich);
	 					R->contents[0] = pcurContents;
							
						R->addresses[1] = combine_address_which_addresses(current, cwhich);
	 					R->contents[1] = curContents;
		 					
	 					R->addresses[2] = combine_address_which_addresses(child, LEFT);
	 					R->contents[2] = childContents;
		 					
	 					R->addresses[3] = combine_address_which_addresses(lgc, LEFT);
	 					R->contents[3] = lgcContents;
							
						R->length = 4;
						
	 					return true;
	 					 
	 				}
	 				else if(rgc->color == RED){
	 					// invariant satisfied
	 					R->addresses[0] = combine_address_which_addresses(pcurrent, pwhich);
	 					R->contents[0] = pcurContents;
							
						R->addresses[1] = combine_address_which_addresses(current, cwhich);
	 					R->contents[1] = curContents;
		 					
	 					R->addresses[2] = combine_address_which_addresses(child, RIGHT);
	 					R->contents[2] = childContents;
	 					
	 					R->addresses[3] = combine_address_which_addresses(rgc, LEFT);
	 					R->contents[3] = rgcContents;
							
						R->length = 4;
							
	 					return true;
	 				}
	 				else{
	 					// both grandchildren of current are black. Invariant is not satisfied.
	 					return false;
	 				}
 					
 				}
 				else{
 					return false;
 				}
 			}
 		}
 		return false;
	}		
}

bool is_invariant_satisfied(thread_data_t * data, node_t * current, AO_t curContents, oprec_t * O, seekrec_t * R){
	
  long key = map_word_to_key(O->op);
	unsigned opn = map_word_to_operation(O->op); 
	 
	if(opn == 2){
	  // insert operation
	 	return(check_ins_inv(data, current, curContents, R, key));
	}
	else{
	 	// delete operation
	 	return(check_del_inv(data, current, curContents, R, key));
	}
}
 
bool fastcheck_delete_case_1(thread_data_t * data, long key, node_t * pcurrent, oprec_t * O){
	
	node_t * current;
	if(key < pcurrent->key){
		current = (node_t *)get_child(pcurrent->lChild);
	}
	else{
		current = (node_t *)get_child(pcurrent->rChild);
	}
	
	if(current == NULL){
		// leaf node reached.
		return true;
	}
	
	int depth = 1;

	node_t * next = NULL;
	node_t * nextS = NULL;
	node_t * child = NULL;
	node_t * childS = NULL;
	node_t * cptr = NULL;
	
	if(key < current->key){
		next = (node_t *)get_child(current->lChild);
		if(next == NULL){
			return true;
		}
		
		nextS = (node_t *)get_child(current->rChild);
		if(nextS == NULL){
			return true;
		}
	}
	else{
		next = (node_t *)get_child(current->rChild);
		if(next == NULL){
			return true;
		}
		
		nextS = (node_t *)get_child(current->lChild);
		if(nextS == NULL){
			return true;
		}
	}
	
	if(is_external_node(next) || is_external_node(nextS)){
		return true;
	}
	
	while(depth < (DELETE_WINDOW_SIZE - 2)){
		if(key < next->key){
			child = (node_t *)get_child(next->lChild);
			
			if( is_external_node(child)){
				return true;
			}
			else{
				// child is not external
				next = child;
				depth++;
			}
		}
		else{
			child = (node_t *)get_child(next->rChild);
			if( is_external_node(child)){
				return true;			
			}
			else{
				// child is not external
				next = child;
				depth++;
			}
		}
	}

	// does not satisfy case 1
		return false;
}

