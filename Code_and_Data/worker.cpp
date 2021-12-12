#ifndef worker_cpp
#define worker_cpp

#include "worker.hpp"

extern int IS_PARALLEL;


void resumeVertices(vertexDeque* resumed_deque){
    // for each resumed vertex
    if(resumed_deque==NULL)return;
    if(resumed_deque->resumedVertices==NULL)return;
    int resume_vertex_count=(int)resumed_deque->resumedVertices->q.size();
    //cout << "Resumed deque contains " <<resume_vertex_count << " resumed vertices. " << endl;
    for(int j=0;j<resume_vertex_count;j=j+1){
        // push vertices onto bottom of active deque
        Node* resumed_vertex = resumed_deque->resumedVertices->popTop();
        //cout << "Pushing vertex " << resumed_vertex << " onto bottom of active deque." << endl;
        resumed_deque->pushBottom(resumed_vertex);
    }
}

void addResumedVertices(){
    // Scan through resumedDeques to enable previously suspended vertices
    //cout << "addResumedVertices()... " << endl;
    //cout << "resumeDeques size = " <<resumedDeques.size()<<endl;
    int resume_deque_count=(int)resumedDeques.size();
    for(int i=0;i<resume_deque_count;i=i+1){
        // fetch front
        vertexDeque* tmp_deque = resumedDeques.front();
        //cout << "Popped " << tmp_deque << " resumed deque... " << endl;
        // pop out front
        resumedDeques.pop_front();
        // spawn threads to execute all vertices in parallel
        if(IS_PARALLEL==1){
            std::thread t(resumeVertices, tmp_deque);
            t.detach();
        }else{
            resumeVertices(tmp_deque);
        }
    }
}



void spin(Node* v, vertexDeque* q){
    if(IS_PARALLEL==1){
        //cout << "Launched thread " << std::this_thread::get_id() << endl;
    }
    int delay = v->delay;
    //cout << "Spinning... " << endl;
    std::this_thread::sleep_for (std::chrono::microseconds(delay*1));
    //cout << "Spinning for " << delay << "ms done!" << endl;
    //cout << "PUSH BOTTOM FOR q " << q << " resumedVerticies " << q->resumedVertices << endl;
    q->resumedVertices->pushBottom(v);
    q->suspendCtr--;
    //cout << "Suspension counter in q " << q << "=" << q->suspendCtr << endl;
    if(q->resumedVertices->size>0){
        //cout << "Pushing q onto resumedDeques... " << endl;
        resumedDeques.push_front(q);
        //cout << "Success!" << endl;
    }
}

void callBack(Node* v, vertexDeque* q){
    if (1==1/*IS_PARALLEL==1*/){
        std::thread t(spin, v, q);
        t.detach();
        //cout << "Detached thread. " << endl;
    }else{
        spin(v, q);   
    }
}

#endif
