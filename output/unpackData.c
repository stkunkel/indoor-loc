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
  int i1 = 0, i2 = 0;

  //Open Files
  status = openFiles();
  if (status != 0){
    return 1;
  }
  
  
  //Read Ten Values at a Time from Input File
  while (fgets(buff, NUM_OF_VALUES+1, (FILE*)infile)){
    //Convert back
    data.gyro[0] = (short) ((buff[0]&0x00FF) | ((buff[1]&0x00FF) << 8));
    data.gyro[1] = (short) ((buff[2]&0x00FF) | ((buff[3]&0x00FF) << 8));
    data.gyro[2] = (short) ((buff[4]&0x00FF) | ((buff[5]&0x00FF) << 8));
    data.accel[0] = (short) ((buff[6]&0x00FF) | ((buff[7]&0x00FF) << 8));
    data.accel[1] = (short) ((buff[8]&0x00FF) | ((buff[9]&0x00FF) << 8));
    data.accel[2] = (short) ((buff[10]&0x00FF) | ((buff[11]&0x00FF) << 8));
    data.compass[0] = (short) ((buff[12]&0x00FF) | ((buff[13]&0x00FF) << 8));
    data.compass[1] = (short) ((buff[14]&0x00FF) | ((buff[15]&0x00FF) << 8));
    data.compass[2] = (short) ((buff[16]&0x00FF) | ((buff[17]&0x00FF) << 8));
    i1 = (buff[18] | (buff[19] << 8)) & 0x0000FFFF;
    i2 = (buff[20] | (buff[21] << 8)) & 0x0000FFFF;
    data.temp = (long) (i1 | (i2 << 16));
    
    //Check for invalid line
    if (data.temp == 0){
      break;
    }
    
    //Print to Output File
    fprintf(outfile, "%hi %hi %hi %hi %hi %hi %hi %hi %hi %li\r\n", data.gyro[0], data.gyro[1], data.gyro[2], data.accel[0], data.accel[1], data.accel[2], data.compass[0], data.compass[1], data.compass[2], data.temp);
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