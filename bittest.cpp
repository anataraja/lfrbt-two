//#include <linux/atomic.h>
#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include "wfrbt.h"




int main(int argc, char* argv[]){

AO_t word = atol(argv[1]);

//AO_t word = 4483460525791248;
std::cout << "" << word << std::endl;
std::cout << "is_node_owned = " << is_node_owned(word) << std::endl;
std::cout << "is_node_guardian = " << is_node_guardian(word) << std::endl;
std::cout << "is_initial_txn = " << is_initial_txn(word) << std::endl;
std::cout << "Oprec = " << (oprec_t *)extract_oprec_from_opdata(word) << std::endl;
}
