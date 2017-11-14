#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "e_lib.h"
#include "columnc.h"

const unsigned  ShmSize=256;
const unsigned minOverlap=1;
const unsigned boost=2;

int main(void){
	const char ShmName[] = "hello_Shm";
	char buf[257]={0};
	e_coreid_t coreid;
	e_memseg_t emem;
	unsigned my_row;
	unsigned my_col;
	char buffervalues[256];
	int i,j,k;
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
	/*for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			e_read(&emem,0,0,0,buf,sizeof(buf));
			inp[i][j]=(int)(buf[0]-'0');
		}
	}*/
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			if(i==j){
				inp[i][j]=1;
			}
			else{
				inp[i][j]=0;
			}
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
				layers[i][j].dendrite_prox[0]=0.5;
				layers[i][j].dendrite_prox[1]=0.5;
				layers[i][j].dendrite_prox[2]=0.5;
				f=0.3+(float)rand()/(float)(RAND_MAX/0.5);	
				//f+=0.3;
				//neighbourhood synapses gets affected
			if(i-1>0 && layers[i-1][j].dendrite_prox[0]==0.0){
				//f=0.3+(float)rand()/(float)(RAND_MAX/0.5);
					layers[i-1][j].dendrite_prox[0]=f;
					layers[i-1][j].dendrite_prox[1]=f;
					layers[i-1][j].dendrite_prox[2]=f;
				}
			if(j-1>0 && layers[i][j-1].dendrite_prox[0]==0.0){
				//f=0.3+(float)rand()/(float)(RAND_MAX/0.5);
					layers[i][j-1].dendrite_prox[0]=f;
					layers[i][j-1].dendrite_prox[1]=f;
					layers[i][j-1].dendrite_prox[2]=f;
				}	
			if(i+1<16 && layers[i+1][j].dendrite_prox[0]==0.0){
				//f=0.3+(float)rand()/(float)(RAND_MAX/0.5);
					layers[i+1][j].dendrite_prox[0]=f;
					layers[i+1][j].dendrite_prox[1]=f;
					layers[i+1][j].dendrite_prox[2]=f;
				}
			if(j+1<16 && layers[i][j+1].dendrite_prox[0]==0.0){
				//f=0.3+ (float)rand()/(float)(RAND_MAX/0.5);
					layers[i][j+1].dendrite_prox[0]=f;
					layers[i][j+1].dendrite_prox[1]=f;
					layers[i][j+1].dendrite_prox[2]=f;
				}
			}
		}
	}

//step 1:- calculate the overlap of input with the columns
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			setOverlap(&layers[i][j]);
			for(k=0;k<3;k++){
	layers[i][j].overlap+=isConnected(layers[i][j].dendrite_prox[k]);
			}
			if (layers[i][j].overlap<minOverlap){
				layers[i][j].overlap=0;
			}
			else{
				layers[i][j].overlap*=boost;
			}
		}
	}

// write to the shared buffer to check input
	k=0;
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			buf[k]=layers[i][j].overlap+'0';
			//buf[k+1]='\0';
			k++;
			//e_write((void *)&emem,buf,my_row,my_col,NULL,strlen(buf)+1);
			//e_write(&emem,0,0,0,buf,sizeof(buf));
		}
	}
	buf[256]='\0';
	e_write((void *)&emem,buf,my_row,my_col,NULL,strlen(buf)+1);
	return EXIT_SUCCESS;
}
