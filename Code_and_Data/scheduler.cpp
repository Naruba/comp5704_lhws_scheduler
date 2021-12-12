#ifndef scheduler_cpp
#define scheduler_cpp

#include "scheduler.hpp"

float prob_he=0.00;
float prob_br=1.0;
float prob_lf=0.00;

int gTotalDeques=0;
int IS_PARALLEL=0;

worker* p[NUM_WORKERS];
deque < vertexDeque*> resumedDeques;
std::atomic<int> gtotaldeques;
vertexDeque* gDeques [NUM_DEQUES];
vertexDeque* resumedVertices [NUM_DEQUES];

void initialize_workers(){
    for(int i=0;i<NUM_WORKERS;i++){
        cout << "Spinning up worker[" << i << "]... " << endl;
        p[i]=new worker;
        p[i]->idle=true;
        p[i]->deque_list=new deque_linked_list();
        p[i]->readyDeques=new deque_linked_list();
        p[i]->emptyDeques=new deque_linked_list();
        p[i]->completed_tasks=0;
    }
}

void allocate_initial_deques(){
    for(int i=0;i<NUM_DEQUES;i++){
        resumedVertices[i]=new vertexDeque;
        resumedVertices[i]->q=*new std::deque <Node*>;
        gDeques[i]=new vertexDeque;
        gDeques[i]->q=*new std::deque <Node*>;
        gDeques[i]->resumedVertices=resumedVertices[i];
        gDeques[i]->resumedVertices->q.push_front(NULL);
        gDeques[i]->resumedVertices->q.pop_front();
        gDeques[i]->suspendCtr=0;
    }
}

void run_scheduler(){
    resumedDeques = *new deque< vertexDeque*>;
    /* RANDOM RNG:*/ //srand((int)time(0));// seed RNG
    /* KNOWN RNG:*/ srand(0);
    
    Node* comp_root0; // root of computation0 (map)
    //Node* comp_root1; // root of computation1 (reduce)

    /* WHEN MAKING MODIFICATIONS TO TEST PARAMETERS DO NOT FORGET TO MODIFY COMPUTATION GRAPH IN SCHEDULER.HPP begin*/
    int map_delay=1000;//us
    IS_PARALLEL=1;
    /* WHEN MAKING MODIFICATIONS TO TEST PARAMETERS DO NOT FORGET TO MODIFY COMPUTATION GRAPH IN SCHEDULER.HPP end*/

    comp_root0=generateRoot(map_delay);
    //comp_root1=generateRoot();

    // Build a new tree using root
   
    newTree(comp_root0,NUM_TASKS,prob_he,prob_lf,prob_br);
    dumpTree(comp_root0);
    
    //newTree(comp_root1,NUM_TASKS,prob_he,prob_lf,prob_br);
    //dumpTree(comp_root1);

    //merge_map_reduce(comp_root0, comp_root1, NUM_TASKS, 500); // comp graph starts in comp_root0...
    
    cout << "DONE generating tree. Beginning scheduler... " << endl;
    
    // Spin-up workers
    initialize_workers();
    
    // Allocate gDeques
    allocate_initial_deques();

    // SCHEDULER:
    // (1) Execution starts by setting one worker's <assignedVerteX> to the root of computation
    p[0]->assignedVertex=comp_root0;
    p[0]->idle=false;
    p[0]->activeDeque=gDeques[0]; //setting up first worker stand-alone
    gDeques[0]->q=*new deque<Node*>;
    gtotaldeques++; //setting up first worker stand-alone
    
    // (2) set all workers <activeDeque> to an empty deque
    for(int i=1;i<NUM_WORKERS;i++){ //NOTE: i=1, setting up the remainder in this for-loop
        p[i]->activeDeque=gDeques[i];
        p[i]->deque_list->add(gDeques[i]);
        gDeques[i]->q=*new deque<Node*>;
        gtotaldeques++;
    }
    
    // Perform NUM_ROUNDS scheduling rounds
    int completed_tasks=0; //counter for completed work
    int round_count=0;
    cout <<"ROUND,TASKS COMPLETED,";
    for(int j=0;j<NUM_WORKERS;j++){
        cout <<"worker"<<j<<" tasks,";
    }
    for(int j=0;j<NUM_WORKERS;j++){
        cout <<"worker"<<j<<" running,";
    }
    cout <<endl;

    auto start = chrono::steady_clock::now();

    while(completed_tasks<0.9*5000)// if almost all tasks done, complete.
    {
        round_count++;
        completed_tasks=0;
        // Kick off each worker to do tasks
        for(int j=0;j<NUM_WORKERS;j++){
            p[j]->tick();
            // accumulate total work done by this worker
            completed_tasks+=p[j]->completed_tasks;
        }
    

        cout << round_count << ",";
        cout << completed_tasks << ",";

        // print each worker

        for(int j=0;j<NUM_WORKERS;j++){
            cout << p[j]->completed_tasks << ",";
        }
        for(int j=0;j<NUM_WORKERS;j++){
            if(p[j]->assignedVertex!=NULL){
                cout <<"1,";
            }else{
                cout <<"0,";

            }
        }

        for(int j=0;j<NUM_WORKERS;j++){
//            p[j]->print_worker();
//            // clear the current vertex here, so we could've printed the vertex above
            p[j]->assignedVertex=NULL;
            
        }
        cout <<endl;
    }
    
    auto end = chrono::steady_clock::now();
    auto diff = end - start;


    cout <<"Completed in " << round_count <<" rounds." << endl;
    cout <<"Completed in " ;
    cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;

    // let the spinning threads expire
    cout << "\nMAIN THREAD STALLING FOR 1SEC TO ALLOW SUSPENSIONS TO ENABLE...\nRestarting scheduler...\n\n" << endl;
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    
    cout << "Total completed tasks: " << completed_tasks << endl;
}

int main(){
    run_scheduler();

    return 0;
}

#endif
