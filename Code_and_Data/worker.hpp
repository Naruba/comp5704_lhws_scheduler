#ifndef worker_hpp
#define worker_hpp

#include "scheduler.hpp"
#include "vertexDeque.hpp"
#include "deque_linked_list.hpp"
#include <chrono>
#include <thread>

//WHEN YOU UPDATE ME, UPDATE IN <scheduler.hpp> AS WELL begin
#define NUM_DEQUES 100000
//WHEN YOU UPDATE ME, UPDATE IN <scheduler.hpp> AS WELL end

extern std::atomic<int> gtotaldeques;

void signalHandler( int signum );
void callBack(Node* v, vertexDeque* q);
extern deque <vertexDeque*> resumedDeques;
extern vertexDeque* gDeques [NUM_DEQUES];

extern int completed_tasks;

void resumeVertices(vertexDeque* resumed_deque);
void addResumedVertices();

class worker{
public:
    bool idle;
    int completed_tasks;
    deque_linked_list* deque_list;
    deque_linked_list* readyDeques;
    //deque_linked_list resumedDeques;
    deque_linked_list* emptyDeques;//deques previously owned by p which have been freed by free().
    vertexDeque* activeDeque;
    Node* assignedVertex;
    
    void handle_both_children(){
        if(assignedVertex==NULL){
            addResumedVertices();
            //cout << "Resumed all hung vertices... " << endl;
            //cout << "No available children, aborting... " << endl;
            return;
        }
        // IF vertex ready, execute and get 0,1,or2 children (may be suspended)
        // Below is basically handleChild()
        if(assignedVertex!=NULL){
        if(assignedVertex->right!=NULL){
            //cout << "Assigned vertex has right child! " << assignedVertex->right << endl;
            // (2) THEN handle right node,
            if(assignedVertex->weight_right>1){
                // IF suspended increment suspension counter using handleChild() + install callBack
                //cout << "Heavy edge, child will spin for " << assignedVertex->weight_right << " msec. " << endl;
                activeDeque->suspendCtr++;
                assignedVertex->right->delay=assignedVertex->weight_right;
                callBack(assignedVertex->right, activeDeque);
                idle=true;
            }else{
                // ELSE the child is active, push onto bottom of deque for execution
                //cout << "Light edge. " << endl;
                activeDeque->pushBottom(assignedVertex->right);
                idle=false;
            }
        }
        }
        // (3) call addResumedVertices()
        addResumedVertices();
        //cout << "Resumed all hung vertices... " << endl;
        // (4) THEN perform (2) for left node.
        if(assignedVertex!=NULL){
        if(assignedVertex->left!=NULL){
            //cout << "Assigned vertex has left child! " << assignedVertex->left << endl;
            // (2) THEN handle left node,
            if(assignedVertex->weight_left>1){
                // IF suspended increment suspension counter using handleChild() + install callBack
                //cout << "Heavy edge, child will spin for " << assignedVertex->weight_left << " msec. " << endl;
                activeDeque->suspendCtr++;
                assignedVertex->left->delay=assignedVertex->weight_left;
                callBack(assignedVertex->left, activeDeque);
                idle=true;
            }else{
                // ELSE the child is active, push onto bottom of deque for execution
                //cout << "Light edge. " << endl;
                activeDeque->pushBottom(assignedVertex->left);
                idle=false;
            }
        }
        }
        //Clear the currnet vertex, we've just serviced it.
        //assignedVertex=NULL; // THIS IS DONE IN MAIN TO ALLOW PRINTING EACH TICK
        //cout << "Child Handler Complete... printing current deque:" << endl;
        //activeDeque->print_deque();
    }
    void tick(){
        // if there's no active Vertex, start off idle.
        if (assignedVertex==NULL){
            this->idle=true;
        }
        // (1) Try to obtain work by popping vertex from bottom of active deque
        //cout << "Popping bottom of active deque... " << endl;
        Node* poppedVertex = NULL;
        if(activeDeque!=NULL){
            poppedVertex = activeDeque->popBottom();
        }
        //cout << "Popped vertex is: " << poppedVertex << endl;
        if(poppedVertex!=NULL){
            assignedVertex=poppedVertex;
            idle=false;
            this->completed_tasks++;
        }
        // If a worker has an assigned Vertex on a round
        //cout << "Assigned vertex is " << assignedVertex << endl;
        if(idle==false){ // there's work to do
            handle_both_children();
        }else{
            //cout << "Current deque not used... freeing deque." << endl;
            free(activeDeque);
            //cout << "Current vertex not ready!...Checking other Deques." << endl;
            // ELSE vertex not ready, check other deques for ready
            vertexDeque* tmpDeque=deque_list->find_ready();
            if(tmpDeque!=NULL){
                activeDeque=tmpDeque;
                // Changed deques, new deque should be popped and scheduled immediately.
                //cout << "Found another active Deques @ addr " << activeDeque << endl;
                idle=false;
                handle_both_children();
            }else{
                // IF no deques available, steal.
                //cout << "No active Deques available.\nStealing..." << endl;
                // choose random gDeque, try to steal top vertex
                int rand_deque =  rand() % gtotaldeques ;
                //cout << "Targetting victim deque[" << rand_deque << "]" << endl;
                assignedVertex=gDeques[rand_deque]->popTop();
                if(assignedVertex==NULL){
                    idle=true;
                    // Even though vertex is NULL, run handler to run resumed vertices
                    handle_both_children();
                    return;
                }
                //cout << "Retrived vertex " << assignedVertex << endl;
                idle=false;
                // setup new deque to handle new vertex
                activeDeque=newDeque();
                handle_both_children();
            }
        }
    }
    
    vertexDeque* newDeque(){
        vertexDeque* tmp_deque = emptyDeques->find_ready();
        if(tmp_deque==NULL){
            gtotaldeques++;
            return gDeques[gtotaldeques];
        }
        return tmp_deque;
    }
    
    void free(vertexDeque* q){
        if(q==NULL) return; // don't allow freeing of NULL vertexDeque
        if(q->popBottom()==NULL){
            emptyDeques->add(q);
//            q=NULL;
        }else{
            // recursively dump this deque
            free(q);
        }
    }
    
    void print_worker(){
        cout << "\tWorker " << this << endl ;
        cout << "\t\tWORK COMPLETED = " << this->completed_tasks << endl;
        cout << "\t\tIDLE = " << this->idle << endl;
        cout << "\t\tASSIGNED VERTEX = " << this->assignedVertex << endl;
        cout << "\t\tACTIVE DEQUE = " << this->activeDeque << endl;
        cout << "\t\tDEQUE LIST = ";  this->deque_list->print_dll(); cout << endl;
        ;
    }
    
    
};



#endif
