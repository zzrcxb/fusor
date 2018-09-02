#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h> 

int64_t MultIntMatrix(int64_t** mat1, int64_t** mat2, int64_t** retMat, int64_t m1Height, int64_t m1Width, int64_t m2Height, int64_t m2Width, int64_t mod){
	if(m1Width != m2Height)
	  return -1;
	/*
	   printf("********Mat1*************\n");
	   for(int64_t i=0; i < m1Height; i++){
	   for(int64_t j=0; j < m1Width; j++){
	   printf("%d,", mat1[i][j]);
	   }
	   printf("\n");
	   }
	   printf("********Mat2*************\n");
	   for(int64_t i=0; i < m2Height; i++){
	   for(int64_t j=0; j < m2Width; j++){
	   int64_t ele = mat2[i][j];
	   printf("%d,", ele);
	   }
	   printf("\n");
	   }
	   printf("**********RetMat***********\n");
	   */
	for(int64_t i=0; i < m1Height; i++){
		for(int64_t j=0; j < m2Width; j++){
			int64_t ele = 0;
			for(int64_t k=0; k < m1Width; k++){
				ele = (ele + mat1[i][k] * mat2[k][j]) % mod;
			}
			retMat[i][j] = ele;
			//printf("%d,",ele);
		}
		//printf("\n");
	}
	return 0;
}

void* Inc(void* i){
	++ *((int*) i);
}

void* Dec(void* i){
	-- *((int*) i);
}

int64_t ThreadProp(int64_t in){
	pthread_t thread;
	int rc1 = pthread_create(&thread, NULL, Inc, (void *) &in); 
	rc1 = pthread_join(thread, NULL); 
	int rc2 = pthread_create(&thread, NULL, Dec, (void *) &in); 
	rc2 = pthread_join(thread, NULL); 
	int64_t out = in;
	return out;
}

int64_t ForkProp(int64_t in){
	int64_t out;
	int pid, fd[2];
	pipe(fd);
	if((pid=fork())==-1)
	  return -1;
	if(pid == 0){//child process
		close(fd[0]);
		char tmp[20];
		sprintf(tmp,"%ld",in);
		write(fd[1],tmp,sizeof(tmp));
		exit(0);
	}
	else{
		close(fd[1]);
		char receive[20];
		read(fd[0], receive, 20);
		out = atoi(receive);
	}
	return out;
}
