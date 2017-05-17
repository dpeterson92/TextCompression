#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int filelen; // length of the input file.
int multi; // a "boolean" variable that describes  whether or not there are multiple outputs.

/* Convery(int): An integer to ascii converion function*/

char* convert(int d){
  char *s = (char*)malloc(sizeof(char)*50); 

  if(!s){
    printf("Malloc Failed at line %d in file %s\n",__LINE__,__FILE__);
    return 0;
  }
  char *z = (char*)malloc(sizeof(char)*50); 

  if(!z){
    printf("Malloc Failed at line %d in file %s\n",__LINE__,__FILE__);
    return 0;
  }

  int pos;

  do{
    s[pos++]= d%10+'0';
    d= d/10;
  }while(d>0);

  int limit = pos/2;
  int i;
  char temp;
  for(i = 0; i<limit; i++){
    temp = s[i];
    s[i]=s[pos-i-1];
    s[pos-i-1]=temp;
  }
  s[pos++]= '\0';
  strcat(z,s);

  //free(s);
  return z;
}

/*int print(int*x, int end){

  int i;

  for(i=0;i<end;i++){

    printf("%d ",x[i]);
  }

  return 0;
}*/


/* createworker(char*txt, int div) is given the input file name and number of output parts from main and 
accomplishes three main tasks: (1) determine the length of the file, (2) use that length to find the start and end positions of the individual processes, 
and (3) create the worker processes to compress the individual pieces of the file, calling exec and inputing different values found during the run of this function. */

int createworker(char*txt, int div){ 


  //char * input;

  int i;
  

  FILE * fp = fopen(txt ,"r");

  if(fp== NULL){
    printf("Error: %s\n",strerror(errno));
    return 0;
  }

  fseek(fp,0L,SEEK_END);
  filelen = (ftell(fp))-1;
  rewind(fp);
  fclose(fp);


  if(div > (filelen)){
    printf("Error: Amount requested exceeds file length.\n");
    return 0;
  }

  int start[div];
  int finish[div];
  int pieceSize = (filelen/div);
   int leftover = filelen%div;


  for(i=0; i<div; i++){
		
    if(i==0){
      start[i]=0;
    }else{
      start[i]=finish[i-1]+1;
    }


    finish[i]=start[i]+(pieceSize-1);

   
    if(leftover>0){
      finish[i]++;
      leftover--;
    }
		
  }
  
  /* print(start,div);
  printf("\n");
  print(finish,div);
  printf("\n");*/
  // printf("Psize: %d\n",pieceSize);
  // printf("leftover: %d\n",leftover);
  // printf("filelen: %d\n",filelen);

 
  int j,status;
  pid_t pid;
  pid_t pid_array[div];

  for(j=0; j<div; j++){

    pid = fork();
    if(pid > 0){// Parent
      pid_array[j]=pid;
      //waitpid(pid,&status,0);
    }
    if(pid == 0){// It is a Child

      char* s = convert(start[j]); //Start variable
   
      char* f= convert(finish[j]);// Finish Variable

      char* jj = convert(j); // Part number

      char* m = convert(multi); // MULTI "boolean"

      execlp("./wcompress","./wcompress",txt,s,f,jj,m,NULL);
      perror("Exec() fail");
      
    }

  }

  for(j=0;j<div;j++){
    waitpid(pid_array[j],&status,0);
  }


  //free(input);
  return 0;
}

/*  This is the main of the parent/manager process. It takes the command line arguments, 
    performs a few error checks and operations to get values, and then submits the 
    arguments to the (char*, int ) */

int main(int argc, char**argv){

  if(argc!=3){
    printf("Error: Invalid number of arguments.\n");
    printf("Required input: <file name> <number of parts>\n");
    return 0;
  }


  char* input = argv[1];
  int pieces = atoi(argv[2]);


  if(pieces > 1){ //multi is set to 1 if there are multiple requested output files.
    multi = 1;
  }else if(pieces == 1){ // multi is set to 0 if there is only one output file requested.
    multi = 0;
  } 

  if(pieces <= 0){

    printf("Error: number of parts must be a positive integer. \n");
    printf("Required input: <file name> <number of parts>\n");
    return 0;

  }else{

    createworker(input, pieces);
  }

  return 0;
}
