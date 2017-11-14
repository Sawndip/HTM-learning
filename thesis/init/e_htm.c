#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "e_lib.h"
#include "columnc.h"

int main(void){
	const char ShmName[] = "hello_Shm";
	char buf[256]={0};
	e_coreid_t coreid;
	e_memseg_t emem;
	unsigned my_row;
	unsigned my_col;
	char buffervalues[256];
	int i,j;
	struct column layers[16][16];
	int inp[16][16];
	float f=0.0;
//querying coreid from hardware
	coreid = e_get_coreid();
	e_coords_from_coreid(coreid, &my_row, &my_col);
	
	if(E_OK != e_shm_attach(&emem , ShmName)){
		return EXIT_FAILURE;
	}

//read input from shared buffer
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			e_read(&emem,0,0,0,buf,sizeof(buf));
			inp[i][j]=(int)(buf[0]-'0');
		}
	}

//initialize the column layer
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			setValue(&layers[i][j]);
		}
	}

//learn proximal connections between dendrites
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			if(inp[i][j]==1){
			
				//synapse gets connected
				layers[i][j].dendrite_prox=0.5;
				f=(float)rand()/(float)(RAND_MAX/0.5);	

				//neighbourhood synapses gets affected
				if(i-1>0 && layers[i-1][j].dendrite_prox==0.0){
					layers[i-1][j].dendrite_prox=f;
				}
				if(j-1>0 && layers[i][j-1].dendrite_prox==0.0){
					layers[i][j-1].dendrite_prox=f;
				}	
				if(i+1<16 && layers[i+1][j].dendrite_prox==0.0){
					layers[i+1][j].dendrite_prox=f;
				}
				if(j+1<16 && layers[i][j+1].dendrite_prox==0.0){
					layers[i][j+1].dendrite_prox=f;
				}
			}
		}
	}

// write to the shared buffer to check output

	return EXIT_SUCCESS;
}
