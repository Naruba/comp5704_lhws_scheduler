#ifndef treebuilder_cpp

#define treebuilder_cpp

#include <iostream>

#include "treebuilder.hpp"


using namespace std;

int counter=1;
int num_leafs=8;
int map_delay=50;

Node* generateRoot(int mapdelay){
    map_delay=mapdelay;
    counter=1;
    Node* root = new Node;
    root->data=1;
    root->left=NULL;
    root->right=NULL;
    
    return root;
}

void insertNode(bool left, int weight, Node* parent){
    Node* node = new Node;
    counter=counter+1;
    node->parent=parent;
    node->data=counter;
    node->done=false;
    node->left=NULL;
    node->right=NULL;
    
    if(left){
        parent->left=node;
        parent->weight_left=weight;
    }else{
        parent->right=node;
        parent->weight_right=weight;
    }
    
    if(counter>=num_leafs){//at edge of tree, finishing mapping
        if(counter%2==0){//if even edge, this will perform reduction
            node->weight_left=map_delay;
            
            Node* reduction_nodes[(int)log2(num_leafs)];
            reduction_nodes[0]=new Node;
            node->left=reduction_nodes[0];
            reduction_nodes[0]->data=node->data*-10;
            reduction_nodes[0]->right=NULL;
            
            for(int i=1;i<log2(num_leafs);i=i+1){
                reduction_nodes[i]=new Node;
                reduction_nodes[i-1]->left=reduction_nodes[i];
                reduction_nodes[i]->data=reduction_nodes[i-1]->data-1;
                reduction_nodes[i]->right=NULL;
                reduction_nodes[i-1]->weight_left=1;
            }
            reduction_nodes[(int)log2(num_leafs)-1]->left=NULL;
        }
    }
    
    return;
}

void printNode(Node* node){
    if(node==NULL)return;
    cout << "[" << node->data << "]" << endl;
}

void printTree(Node* node){
    printNode(node);
    if(node->left!=NULL){
        printTree(node->left);
    }
    if(node->right!=NULL){
        printTree(node->right); 
    }
    return;
}

void dumpTree(Node* node){
    if(node->left!=NULL){
        cout << node->data << "->" << node->left->data;
        if(node->weight_left>1){
           cout <<  "[label=\""<<node->weight_left<<"\"];";
        }
        cout << endl;
        dumpTree(node->left);
    }
    if(node->right!=NULL){
        cout << node->data << "->" << node->right->data;
        if(node->weight_right>1){
            cout << "[label=\""<<node->weight_right<<"\"];";
        }
        cout << endl;
        dumpTree(node->right);
    }
    return;
}

void buildLevel(Node* node, float prob_he, float prob_lf, float prob_br){
        double val_he1=((double) rand() / (RAND_MAX)) ;
        double val_he2=((double) rand() / (RAND_MAX)) ;
        double val_lf=((double) rand() / (RAND_MAX)) ;
        double val_br=((double) rand() / (RAND_MAX)) ;
        bool he1= val_he1 < prob_he;
        bool he2= val_he2 < prob_he;
        bool lf= val_lf < prob_lf;
        bool br= val_br < prob_br;
        int edge_weight1 = he1 ? rand() % 10 + 1 : 1;
        int edge_weight2 = he2 ? rand() % 10 + 1 : 1;
        if(counter<=3){// allow tree root-area to be dense
            br=true;
        }
        if(br){
            if(node->left==NULL)insertNode(true,edge_weight1,node);
            if(node->right==NULL)insertNode(false,edge_weight2,node);
        }else{
            if(lf){
                return;
            }else{
                if(((double) rand() / (RAND_MAX)) > 0.5){
                    if(node->left==NULL)insertNode(true,edge_weight1,node);
                }else{
                    if(node->right==NULL)insertNode(false,edge_weight2,node);
                }
            }
        }   
}

void buildTree(int num_nodes, Node* node, float prob_he, float prob_lf, float prob_br){
    if(counter>=num_nodes)return;// done
    if(node->left==NULL){
        buildLevel(node, prob_he, prob_lf, prob_br);
    }else if(node->right==NULL){
        buildLevel(node, prob_he, prob_lf, prob_br);
    }else{
        buildTree(num_nodes, node->left, prob_he, prob_lf, prob_br);
        buildTree(num_nodes, node->right, prob_he, prob_lf, prob_br);
    }
}

void newTree(Node* root, int total_nodes, float prob_he, float prob_lf, float prob_br){
    counter=1;
    root->data=1;
    buildLevel(root, prob_he, prob_lf, prob_br);
    while(counter<total_nodes)buildTree(total_nodes, root, prob_he, prob_lf, prob_br);
}


Node* find_leaf(Node* root, int cur_stage, int num_tasks, int tgt_leaf_num){
    if(cur_stage==log2(num_tasks+2)-1){//at leaf-level
        if(tgt_leaf_num%2==0){//even leaf
            return root->right;
        }else{//odd leaf
            return root->left;
        }
    }else{//keep traversing}
        if(tgt_leaf_num<=(num_tasks+2)/(cur_stage+1)){//take left
            find_leaf(root->left,cur_stage+1,num_tasks,tgt_leaf_num);
        }else{//take right
            find_leaf(root->right,cur_stage+1,num_tasks,tgt_leaf_num);
        }
    }
    return NULL;
}

void merge_map_reduce(Node* start_root, Node* end_root, int num_tasks, int comp_delay){
    int stages=log2(num_tasks+2);
    for(int i=pow(2,stages-1);i<=2*pow(2,stages-1);i=i+1){
        Node* start_root_leaf = find_leaf(start_root,1,num_tasks,i);
        Node* end_root_leaf = find_leaf(end_root,1,num_tasks,i);
        //tie leafs with heavy edge
        start_root_leaf->left=end_root_leaf;
        start_root_leaf->weight_left=comp_delay;
    }
}

#endif
