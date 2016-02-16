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
#define NUM_OF_VALUES		22
#define NUMBER_OF_AXES		3

/*
 * Typedefs
 */
typedef struct {
	short gyro[NUMBER_OF_AXES];
	short accel[NUMBER_OF_AXES];
	short compass[NUMBER_OF_AXES];
	long temp;
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
  MpuRegisterData data;

  //Open Files
  status = openFiles();
  if (status != 0){
    return 1;
  }
  
  
  //Read Ten Values at a Time from Input File
  while (fgets(buff, NUM_OF_VALUES+1, (FILE*)infile)){
    //Convert back
    data.gyro[0] = (short) (buff[0] | (buff[1] << 8));
    data.gyro[1] = (short) (buff[2] | (buff[3] << 8));
    data.gyro[2] = (short) (buff[4] | (buff[5] << 8));
    data.accel[0] = (short) (buff[6] | (buff[7] << 8));
    data.accel[1] = (short) (buff[8] | (buff[9] << 8));
    data.accel[2] = (short) (buff[10] | (buff[11] << 8));
    data.compass[0] = (short) (buff[12] | (buff[13] << 8));
    data.compass[1] = (short) (buff[14] | (buff[15] << 8));
    data.compass[2] = (short) (buff[16] | (buff[17] << 8));
    data.temp = (long) (buff[18] | (buff[19] << 8) | (buff[20] << 16) | (buff[21] << 24));
    
    //Print to Output File
    fprintf(outfile, "%hi %hi %hi %hi %hi %hi %hi %hi %hi %ld\r\n", data.gyro[0], data.gyro[1], data.gyro[2], data.accel[0], data.accel[1], data.accel[2], data.compass[0], data.compass[1], data.compass[2], data.temp);
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