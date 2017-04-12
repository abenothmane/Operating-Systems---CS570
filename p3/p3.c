
#include "p3.h"
#define COURSEID "570"

extern int nrRobots;
extern int quota;
extern int seed;
int fd;
int fd2;
sem_t *pmutx; /* semaphore guarding access to shared data */
int count; /* local copy of counter */
int row;
char semaphoreMutx[SEMNAMESIZE];

/* General documentation for the following functions is in p3.h
   Here you supply the code, and internal documentation:
   */
void initStudentStuff(void) {
  sprintf(semaphoreMutx,"/%s%ldmutx",COURSEID,(long)getuid());

 if((pmutx = sem_open(semaphoreMutx,O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR,1)) == SEM_FAILED){
 	
	CHK((int)(pmutx = sem_open(semaphoreMutx,O_RDWR)));
	CHK(fd = open("countfile",O_RDWR));
	CHK(fd2 = open("rowfile",O_RDWR));
 }
 else{
 	//Initialize the file
	
	CHK(fd = open("countfile",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
	CHK(fd2 = open("rowfile",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
  	count = 0;
	CHK(sem_wait(pmutx)); //wait for files to be initialized
  	CHK(lseek(fd,0,SEEK_SET));
  	assert(sizeof(count) == write(fd,&count,sizeof(count))); //write count to file
  	row = 1;
  	CHK(lseek(fd2,0,SEEK_SET));
  	assert(sizeof(row) == write(fd2,&row,sizeof(row)));  //write row to file
	CHK(sem_post(pmutx)); //files are now initialized
	
  }
}

void placeWidget(int n) {
  /* request access to critical section */
  CHK(sem_wait(pmutx));
  /* begin critical section -- read count, increment count, write count */
  CHK(lseek(fd,0,SEEK_SET));
  assert(sizeof(count) == read(fd,&count,sizeof(count)));
  //rowfile set
  CHK(lseek(fd2,0,SEEK_SET));
  assert(sizeof(row) == read(fd2,&row,sizeof(row)));
  count++;
  int tmp=count;
  if(count == ((nrRobots * quota))){
  	printeger(n);
	printf("F\n");
	fflush(stdout);
    	CHK(close(fd));
    	CHK(unlink("countfile"));
	CHK(close(fd2));
    	CHK(unlink("rowfile"));
    	CHK(sem_close(pmutx));
    	CHK(sem_unlink(semaphoreMutx));
  }
  else{
  	int i;
  	for(i=1; i<row ; i++){
  		count=count-i;
  	}
  	if(count == row){
    		printeger(n);
  		printf("N\n");
  		fflush(stdout);
		row++;
  	}
	else{
		printeger(n);
		fflush(stdout);
	}
	count=tmp;
	CHK(lseek(fd,0,SEEK_SET));
  	assert(sizeof(count) == write(fd,&count,sizeof(count)));
  	CHK(lseek(fd2,0,SEEK_SET));
  	assert(sizeof(row) == write(fd2,&row,sizeof(row)));
  	/* end critical section */
  	CHK(sem_post(pmutx)); /* release critical section */
  }
  
  
}

