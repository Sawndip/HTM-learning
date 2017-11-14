#include<stdio.h>
#include<stdlib.h>
#include<e-hal.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>

const unsigned ShmSize = 256;
const char ShmName[] = "hello_Shm";
const unsigned SeqLen = 20;

int main(){
	unsigned row,col,coreid,i,j,k;
	e_epiphany_t dev;
	e_platform_t platform;
	e_mem_t mbuf;
	int rc,inp[16][16];
	float f;

//receive the input from user (or) hard code
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			if(i==j)
				inp[i][j]=1;
			else
				inp[i][j]=0;
		}
	}
	
	srand(1);
	e_set_loader_verbosity(H_D0);
	e_set_host_verbosity(H_D0);

//initialize epiphany
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);

//allocating a shared buffer
	rc= e_shm_alloc(&mbuf,ShmName,ShmSize);
	if(rc!=E_OK){
		rc=e_shm_attach(&mbuf,ShmName);
	}
	if(rc!=E_OK){
		printf("failed to allocate memory\n");
		return EXIT_FAILURE;
	}

	char buf[ShmSize];

//select a core and setup coreid
	row = rand()%platform.rows;
	col= rand()%platform.cols;
	coreid = (row+platform.row)*64 + col +platform.col;

//open a core on epiphany
	e_open(&dev, row,col,1,1);
	e_reset_group(&dev);	

//load device program onto the core
	if(E_OK!=e_load("e_hello_world.srec", &dev, 0,0,E_TRUE)){
		printf("failed to load e_hello_world.srec\n");
		return EXIT_FAILURE;
	}
	usleep(10000);

// send the input values to e-cores through shared buffer
	/*k=0;
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			buf[k]=inp[i][j]+'0';
			k++;
			//e_write(&mbuf,0,0,0,buf,sizeof(buf));
		}
	}
	e_write(&mbuf,0,0,0,buf,sizeof(buf));*/
	
//read the output from e-cores
	printf("writing to the display...\n");
	e_read(&mbuf,0,0,0,buf,sizeof(buf));
	//printf("%s\n",buf);
	for(i=0;i<256;i++){
		if((i)%16==0)
			printf("\n");
		printf("%d ",buf[i]-'0');
	}
	printf("\n");

//close core on epiphany
	e_close(&dev);

//close shared buffer
	e_shm_release(ShmName);
	e_finalize();

	return 0;
}
