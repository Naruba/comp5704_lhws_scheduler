#ifndef deque_linked_list_hpp
#define deque_linked_list_hpp

#include "vertexDeque.hpp"

struct list_node{
    list_node* next;
    list_node* prev;
    vertexDeque* node;
};

class deque_linked_list{
public:
    list_node* first_node;
    list_node* final_node;
    void add(vertexDeque* q){
        list_node* new_node = new list_node;
        new_node->next=NULL;
        new_node->node=q;
        if(first_node==NULL){
            first_node=new_node;
            final_node=new_node;
        }else{
            new_node->prev=final_node;
            final_node->next=new_node;
        }
    }
    void remove(vertexDeque* q){
        list_node* del_node = find(first_node, q);
        if(del_node->next!=NULL){
            // There is something after this deleted node
            if(del_node->prev!=NULL){
                // There is something before this deleted node
                del_node->prev->next=del_node->next;
                del_node->next->prev=del_node->prev;
            }else{
                // Deleted node is first node
                del_node->next->prev=NULL;
                first_node=del_node->next;
            }
        }else{
            // There is nothing after this deleted node
            // Deleted node is final node
            if(del_node->prev!=NULL){
                // There is something before this deleted node
                del_node->prev->next=NULL;
                final_node=del_node->prev;
            }else{
                // Deleted node is first node
                first_node=NULL;
            }
        }
    }
    vertexDeque* find_ready(){
        list_node* current_node = first_node;
        while(current_node!=NULL){
            if (current_node->node->ready==true){
                vertexDeque*  ret=current_node->node;
                return ret;
            }else{
                current_node=current_node->next;
            }
        }
        return NULL;
    }
    void print_dll(){
        list_node* current_node = first_node;
        while(current_node!=NULL){
            current_node->node->print_deque();
                current_node=current_node->next;
        }
    }
private:
    list_node* find(list_node* curr_node, vertexDeque* target){
        if(curr_node->node==target) return curr_node;
        return find(curr_node->next, target);
    }
};


#endif
