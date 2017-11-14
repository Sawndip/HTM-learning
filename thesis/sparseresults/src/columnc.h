
struct column{
	int col_size;
	int cell_state[3];
	//float dendrite_distal[3][3];
	//int dist_wid;
	float minDutyCycle,activeDutyCycle,overlapDutyCycle,dendrite_prox[3],connPerm;
	int overlap;
	int col_state;
	int boost;
	int inputmap[3][2];
};

	void setValue(struct column *obj){
		int i=0,j=0;
		(*obj).col_size=3;
		(*obj).col_state=0;
		
		//initialize member cell states in a column
		for(i=0;i<(*obj).col_size;i++){
			(*obj).cell_state[i]=1;
		}

		//initialize proximal map
		for(i=0;i<3;i++){
			for(j=0;j<2;j++){
				(* obj).inputmap[i][j]=-1;
			}
		}
		
		//set prox dendrite to 0 before learning
		for(i=0;i<3;i++){
			(*obj).dendrite_prox[i]=0.0;
		}

		//set boost parameter and connPerm values
		(*obj).boost=1;
		(*obj).connPerm=0.5;

		//set the dutycycle values to 0
		(*obj).activeDutyCycle=0.0;
		(*obj).overlapDutyCycle=0.0;
		(*obj).minDutyCycle=0.0;
		
	}
	
	void setOverlap(struct column *obj){
		(*obj).overlap=0;
	}
	
	int isConnected(struct column *s,float val){
		if(val>=(*s).connPerm)
			return 1;
		else
			return 0;
	}

	void boostfunction(struct column *s){
		if((*s).activeDutyCycle<(*s).minDutyCycle)	
		(*s).boost+=1;	
	}

	void increasePermanences(struct column *s,float inc){
		//(*s).connPerm+=0.1;
		int i=0;
		for(i=0;i<3;i++){
			(*s).dendrite_prox[i]+=inc;
		}
	}
	
	int active(struct column *s,float val){
		if(val>=(*s).connPerm)
			return 1;
		else
			return 0;		
	}	
