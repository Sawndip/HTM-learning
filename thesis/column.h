#include<iostream>
#include<stdio.h>
using namespace std;

class column{
	int col_size;
	int *cell_state;
public:
	column(){}
	column(int size){
		col_size=size;
		cell_state=new int[col_size];
		for(int i=0;i<col_size;i++){
			cell_state[i]=0;
		}
	}
	void setValue(int size){
		col_size=size;
		cell_state=new int[col_size];
		for(int i=0;i<col_size;i++){
			cell_state[i]=0;
		}
	}
	void print_state(){
		cout<<"building column state successful"<<endl;
		for(int i=0;i<col_size;i++){
			cout<<cell_state[i]<<endl;
		}
	}	
};
