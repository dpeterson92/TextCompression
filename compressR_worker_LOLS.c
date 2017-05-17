#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int part;// the part number of the proces being run. 
int multi; // check if there is more than one part.


/*  makename(char*filein): makes the new LOLS file name, along with the part number, 
based on which number process, and which part of the file is being handled. 
Also contains a check for whether it is necessary to include and part number at all.  */

char* makename(char*filein){


  char num[20];
  sprintf(num,"%d",part);

  char end[30] = "_LOLS";

  if(multi == 1){
    strcat(end,num);
  }
  int i=0;
  int len = strlen(filein);
  char*fileout = (char*)malloc(sizeof(char)*len+5);

  if(!fileout){
    printf("Malloc Failed at line %d in file %s\n",__LINE__,__FILE__);
    return 0;
  }

  while(i<len){

    if(filein[i] == '.'){

      filein[i]='_';
      break;
    }

    i++;
  }

  strcpy(fileout,filein);
  strcat(fileout,end);

  return fileout;
}

/* Gets the string that any particular process is supposed to handle from the input file, 
   using the start and finish numbers to determing that portion. It then returns this unedited portion of the file.*/

char* getstring(char*file, int start, int finish){
  // printf("start: %d\n",start);
  // printf("finish %d\n",finish);

  FILE *fp = fopen(file,"r");
  int length = finish - start;

  //printf("length %d\n",length);

  if(fp == NULL){
    printf("%s\n",strerror(errno));
    return 0;
  }



  char*str = (char*)malloc(sizeof(char)* (length+10));
  str[length+1] = '\0';
  if(!str){
    printf("Malloc Failed at line %d in file %s\n",__LINE__,__FILE__);
    return 0;
  }

  fseek(fp,start,0);
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
    //  printf("str[j]: %c\n",str[j]);
    j++;
  }
  //  printf("Getstring result: %s\n", str);
  fclose(fp);
  return str;
}

/* compress(char*filename, char*s): This function compresses the given string, if necessary, into its smaller form.
After opening and naming a newfile to be written to, it loops through the string, checking for repeating sequentially 
repeating characters greater than two, writting them to the file based on the set condition. 
*/


int compress(char*filename, char*s){

  char* newname = makename(filename); // new LOLS file name

  int len = strlen(s);// length of string input
  
  int same = 1; // count of characters
  int i;
  char comp, temp;
  //char * p = (char*)malloc(sizeof(char)*50);

  FILE * fp;
  fp = fopen (newname, "w+");

  if(fp == NULL){
    printf("Error: %s\n",strerror(errno));
    return 0;
  }

  for(i = 0; i < len; i++){
  

    comp = s[i];
    temp = s[i+1];

    if(comp == temp){
      same++;
    }else if(comp != temp && same > 2){

      fprintf(fp,"%d",same);
      fprintf(fp,"%c",comp);

      same = 1;
    }else if(comp != temp && same == 2){
      fprintf(fp,"%c",comp);
      fprintf(fp,"%c",comp);
      same = 1;
    }else if(comp != temp && same == 1){

      fprintf(fp,"%c",comp);
      same =1;
    }

  }

  // free(s);
  fclose(fp);
  return 0;
}



int main(int argc, char**argv){


  char* string;
 
  if(argc!=6){
    printf("Invalid number of arguments\n");
    return 0;
  }


  char* file = argv[1];
  //printf("Text: %s\n",file);


  int start = atoi(argv[2]);
  //printf("Start: %d\n",start);

  int finish = atoi(argv[3]);
  //printf("Finish %d\n",finish);

  part = atoi(argv[4]);
  //printf("Part: %d\n",part);
  multi = atoi(argv[5]);

  string = getstring(file,start,finish);
  // printf("string: %s\n",string);
  compress(file,string);




  return 0;
}
