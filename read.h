
void seek(thread_data_t * data, oprec_t * O, long key, seekrec_t * R){
  //std::cout << "SEEK" << std::endl;
  // initially priAddress is tree root, as invariant trivially holds there

  node_t * root = data->prootOfTree;
  	
  R->addresses[0] =   combine_address_which_addresses(root, LEFT);
  R->contents[0] = (root->opData);
  
  int opn = map_word_to_operation(O->op);
	
  node_t * candidate;
	
  if(key <= root->key){
    candidate = (node_t *)get_child(root->lChild);
  }
  else{
    candidate = (node_t *)get_child(root->rChild);
  }
  
  R->addresses[1] = combine_address_which_addresses(candidate, LEFT);
  R->contents[1] = (candidate->opData);
  R->length = 2;  
  
  if(key <= candidate->key){
    candidate = (node_t *)get_child(candidate->lChild);
  }
  else{
    candidate = (node_t *)get_child(candidate->rChild);
  }
	
  if(candidate != NULL){
    //std::cout << "HERE" << std::endl;	
    AO_t curContents = (candidate->opData);

//#ifdef DEBUG	
  int depth=0;
//#endif    
  bool flag = false;
 
  while(!is_external_node(candidate)){
//#ifdef DEBUG      
    depth++;
//#endif
      
    if(opn == 1 && !flag){
	    // Delete operation.
	    //Check if a leaf node is encountered within the window rooted at current, before determining if current or its descendent satisfies invariant
	    if(fastcheck_delete_case_1( data, key, candidate, O)){
	      flag = true;
	    }
    }
		
    if(!flag){
	    // Leaf ndoe not encountered in delete window
	    is_invariant_satisfied(data, candidate, curContents, O, R);
	    // R is already updated
    }
      
      // next node on access path
      node_t * next;
      AO_t nextContents;
      
      if(key <= candidate->key){
	      next = (node_t *)get_child(candidate->lChild);
      }
      else{
	      next = (node_t *)get_child(candidate->rChild);
      }
		  nextContents = (next->opData);
		
		  candidate = next;
		  curContents = nextContents;
		  //curDNode = (dataNode_t *)extract_dnode_from_ptrnode(curContents);

//#ifdef DEBUG		
		  if(depth > 1000){
	  		std::cout << "Iterations Exceeded__111r" << std::endl;
	  		exit(0);
  		}
//#endif  		
	}
	
	// curDNode is leaf node
	R->leafKey = candidate->key;
	//return R;
	}
	else{
		R->leafKey = root->key;
	}
	
}


void search(thread_data_t * data, long key){
	
	node_t * root = data->prootOfTree;
	
	while(root != NULL){
		
		if(key <= root->key){
			root = (node_t *)get_child(root->lChild);
		}
		else{
			root = (node_t *)get_child(root->rChild);
		}
	}
	
}

long update_search(thread_data_t * data, long key){
	
	node_t * root = data->prootOfTree;
	
	while(!is_external_node(root)){
		
		if(key <= root->key){
			root = (node_t *)get_child(root->lChild);
		}
		else{
			root = (node_t *)get_child(root->rChild);
		}
	}
	
	return root->key;
	
}




