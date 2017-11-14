#include<iostream>
#include<stdio.h>
#include<vector>

using namespace std;

class dendrite {
private:
	int pot_synapse_x[10];
	int pot_synapse_y[10];
	float permanence[10];
public:
	void init_pot_syn(){}	//can be constructor
	void update_perm(){} //can be used during learning
	void form_synapse(){} //implement the perm*pot_syn function
};

class cort_column{
private:
	int inpbits[10];
	int cell_states[3];
	//distal_dendrite cells[3];
	dendrite proximal;
	int state;
public:
	//write constructor
	int get_state(){return state;}
};

class region{
private:
	int inpbits[10];
	cort_column layer[10][10]; //2D rep of columns
	vector <region *> children;
public:
	void set_col_inp(){}; //can be constructor
	void inhibit(){}; //implement the logic to only activate sparse cols
	cort_column** get_state(){return layer;} //returns the sparse rep
};

region root; 

class HTM{
public:
	HTM(){root = NULL;} //HTM modelled as tree using child vector
	//write function for insert
	//function for delete
	//function for traversal
};

int main(){
//initialize the HTM object
return 0;
}
