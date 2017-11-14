#include<stdio.h>
#include<stdlib.h>
#include<e-hal.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include<math.h>

const char ShmName[]="hello_Shm";
const unsigned ShmSize=256;
unsigned lock=0x00000000,unlock=0x00000001;
unsigned const OUTPUT_WAIT=3; 
unsigned const OUTPUT_READY = 2;
unsigned const INPUT_READY = 1;
unsigned const INPUT_WAIT=0;

int main(){
	unsigned row,col,coreid,i,j,k,state; //memory address 0x4000 in shared memory is allocated to store 'state' variable
	int rc,inp[16][16],inpactivebits,colsactive;
	FILE *iptr;
	e_epiphany_t dev;
	e_platform_t platform;
	e_mem_t mbuf;
	
	srand(1);
	e_set_loader_verbosity(H_D0);
	e_set_host_verbosity(H_D0);

//initialize epiphany
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);
	
//allocating a shared buffer
	rc=e_shm_alloc(&mbuf,ShmName,ShmSize);
	if(rc!=E_OK){
		rc=e_shm_attach(&mbuf,ShmName);
	}
	if(rc!=E_OK){	
		printf("failed to allocate memory\n");
		return EXIT_SUCCESS;
	}
	char sentinput;
	char fileinput[ShmSize];
	int ch;
	char receivedoutput[ShmSize];

//select a core to load epiphany program
	row=0;
	col=0;
	coreid=(row+platform.row)*64+col+platform.col;

//open a core on epiphany
	e_open(&dev,row,col,1,1);
	e_reset_group(&dev);

	e_write(&dev,row,col,0x4000,&INPUT_WAIT,sizeof(const unsigned));

//load device program onto the core
	if(E_OK!=e_load("e_htm.srec",&dev,row,col,E_TRUE)){
		printf("failed to load application\n");
		return EXIT_FAILURE;
	}

//read the input from file
	inpactivebits=0;
	iptr= fopen("inputhtm.txt","r");
	if(iptr==NULL){
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	printf("reading the file\n");
	while(fscanf(iptr,"%1d",&ch)!=EOF){
		fileinput[k]=ch+'0';
		k++;	
	}
	fclose(iptr);
	k=0;
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			inp[i][j]=fileinput[k]-'0';
			if(inp[i][j]==1)
				inpactivebits++;
			k++;
		}
	}

//write the input to shared input buffer which is in location 0x2000 on shared memory
	k=0;
	e_write(&dev,row,col,0x7000,&lock,sizeof(unsigned));
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			sentinput=inp[i][j]+'0';
			e_write(&dev,row,col,0x2000+k,&sentinput,sizeof(char));
			k++;
		}
	}
	
	e_write(&dev,row,col,0x7000,&unlock, sizeof(unsigned));
	e_write(&dev,row,col,0x4000,&INPUT_READY,sizeof(const unsigned));

	while(1){
		e_read(&dev,row,col,0x4000,&state,sizeof(const unsigned));
		if(state==OUTPUT_READY) break;
	}

//read the output when ready. location 0x5000 is where epiphany writes the output
		k=0;
		/*for(k=0;k<256;k++){
			e_read(&dev,row,col,0x5000+k,&receivedoutput,sizeof(char));
			printf("%c",receivedoutput);
		}*/
		while(1){
			e_read(&dev,row,col,0x5000+k,&receivedoutput[k],sizeof(char));
			if(receivedoutput[k]=='\0')break;
			//printf("%c",receivedoutput[k]);
			k++;
		}

//close epiphany
	e_close(&dev);

//Release the allocated buffer
	e_shm_release(ShmName);
	e_finalize();

	colsactive=0;
	for(i=0;i<k;i++){
		colsactive+=((receivedoutput[i]-'0')*pow(10,i));
	}	
	printf("number of active input bits..\t");
	printf("%d\n",inpactivebits);
	printf("number of columns active ...\t");
	printf("%d\n",colsactive);	
	iptr=fopen("outputhtm.txt","a");
	fprintf(iptr,"%d\n", inpactivebits);
	fprintf(iptr,"%d\n",colsactive);
	return 0;
}
