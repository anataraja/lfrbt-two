#pragma once

/* ################################################################### *
	HELPING FUNCTIONS
 * ################################################################### */

void update_location(thread_data_t * data, node_t * node, AO_t state, int id){

  oprec_t * helpeeOprec = (oprec_t *)extract_oprec_from_opdata(state);

  /// read the contents of the child field
  AO_t childWord;
  bool which = RIGHT;
  if(extract_child_from_opdata(state) == LEFT){
    childWord = node->lChild;
    which = LEFT;
  }
  else{
    childWord = node->rChild;
  }
  
  if((oprec_t *)extract_oprec_from_opdata(node->opData) == helpeeOprec  && (get_child_status(childWord) == ONEED)){
    if(is_node_owned(state)){
      /// Step 3d: Release (the ownership of the node)
      AO_t newState = make_node_not_owned(state); /// state with own flag set to 0
      
      assert(!is_node_owned(newState));
      
      int result0 = atomic_cas_full1(data, node, state, newState);
      
      if(result0 == 1){
        node->creator = 30;
      }
      
      
      AO_t old_state = node->opData;
      if((oprec_t *)extract_oprec_from_opdata(old_state) == helpeeOprec &&
          is_node_owned(old_state)){
        std::cout << "Fishy" << std::endl;    
        exit(0);
      }
      
   }  
      /// Step 3e: Update (the data record with new location of the guardian node)
      AO_t currentWindowLoc = helpeeOprec->windowLoc;
      node_t * dNode = (node_t *)extract_position_from_oprecord(currentWindowLoc);
      
      node_t * child = (node_t *)get_child(childWord);
      
      if(dNode == node){
        int result1 = atomic_cas_full( &helpeeOprec->windowLoc, currentWindowLoc, combine_position_status_oprecord(child->move, INJECTED));
        if(result1 == 1){
          if(child->move != NULL){
            oprec_t * O1 = (oprec_t *)extract_oprec_from_opdata(child->move->opData);
            if(O1 != helpeeOprec)
              std::cout << "38__~~" << O1 << "___" << helpeeOprec << std::endl; 
            if(!is_node_owned(child->move->opData))
              std::cout << "39__~~" << O1 << "___" << helpeeOprec << std::endl; 
          }
          helpeeOprec->changer = id;
        }
      }
      
      /// step 3f: Reset the flag in the child field
      if(which == LEFT){
        int result2 = atomic_cas_full(&node->lChild, childWord, create_child_word(child, OFIN));
      }
      else{
        int result2 = atomic_cas_full(&node->rChild, childWord, create_child_word(child, OFIN));
      }  
          
    
  }
  
  
}

node_t * find_guardian(thread_data_t * data, node_t * node, AO_t state){
  
  if(is_node_guardian(state)){
    data->lastCase0 = 1;
    return node;
  }
  else if(is_initial_txn(state)){
    data->lastCase0 = 2;
    oprec_t * helpeeOprec = (oprec_t *)extract_oprec_from_opdata(state);
    return ((node_t *)get_address_from_addresses(helpeeOprec->sr->addresses[0]));
  }
  else{
    data->lastCase0 = 3;
    return (node->markedRoot);
  }

}

 

int help(thread_data_t * data, oprec_t * O, node_t * pNode, AO_t pNodeContents,int sucid){
  /// Phase 4: Helping Phase
  oprec_t * helpeeOprec = (oprec_t *)extract_oprec_from_opdata(pNodeContents);
  
  AO_t currentWindowLoc = helpeeOprec->windowLoc;
  
  if(extract_status_from_oprecord(currentWindowLoc) == TRYING){
    /// help inject operation into the tree.
    inject(data, helpeeOprec);
  }
  
  if(extract_status_from_oprecord(currentWindowLoc) == INJECTED){
    /// execute one window transaction
    if(is_node_guardian(pNodeContents)){
      //std::cout << "84" << std::endl;
      /// find the current location of the operation's window using the OpRecord
      node_t * dNode = (node_t *)extract_position_from_oprecord(currentWindowLoc);
      if(dNode == NULL){
        return 0;
      }
      
      if(dNode != pNode){
        /// find if the location field of the data record points to the old or the new guardian node
        AO_t childWord;
        if(extract_child_from_opdata(pNodeContents) == LEFT){
          childWord = pNode->lChild;
        }
        else{
          childWord = pNode->rChild;
        }
        if(((oprec_t *)extract_oprec_from_opdata(pNode->opData) == helpeeOprec) && (is_node_owned(pNodeContents)) && (get_child_status(childWord) == ONEED)){
          /// It is the old guardian node; Perform the remaining steps of a window transaction execution
          AO_t dState = dNode->opData;
          if((oprec_t *)extract_oprec_from_opdata(dState) == helpeeOprec){
            update_location(data, dNode, dState, 107);
          }
        }
      }
      
      /// execute one window transaction
      int temp = perform_one_window_operation(data, pNode, helpeeOprec, 592);
      return temp;
    }
    else{
    //std::cout << "85" << std::endl;
      /// locate the guardian node
      node_t * gNode = find_guardian(data, pNode, pNodeContents);
      //assert(gNode != NULL);
      if(gNode == NULL){
        std::cout << "EXITING__" << pNodeContents << "__" << data->lastCase0 << std::endl;
        exit(0);
      }
      
      AO_t gState = gNode->opData;
      if((oprec_t *)extract_oprec_from_opdata(gState) == helpeeOprec){
        /// Perform helping
        help(data, helpeeOprec, gNode, gState, 153);      
      }
     
    }
  }
}

int help_within_window(thread_data_t * data, oprec_t * O, node_t * current, AO_t curContents, int sucid){
  return (help(data, O, current, curContents, sucid));
}


