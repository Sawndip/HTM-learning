#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "e_lib.h"
#include "columnc.h"

const unsigned  ShmSize=256;
const unsigned minOverlap=3;
const unsigned boost=2;
const unsigned desiredLocalActivity=2;

int kthScore(struct column layer[16][16],int row,int col,int desiredLocalActivity){
	//choose the desiredLocalActivity-th highest value from list of neighbouring columns overlap values
	//here choose the 2nd greatest value from neighbouring overlap values and all the columns above this overlap score will be active -here two columns willbe active. Hence desiredLocalActivity controls the number of columns active
	int k,max,pos,neighbours[]={0,0,0,0};
	if(row-1>0)
	neighbours[0]=layer[row-1][col].overlap;
	if(col-1>0)
	neighbours[1]=layer[row][col-1].overlap;
	if(row+1<16)
	neighbours[2]=layer[row+1][col].overlap;
	if(row+1<16)
	neighbours[3]=layer[row][col+1].overlap;
	//logic for choosing the nth highest number from a array
	while(desiredLocalActivity>0){
		max=0,pos=0;
		for(k=0;k<4;k++){
			if(neighbours[k]>max){
				max=neighbours[k];
				pos=k;
			}
		}
		neighbours[pos]=0;
		desiredLocalActivity--;		
	}
	return max;
}

int main(void){
	const char ShmName[] = "hello_Shm";
	char buf[256]={0};
	e_coreid_t coreid;
	e_memseg_t emem;
	unsigned my_row;
	unsigned my_col;
	char buffervalues[256];
	int i,j,k,cnt;
	struct column layers[16][16];
	int inp[16][16],minLocalActivity;
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
				//f=0.3+(float)rand()/(float)(RAND_MAX/0.5);
					layers[i][j+1].dendrite_prox[0]=f;
					layers[i][j+1].dendrite_prox[1]=f;
					layers[i][j+1].dendrite_prox[2]=f;
				}
			}
		}
	}

//phase 1:- calculate the overlap of input with the columns
	cnt=0;
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			setOverlap(&layers[i][j]);
			for(k=0;k<3;k++){
	layers[i][j].overlap+=isConnected(layers[i][j].dendrite_prox[k]);	
			}
			if (layers[i][j].overlap<minOverlap){
				//buf[cnt]=layers[i][j].overlap+'0';
				layers[i][j].overlap=0;
			}
			else{
				layers[i][j].overlap*=boost;
			}
		}
	}

//phase 2:- calculate column winners after inhibition
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			minLocalActivity=kthScore(layers,i,j,desiredLocalActivity);
	if(layers[i][j].overlap>0 && layers[i][j].overlap>=minLocalActivity)
			layers[i][j].state=1;			
		}
	}	//--> end of sparse distributed representation of input

// write to the shared buffer to check input
	k=0;
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			buf[k]=layers[i][j].state+'0';
			//buf[k+1]='\0';
			k++;
			//e_write((void *)&emem,buf,my_row,my_col,NULL,strlen(buf)+1);
			//e_write(&emem,0,0,0,buf,sizeof(buf));
		}
	}
	buf[256]='\0';
	e_write((void *)&emem,buf,my_row,my_col,NULL,strlen(buf)+1);
	//e_write((void *)&emem,buf,my_row,my_col,NULL,strlen(buf)+1);
	return EXIT_SUCCESS;
}
