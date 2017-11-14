#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "e_lib.h"
#include "columnc.h"

const char ShmName[]="hello_Shm";
unsigned const lock =0x00000000;
unsigned const unlock=0x00000001;
unsigned const OUTPUT_WAIT= 3;
unsigned const OUTPUT_READY = 2;
unsigned const INPUT_READY =1;
unsigned const INPUT_WAIT =0;

const unsigned minOverlap =3;
const unsigned desiredLocalActivity =2;

int kthscore(struct column layer[16][16],int row,int col,int desiredLocalActivity){
	/*choose the desiredLocalActivity-th highest value from the list of neighbouring columns overlap values
	here choose the 2nd greatest value from the neighbouring overlap values and all the columns above this overlap score will be active-Hence desiredLocalActivity controls the numer of columns active*/
	int k,max,pos,neighbours[]={0,0,0,0};
	if(row-1>0)
		neighbours[0]=layer[row-1][col].overlap;
	if(col-1>0)
		neighbours[1]=layer[row][col-1].overlap;
	if(row+1<16)
		neighbours[2]=layer[row+1][col].overlap;
	if(col+1<16)
		neighbours[3]=layer[row][col+1].overlap;

	//logic for choosing the nth highest number from a array
	while(desiredLocalActivity>0){
		max=0;pos=0;
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
	e_coreid_t coreid;
	e_memseg_t emem;
	
	char * receivedinput;
	char * sentoutput;
	unsigned i,j,k,cnt;
	unsigned * inpflag,*outflag, epi_row,epi_col;
	struct column layers[16][16];
	int inp[16][16],r,c,minLocalActivity;
	unsigned *state;
	int activebits;
	
//allocating the memory locations to different shared resources in agreement with the arm core 
	receivedinput = (char *) 0x2000;
	sentoutput = (char *) 0x5000;
	inpflag = (unsigned *)0x7000;
	outflag = (unsigned *)0x6000;
	state = (unsigned *)0x4000;

	coreid = e_get_coreid();
	e_coords_from_coreid(coreid,&epi_row,&epi_col);
	if(E_OK!= e_shm_attach(&emem, ShmName)){
		return EXIT_FAILURE;
	}

	while((*(state))==INPUT_WAIT);
	(*(state))=OUTPUT_WAIT;

//receive the input from arm core		
		k=0;
		for(i=0;i<16;i++){
			for(j=0;j<16;j++){
				inp[i][j]=(*(receivedinput+k))-'0';
				k++;		
			}
		}

//initialize the column layer
		for(i=0;i<16;i++){
			for(j=0;j<16;j++){
				setValue(&layers[i][j]);
			}
		}

//map the input bits to proximal dendrites
		for(i=0;i<16;i++){
			for(j=0;j<16;j++){
				for(k=0;k<3;k++){
					//set the dendrite map values using any mapping function.
					layers[i][j].inputmap[k][0]=rand()%16;
					layers[i][j].inputmap[k][1]=rand()%16;
				}
			}
		}

//learn proximal connections between dendrites
		for(i=0;i<16;i++){
			for(j=0;j<16;j++){
				for(k=0;k<3;k++){
					r=layers[i][j].inputmap[k][0];
					c=layers[i][j].inputmap[k][1];

					if(inp[r][c]==1){
						//synapse gets connected
						layers[i][j].dendrite_prox[k]=0.5; 
					
						//neighbourhood synapses get affected
						if(i-1>0&&layers[i-1][j].dendrite_prox[k]==0.0)
							layers[i-1][j].dendrite_prox[k]=(float)rand()/(float)RAND_MAX; //a random value
						if(j-1>0 && layers[i][j-1].dendrite_prox[k]==0.0)
							layers[i][j-1].dendrite_prox[k]=(float)rand()/(float)RAND_MAX; //a random value
						if(i+1<16 && layers[i+1][j].dendrite_prox[k]==0.0)
							layers[i+1][j].dendrite_prox[k]=(float)rand()/(float)RAND_MAX; //a random va;ue
						if(j+1<16 && layers[i][j+1].dendrite_prox[k]==0.0)
							layers[i][j+1].dendrite_prox[k]=(float)rand()/(float)RAND_MAX; //a random value
					}
				}
			}
		}

//phase 1:- calculate the overlap of input with the columns
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			setOverlap(&layers[i][j]);
			for(k=0;k<3;k++){
				layers[i][j].overlap+=isConnected(&layers[i][j],layers[i][j].dendrite_prox[k]);
			}
			
			if(layers[i][j].overlap<minOverlap){
				layers[i][j].overlap=0;
			}
			else{
				layers[i][j].overlap*=(layers[i][j].boost);
				//layers[i][j].overlapDutyCycle+=1.0;
			}
		}
	}

//phase 2: - calculate the column winners after inhibition
	activebits=0;
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			minLocalActivity=kthscore(layers,i,j,desiredLocalActivity);
			if( layers[i][j].overlap>0 && layers[i][j].overlap>=minLocalActivity){
				layers[i][j].col_state=1;
				activebits++;
			}
			//else if(layers[i][j].overlap>0){
				//layers[i][j].activeDutyCycle =0.0;
			//}
		}
	}	//--> end of distributed sparse representation of input bits

//write the output to the shared core
		(*(outflag))=lock;
		k=0;
		/*for(i=0;i<16;i++){
			for(j=0;j<16;j++){
				(*(sentoutput+k))=layers[i][j].col_state+'0';
				k++;
			}
		}*/
		if(activebits==0){
			(*(sentoutput+k))=activebits+'0';
			k++;
		}
		while(activebits>0){
			(*(sentoutput+k))=(activebits%10)+'0';
			activebits=activebits/10;
			k++;
		}
		(*(sentoutput+k))='\0';
		//(*(sentoutput))=activebits+'0';
		//(*(sentoutput+1))='\0';
		(*(outflag))=unlock;
		(*(state))=OUTPUT_READY;
	
	return EXIT_SUCCESS;
}
