#ifndef vertexdeque_hpp
#define vertexdeque_hpp

#include <deque>
#include "treebuilder.hpp"
#include <iostream>

using namespace std;

class vertexDeque{
    public: 
        std::atomic<int> suspendCtr;
        std::atomic<int> size;
        vertexDeque* resumedVertices;
        bool ready;
        std::deque <Node*> q;
    void print_deque(){
        for(int i=0;i<q.size();i=i+1){
            cout << "\t\t";
            cout << q[i] << " " ;
        }
        cout << endl;
    }
        Node* popTop(){
            int q_size=(int)q.size();
            if(q_size==0){
                return NULL;
            }
            Node* ret_q=q.front();
            q.pop_front();
            if(size>0)size--;
            update_state();
            return ret_q;
        }
        Node* popBottom(){
            //cout << "Executing popBottom()  on Deque " << &q << " ... " << endl;
            //cout << "Previous deque contents:" << endl;
            //this->print_deque();
            int q_size=(int)q.size();
            if(q_size==0){
                return NULL;
            }
            Node* ret_q=q.back();
            //cout << "Fetched back of deque value: " << ret_q << endl;
            //cout << "Pop back of deque... " << endl;
            q.pop_back();
            //cout << "Success!" << endl;
            //cout << "Popped. New dequent contents: " << endl;
            //this->print_deque();
            if(size>0)size--;
            update_state();
            return ret_q;
        }
        void pushBottom(Node* v){
            //cout << "Pushing vertex " << v << endl;
            //cout << "Previous deque contents:" << endl;
            //this->print_deque();
            q.push_back(v);
            //cout << "Pushed. New dequent contents: " << endl;
            //this->print_deque();
            size++;
            update_state();
        }
    private:
        void update_state(){
            if(q.size()>0){
                ready=true;
            }else{
                ready=false;
            }
        }
};

//vertexDeque* newDeque();
//randomDequevertexDeque* randomDeque();
//void printDeque(deque <Node*> q);

#endif
