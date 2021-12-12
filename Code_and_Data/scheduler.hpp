#ifndef scheduler_hpp

#define scheduler_hpp


#include <ctime>
#include "vertexDeque.cpp"
#include "treebuilder.hpp"
#include "worker.hpp"


//WHEN YOU UPDATE ME, UPDATE IN <worker.hpp> AS WELL!!! begin
#define NUM_DEQUES 100000
//WHEN YOU UPDATE ME, UPDATE IN <worker.hpp> AS WELL!!! end
/* WHEN MAKING MODIFICATIONS TO TEST PARAMETERS DO NOT FORGET TO MODIFY COMPUTATION GRAPH IN SCHEDULER.CPP begin*/
#define N 5000
#define NUM_TASKS N-2//2^n-2
#define NUM_WORKERS 30
#define NUM_ROUNDS 5
/* WHEN MAKING MODIFICATIONS TO TEST PARAMETERS DO NOT FORGET TO MODIFY COMPUTATION GRAPH IN SCHEDULER.CPP end*/

extern int IS_PARALLEL;


extern int completed_tasks;

extern vertexDeque* gDeques [NUM_DEQUES];
extern deque < vertexDeque*> resumedDeques;

vertexDeque* random_gDeque();


int main();

#endif
