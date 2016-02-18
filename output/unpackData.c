/*
 * Includes
 */
#include <stdio.h>
#include <stdlib.h>

/*
 * Parameters
 */
#define INFILEDESC		"data.bin"
#define OUTFILEDESC		"data.txt"
#define NUM_OF_VALUES		22//sizeof(MpuRegisterData)
#define NUMBER_OF_AXES		3

/*
 * Typedefs
 */
typedef struct {
	short gyro[NUMBER_OF_AXES];
	short accel[NUMBER_OF_AXES];
	short compass[NUMBER_OF_AXES];
	int temp; //instead of long --> 4 byte
	//short fill;
} MpuRegisterData;

/*
 * Global Variables
 */
static FILE* infile;
static FILE* outfile;

/*
 * Function Prototypes
 */
int openFiles();
int closeFiles();

/*
 * Main
 */
int main() {
  //Variables
  int status;
  char buff[NUM_OF_VALUES];
  MpuRegisterData *pdata;

  //Open Files
  status = openFiles();
  if (status != 0){
    return 1;
  }
  
  
  //Read Ten Values at a Time from Input File
  while (fgets(buff, NUM_OF_VALUES, (FILE*)infile)){
    //Convert back
    pdata = (MpuRegisterData*) &buff;

    //Check for invalid line
    //if (pdata->temp == 0){
    //  break;
    //}
    
    //Print to Output File
    fprintf(outfile, "%hi %hi %hi %hi %hi %hi %hi %hi %hi %d\r\n", pdata->gyro[0], pdata->gyro[1], pdata->gyro[2], pdata->accel[0], pdata->accel[1], pdata->accel[2], pdata->compass[0], pdata->compass[1], pdata->compass[2], pdata->temp);
  }
  
  //Close Files
  status = closeFiles();
  if (status != 0){
    return 1;
  }
  
  //Return
  return 0;
}

/*
 * Open Files
 */
int openFiles(){
  //Open Files
  infile = fopen(INFILEDESC, "r");
  outfile = fopen(OUTFILEDESC, "w");

  if (infile == 0 || outfile == 0){
    return 1;
  } else {
    printf("Files opened.\r\n");
  }

  //Return
  return 0;
}

/*
 * Close Files
 */
int closeFiles(){
  //Close Input File
  if (infile){
    fclose(infile);
    printf("Input File closed.\r\n");
  }
  
  //Close Output File
   if (outfile){
    fclose(outfile);
    printf("Output File closed.\r\n");
  }

  //Return
  return 0;
}