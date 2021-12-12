#ifndef treebuilder_hpp

#define treebuilder_hpp

#include <stdio.h>
#include <vector>
#include <math.h>

class Node{
    public:
        int data;
        Node* left;
        Node* right;
        Node* parent;
        int weight_left;
        int weight_right;
        bool done;
        bool suspended;
        int delay;
        int ctr;
        long q_addr; 
        bool isSuspended(){
            bool suspended=delay==ctr;

            return suspended;
        }
};

void insertNode();
void printNode();
void dumpTree(Node* node);
void buildLevel();
void buildTree();
void newTree(Node* root, int total_nodes, float prob_he, float prob_lf, float prob_br);
Node* generateRoot(int mapdelay);
void merge_map_reduce(Node* root1, Node* root2, int num_tasks, int comp_delay);
#endif
