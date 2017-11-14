#include<iostream>
#include "column.h"
#include<vector>

using namespace std;

class region{
	bool *input;
	int isize;
	int len_cols,bre_rows;
	column **layer; //create a 2D array of column (dynamic)
	vector<float> permanence;
	vector<int> x1,x2,y1,y2,z1,z2;	
public:
	region(int bre,int len, bool *inp,int inp_size){
		bre_rows=bre;
		len_cols=len;
		layer=new column*[bre_rows];
		for(int i=0;i<bre_rows;i++){
			layer[i]=new column[len_cols];	
		}
		for(int i=0;i<bre_rows;i++){
			for(int j=0;j<len_cols;j++){
				layer[i][j].setValue(3);
			}
		}
		isize=inp_size;
		input=new bool[inp_size];
		for(int i=0;i<inp_size;i++){
			input[i]=inp[i];
		}
	}
	void initialize_distal(){
		//initialize permanence array and position array
		permanence.push_back(0.5);
		x1.push_back(0);
		y1.push_back(0);
		z1.push_back(0);
		x2.push_back(1);
		y2.push_back(1);
		z2.push_back(1);
	}
	void print_state(){
		cout<<"input array initialized"<<endl;
		for(int i=0;i<isize;i++){
			cout<<input[i]<<endl;
		}
		cout<<"column layer initialized..."<<endl;
		/*for(int i=0;i<bre_rows;i++){
			for(int j=0;j<len_cols;j++){
				layer[i][j].print_state();
			}
		}*/
		cout<<"distal connections in the region"<<endl;
		for(int i=0;i<permanence.size();i++){
			cout<<"("<<x1[i]<<","<<y1[i]<<","<<z1[i]<<")";
			cout<<"-->"<<"("<<x2[i]<<","<<y2[i]<<","<<z2[i]<<")";
			cout<<" "<<permanence[i]<<endl;
		}
	}
};
