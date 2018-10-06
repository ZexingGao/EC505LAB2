// Copyright 2018 Zexing Gao zexing@bu.edu
// The goal is to implement a splay tree data base ...
/*
Source reference:
The code is adopted from: https://www.geeksforgeeks.org/splay-tree-set-1-insert/
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* need this for string manipulation */
#include <time.h>   /* need this for simple timers */

#define NIL NULL

// This is used to disable silly errors with standard library functions in visual studio
#pragma warning(disable:4996)

clock_t clock(void);

static char tab = '\t';
static char cr = '\r';
static char lf = '\n';
static char es = '\0';
FILE *fp_out;
//
///* Type definitions */
//
typedef struct Node Data_Node;
struct Node {
	int code;
	char zip[12];
	char city[150];
	char state[6];
};

typedef struct tree_node Tree;
struct tree_node {
	struct tree_node *left, *right;
	struct tree_node *parent;
	struct Node *data;
};

int size;  /* number of nodes in the tree */
		   /* Not actually needed for any of the operations,
		   but will be used in output */

/*
                y                                     x
               / \     Zig (Right Rotation)          /  \
              x   T3   – - – - – - – - - ->         T1   y 
             / \       < - - - - - - - - -              / \
            T1  T2     Zag (Left Rotation)            T2   T3

*/

 
/* Helper function that allocates a new node with the given key and 
    NULL left and right pointers. */
Tree *newTree(Data_Node *data) 
{ 
    Tree* tree = (Tree*)malloc(sizeof(Tree)); 
    tree->data  = data;
    tree->parent = NULL;
    tree->left  = tree->right  = NULL; 
    return (tree); 
} 

// A utility function to right rotate subtree rooted with y 
// See the diagram given above. 
Tree *zig(Tree *x) 
{ 
    Tree *y = x->left; 
    x->left = y->right; 
    y->right = x; 
    return y; 
} 
  
// A utility function to left rotate subtree rooted with x 
Tree *zag(Tree *x) 
{ 
    Tree *y = x->right; 
    x->right = y->left; 
    y->left = x; 
    return y; 
} 
  

/* WRITE A SPLAY FUNCTioN */
Tree * splay(int code, Tree *t) {
	/* Splay, not requiring code to be in the tree t.  
	   Returns a pointer to the t of the resulting tree*/

	// Base cases: t is NULL or code is present at t 
    if (t == NULL || t->data->code == code) 
        return t; 
  
    // data->code lies in left subtree 
    if (t->data->code > code) 
    { 
        // data->code is not in tree, we are done 
        if (t->left == NULL) return t; 
  
        // Zig-Zig (Left Left) t
        if (t->left->data->code > code) 
        { 
            // First recursively bring the data->code as t of left-left 
            t->left->left = splay(code ,t->left->left); 
  
            // Do first rotation for t, second rotation is done after else 
            t = zig(t); 
        } 
        else if (t->left->data->code < code) // Zig-Zag (Left Right) 
        { 
            // First recursively bring the code as t of left-right 
            t->left->right = splay(code ,t->left->right); 
  
            // Do first rotation for t->left 
            if (t->left->right != NULL) 
                t->left = zag(t->left); 
        } 
  
        // Do second rotation for t 
        return (t->left == NULL)? t: zig(t); 
    } 
    else // data->code lies in right subtree 
    { 
        // data->code is not in tree, we are done 
        if (t->right == NULL) return t; 
  
        // Zag-Zig (Right Left) 
        if (t->right->data->code > code) 
        { 
            // Bring the data->code as t of right-left 
            t->right->left = splay(code, t->right->left); 
  
            // Do first rotation for t->right 
            if (t->right->left != NULL) 
                t->right = zig(t->right); 
        } 
        else if (t->right->data->code < code)// Zag-Zag (Right Right) 
        { 
            // Bring the data->code as t of right-right and do first rotation 
            t->right->right = splay(code, t->right->right); 
            t = zag(t); 
        } 
  
        // Do second rotation for t 
        return (t->right == NULL)? t: zag(t); 
    } 

    return t;
} 


/*  WRITE A SPLAY TREE INSERT FUNCTION */
Tree * insert(Data_Node *data, Tree *t) {
	/* Insert data into the tree t, unless it's already there.    */
	/* Return a pointer to the resulting tree.                 
	   Keep track of the total number of records in the tree in the global
	   variable size */

	// Simple Case: If tree is empty 
    if (t == NULL) {
    	size = size + 1;
    	return newTree(data); 
    }
    	
  
    // Bring the closest leaf node to t 
    t = splay(data->code, t); 
  
    // If data->code is already present, then return 
    if (t->data->code == data->code) {
    	//size = size++;
		return t; 
    }
    	
    // Otherwise allocate memory for new node 
    Tree *newnode  = newTree(data); 
  
    // If t's data->code is greater, make t as right child 
    // of newnode and copy the left child of t to newnode 
    if (t->data->code > data->code) 
    { 
        newnode->right = t; 
        newnode->left = t->left; 
        t->left = NULL; 
        size = size + 1;
    } 
  
    // If t's key is smaller, make t as left child 
    // of newnode and copy the right child of t to newnode 
    else
    { 
        newnode->left = t; 
        newnode->right = t->right; 
        t->right = NULL; 
        size = size + 1;
    } 
  	
    return newnode; // newnode becomes new t 	
}


/*  WRITE A SPLAY TREE DELETE FUNCTION */
Tree * delete(int code, Tree * t) {
	/* Deletes record with key 'code' from the tree if it's there. */
	/* Return a pointer to the resulting tree.   
	   Keep track of the total number of records in the tree in the 
	   variable size */
	Tree *temp; 
    if (!t) 
        return NULL; 
      
    // Splay the given key     
    t = splay(code, t); 
      
    // If key is not present, then 
    // return t 
    if (code != t->data->code) {

    	//size = size--;
		return t; 
    }
        

          
    // If key is present 
    // If left child of t does not exist 
    // make t->right as t    
    if (!t->left) 
    { 
        temp = t; 
        t = t->right; 
        size = size - 1;
    } 
      
    // Else if left child exits 
    else
    { 
        temp = t; 
  
        /*Note: Since key == t->key, 
        so after Splay(key, t->lchild), 
        the tree we get will have no right child tree 
        and maximum node in left subtree will get splayed*/
        // New t 
        t = splay(code, t->left); 
          
        // Make right child of previous t  as 
        // new t's right child 
        t->right = temp->right; 
        size = size - 1;
    } 
      
    // free the previous t node, that is, 
    // the node containing the key 
    free(temp); 
     
    // return t of the new Splay Tree 
    return t; 
                     
}

/* function searchTree()
This function is used to search through tree and print out the zipcodes
that correspond to |zip(code)-z(i)| <= 10, where z(i) is the key value
that has been passed into the function.  The city, state, and zip is then
printed out.
*/

/* DO NOT CHANGE*/
void searchTree2(Tree *node, int key) { 
	int test_code;
	if (node != NULL) {
		test_code = node->data->code;
		if ((key - test_code) < 10) searchTree2(node->left, key);
		if ((((test_code - key) <= 10) && ((test_code - key) >= 0))
			|| ((key - test_code) <= 10) && ((key - test_code) >= 0))
			fprintf(fp_out,"%s %s %s\n", node->data->city, node->data->state, node->data->zip);
		if ((test_code - key) < 10) searchTree2(node->right, key);
	}
}


/* function treeHeightht: computes the height of the root.  DO NOT CHANGE!
	*/
int treeHeight(Tree *node) {
	int left_height, right_height;
	if (node->left == NULL) {
		if (node->right == NULL) { // it's a leaf
			return 0;
		}
		else {
			return 1 + treeHeight(node->right);
		}
	}
	else {
		left_height = 1 + treeHeight(node->left);
		if (node->right == NULL) {
			return left_height;
		}
		else {
			right_height = 1 + treeHeight(node->right);
			if (right_height > left_height) {
				return right_height;
			}
			else return left_height;
		}
	}
}
//
//
//

/* DO NOT CHANGE THE MAIN FUNCTION */

int main( int argc, char *argv[] ) {
	/* A sample use of these functions.  Start with the empty tree,         */
	/* insert some stuff into it, and then delete it                        */
	Tree *root;
	int i;
	int height;
	root = NULL;              /* the empty tree */
	size = 0;
	clock_t start, end;
	double time;
	int index1, index2, count;
	FILE *fp;
	char list[1000];
	char city[150];
	char state[3];
	char zip[11];
	char a;
	int qcode;
	int z[10];
	int numCities=0;
	int countCities=0;
	int nqueries = 0;
	char outname[100] = "";

	Data_Node *datanode;


	start = clock();
	fp = fopen(argv[1], "r");

	strcpy(outname, argv[1]);
	strcat(outname, "_out");

	fp_out = fopen(outname, "w");

	count = 0;
	if(fp == NULL){
		printf("could not open file\n");
		return 0;
	}
	while (fgets(list, 999, fp)) { /* this continues until end of records*/
		if(numCities==0){
			numCities=atoi(list); /* first line of input_file tells you how many entries to insert*/
		}
		else if(countCities < numCities){ /* read line and build a splay tree */
			strcpy(city, ""); strcpy(state, ""); strcpy(zip, ""); /* clear old info */
			index1 = 0;
			index2 = 0;

			a = list[index1]; /* copy city name until first tab */
			while ((a != cr) && (a != lf)) {
				while ((a != tab) && (a != cr) && (a != lf)) {
					city[index2++] = a;
					a = list[++index1];
				}
				city[index2] = es;
				index2 = 0; a = list[++index1]; /* copy state name until next tab */
				while ((a != tab) && (a != cr) && (a != lf)) {
					state[index2++] = a;
					a = list[++index1];
				}
				state[index2] = es;
				index2 = 0; a = list[++index1];
				/* copy zip until linefeed or carriage return */
				while ((a != tab) && (a != cr) && (a != lf)) {
					zip[index2++] = a;
					a = list[++index1];
				}
				zip[index2] = es;
			}
			datanode = (Data_Node *)malloc(sizeof(Data_Node));

			sscanf(zip, "%d", &qcode);
			datanode->code = qcode;
			strcpy(datanode->city, city);
			strcpy(datanode->state, state);
			strcpy(datanode->zip, zip);

			/******************* INSERT **********************/
			root = insert(datanode, root);/* store dat anode into Splay tree.  */
			count++;
			countCities++;
			if(countCities == numCities){
				end = clock();
				time = (double)(end - start) / CLOCKS_PER_SEC;
				fprintf(fp_out,"%f\n", (float)time);
			
				height = treeHeight(root);
				/*************************** HEIGHT OF SPLAY TREE ******************/
				//fprintf(fp_out,"Height of resulting tree: %d \n", height);
				i = 0;
			}
		}
		else if(countCities == numCities){ /* Entries (for insertion) are followed by queries */
			/* read "queries.txt" into an array, then compare all of the zipcodes
			   in "queries.txt" with the zipcodes of the tree, and print out the
			   zipcodes where |zip(code)- z(i)| <= 10
			*/
			qcode = atoi(list);
			z[i] = qcode;
			i++;
		}
	}/* end while fgets */

	fclose(fp);
	nqueries = i;
	start = clock();
	for (i = 0; i < nqueries; i++) {
		fprintf(fp_out,"\n\nCities within 10 of %d:\n", z[i]);
		/*********************** SEARCH ********************/
		searchTree2(root, z[i]);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;

	/********** SIZE OF TREE BEFORE DELETION ********************/
	fprintf(fp_out,"size = %d\n", size);

	for (i = 0; i < nqueries; i++) {
		root = delete(z[i],root);
	}
	/********** SIZE OF TREE AFTER DELETION *******************/
	fprintf(fp_out,"size = %d\n", size);
	//fprintf(fp_out, "Height of final display tree: %d \n", treeHeight(root));
	fclose(fp_out);
	return 0;
}

