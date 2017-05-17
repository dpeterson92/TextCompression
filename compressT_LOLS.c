#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

int compress(char * fileName, char * s){
	//printf("compressing the following string: %s\n",s);
	fflush(stdout);

  	// int pos = 0;
  	int len = strlen(s);
  	char* x = (char*)malloc(sizeof(char)*len);
	if(!x){
		printf("Malloc failed at line %d in %s\n",__LINE__,__FILE__);	
		return 0;
	}
  	int same = 1;
  	int i;
  	char comp, temp;
  	char * p = (char*)malloc(sizeof(char)*50);
	if(!p){
		printf("Malloc failed at line %d in %s\n",__LINE__,__FILE__);	
		return 0;
	}
	
	
	
	FILE * fp;
	fp = fopen (fileName, "w+");
	if(!fp){
		printf("fopen(%s) failed at line %d in %s\n",fileName,__LINE__,__FILE__);
		printf("%s\n",strerror(errno));
		fflush(stdout);
		return 0;
	}

	for(i = 0; i < len; i++){
    	comp = s[i];
    	temp = s[i+1];
	
	    if(comp == temp){
    	same++;
    	}else if(comp != temp && same > 2){

      	// p = convert(same);
      	//  printf("Compresses number: %s\n", p);
      	fprintf(fp,"%d",same);
		//printf("%d",same);
      	//x[pos] = p;
      	//   x[pos+1] = comp;
      	fprintf(fp,"%c",comp);
		//printf("%c",comp);
		
      	//  pos+=2;
      	same = 1;
    	}else if(comp != temp && same == 2){
    	  fprintf(fp,"%c",comp);
    	  fprintf(fp,"%c",comp);
		//printf("%c",comp);
		//printf("%c",comp);
      	// x[pos]= comp;
      	// x[pos+1]= comp;
      	//  pos+=2;
      	same = 1;
    	}else if(comp != temp && same == 1){
      	//   x[pos]= comp;
      	// pos++;
    	  fprintf(fp,"%c",comp);
		//printf("%c",comp);
      	same =1;
    	}

  	}
	//printf("\n");
	fflush(stdout);
  	// printf("Compressed String: %s\n", x);
    free(s);
  	free(x);
  	free(p);
  	fclose(fp);
  	return 0;
}

char * getString(char * fileName, int start, int finish){
  // printf("start: %d\n",start);
  // printf("finish %d\n",finish);

  FILE *fp = fopen(fileName,"r");
  int length = finish - start;

  //printf("length %d\n",length);

  if(fp == NULL){
    printf("%s\n",strerror(errno));
    return 0;
  }



  char*str = (char*)malloc(sizeof(char)* (length+10));
  str[length+1]='\0';

  if(!str){
    printf("Malloc Failed at line %d in file %s\n",__LINE__,__FILE__);
    return 0;
  }

  fseek(fp,start,SEEK_SET);
  int i = 0;
  int j = 0;
  char c;

  while(i<=length){

    c = fgetc(fp);
    i++;

    if(!isalpha(c)){
      continue;
    }
    str[j]= c;
    j++;
  }
  fclose(fp);
  return str;
}

struct compressStruct{
	char * fileName;
	int start;
	int finish;
	int threadNo;
	int multi;
};
/*
char* convert(int d){
  char *s = (char*)malloc(sizeof(char)*50);
  char *z = (char*)malloc(sizeof(char)*50); 
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
	printf("%d\n",__LINE__);
	fflush(stdout);
	
  free(s);
	printf("%d\n",__LINE__);
	fflush(stdout);
	
  return z;
}
*/
void * compressThread(void * cS){
	struct compressStruct * details = (struct compressStruct *)cS;
	int start=details->start;
	int finish=details->finish;
	int threadNo=details->threadNo;
	int multi=details->multi;
	char * fileName=details->fileName;
	
	/*
	printf("Thread %s beginning\n",threadNo);
	fflush(stdout);
	*/
	char * newFile = malloc(sizeof(char)*(strlen(fileName)+20));
	if(!newFile){
		printf("Malloc failed at line %d in %s\n",__LINE__,__FILE__);
		return 0;
	}
	
	char * fileString = getString(fileName,start,finish);
	
	char * period = strchr(fileName,'.');
	*period='_';
	if(multi){
		sprintf(newFile,"%s_LOLS%d",fileName,threadNo);
		//printf("%s\n",newFile);
	}else{
		sprintf(newFile,"%s_LOLS",fileName);
	}
	compress(newFile,fileString);
  
  free(newFile);
	free(details->fileName);
	free(cS);
	/*
	printf("Thread %s ending\n",threadNo);
	fflush(stdout);
	*/
	pthread_exit(NULL);
}

int main(int argc, char ** argv){
	if(argc!=3){
		printf("Invalid number of arguments\n");
		return 0;
	}
	
	char *filename = malloc(sizeof(char)*(strlen(argv[1])));
	if(!filename){
		printf("Malloc failed at line %d in %s\n",__LINE__,__FILE__);	
		return 0;
	}
	
	strcpy(filename,argv[1]);
	
	int div = atoi(argv[2]);
	if(div==0){
		printf("Improper arguments:  Requires arguments of <filename> <positive integer>\n");
		return 0;
	}
	
	FILE *fp = fopen(filename,"r");
	if(!fp){
		printf("fopen(%s) failed at line %d in %s\n",filename,__LINE__,__FILE__);
		printf("%s\n",strerror(errno));
		fflush(stdout);
		return 0;
	}
	fseek(fp,0L,SEEK_END);
	int filelen = ftell(fp)-1;
	rewind(fp);
	fclose(fp);
	/*
	printf("%d\n",__LINE__);
	fflush(stdout);
	*/
	int start[div];
	int finish[div];
	int pieceSize=filelen/div;
	int leftover=filelen%div;
	int i;
	for(i=0; i<div; i++){
		
		if(i==0){
			start[i]=0;
		}else{
			start[i]=finish[i-1]+1;
		}
		finish[i]=start[i]+pieceSize-1;
		if(leftover>0){
			finish[i]++;
			leftover--;
		}
		
	}
	
	pthread_t threads[div];
	/*
	printf("%d\n",__LINE__);
	fflush(stdout);
	*/
	for(i=0; i<div;i++){
		
		pthread_t newthread;
		pthread_attr_t attr;
		pthread_attr_t * p_attr = &attr;
		if(pthread_attr_init(p_attr)){
			printf("error at attr_init in thread %d",i);
      continue;
		}
		pthread_attr_setscope(p_attr,PTHREAD_SCOPE_SYSTEM);
		
		struct compressStruct * cS = malloc(sizeof(struct compressStruct));
		if(!cS){
			printf("Malloc failed at line %d in %s\n",__LINE__,__FILE__);	
			return 0;
		}
		cS->start=start[i];
		cS->finish=finish[i];
		cS->threadNo=i;
		cS->fileName=malloc(sizeof(char)*50);
		if(!cS->fileName){
			printf("Malloc failed at line %d in %s\n",__LINE__,__FILE__);	
			return 0;
		}
		if(div==1){
			cS->multi=0;
		}else{
			cS->multi=1;
		}
		strcpy(cS->fileName,argv[1]);
		
		if(pthread_create(&newthread,p_attr,compressThread,(void *) cS)){
			printf("error at pthread_create %d at line %d",i,__LINE__);
      continue;
		}
		threads[i]=newthread;
		
		
		//spawn thread here, gets piece and 'compresses'
		//char *piece = getString(fpLocal,start[i],finish[i]);
		//thread/process gets passed (filename,start[i],finish[i],i)
		//(name of file to open, index to start, index to end, number to append to 			//file name
		//spawn thread/process to compress piece here
		//thread/process runs getString then compress on the string it gets
	}
	/*
	printf("%d\n",__LINE__);
	fflush(stdout);
	*/
	for(i=0;i<div;i++){
    if(threads[i]){
		  pthread_join(threads[i],NULL);
    }
	}
	/*
	printf("%d\n",__LINE__);
	fflush(stdout);
	*/
	free(filename);
	//wait for all threads here
	return 0;
}

