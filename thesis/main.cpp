#include<iostream>
#include "region.h"
#include<stdio.h>

using namespace std;

int main(){
	bool* inp=new bool[4];
	for(int i=0;i<4;i++){
		inp[i]=true;
	}
	region r1(2,1, inp,4);
	r1.initialize_distal();
	r1.print_state();
	return 0;
}
