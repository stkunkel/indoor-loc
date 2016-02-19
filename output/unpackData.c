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
#define NUM_OF_VALUES		22//23//sizeof(MpuRegisterData)
#define NUMBER_OF_AXES		3

/*
 * Typedefs
 */
typedef struct {
	int16_t gyro[NUMBER_OF_AXES];
	int16_t accel[NUMBER_OF_AXES];
	int16_t compass[NUMBER_OF_AXES];
	int32_t temp;
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
  //int16_t i1 = 0, i2 = 0;

  //Open Files
  status = openFiles();
  if (status != 0){
    return 1;
  }
  
  
  //Read Ten Values at a Time from Input File
  while (fread(buff, NUM_OF_VALUES, 1, (FILE*)infile)){ //fgets(buff, NUM_OF_VALUES, (FILE*)infile)
    //Convert back
    //data.gyro[0] = (int16_t) ((buff[0]&0x00FF) | (((buff[1]&0x00FF) << 8)&0xFF00));
    //data.gyro[1] = (int16_t) ((buff[2]&0x00FF) | (((buff[3]&0x00FF) << 8)&0xFF00));
    //data.gyro[2] = (int16_t) ((buff[4]&0x00FF) | (((buff[5]&0x00FF) << 8)&0xFF00));
    //data.accel[0] = (int16_t) ((buff[6]&0x00FF) | (((buff[7]&0x00FF) << 8)&0xFF00));
    //data.accel[1] = (int16_t) ((buff[8]&0x00FF) | (((buff[9]&0x00FF) << 8)&0xFF00));
    //data.accel[2] = (int16_t) ((buff[10]&0x00FF) | (((buff[11]&0x00FF) << 8)&0xFF00));
    //data.compass[0] = (int16_t) ((buff[12]&0x00FF) | (((buff[13]&0x00FF) << 8)&0xFF00));
    //data.compass[1] = (int16_t) ((buff[14]&0x00FF) | (((buff[15]&0x00FF) << 8)&0xFF00));
    //data.compass[2] = (int16_t) ((buff[16]&0x00FF) | (((buff[17]&0x00FF) << 8)&0xFF00));
    //i1 = ((buff[18]&0x00FF) | (((buff[19]&0x00FF) << 8)&0xFF00)) & 0x0000FFFF;
    //i2 = ((buff[20]&0x00FF) | (((buff[21]&0x00FF) << 8)&0xFF00)) & 0x0000FFFF;
    //data.temp = (int32_t) ((i1&0x0000FFFF) | (((i2&0x0000FFFF) << 16)&0xFFFF0000));
    data.gyro[0]  = (int16_t) (buff[0]&0x00FF);
    data.gyro[0] |= (int16_t) ((buff[1]&0x00FF)<<8);
    data.gyro[1]  = (int16_t) (buff[2]&0x00FF);
    data.gyro[1] |= (int16_t) ((buff[3]&0x00FF)<<8);
    data.gyro[2]  = (int16_t) (buff[4]&0x00FF);
    data.gyro[2] |= (int16_t) ((buff[5]&0x00FF)<<8);
    data.accel[0]  = (int16_t) (buff[6]&0x00FF);
    data.accel[0] |= (int16_t) ((buff[7]&0x00FF)<<8);
    data.accel[1]  = (int16_t) (buff[8]&0x00FF);
    data.accel[1] |= (int16_t) ((buff[9]&0x00FF)<<8);
    data.accel[2]  = (int16_t) (buff[10]&0x00FF);
    data.accel[2] |= (int16_t) ((buff[11]&0x00FF)<<8);
    data.compass[0]  = (int16_t) (buff[12]&0x00FF);
    data.compass[0] |= (int16_t) ((buff[13]&0x00FF)<<8);
    data.compass[1]  = (int16_t) (buff[14]&0x00FF);
    data.compass[1] |= (int16_t) ((buff[15]&0x00FF)<<8);
    data.compass[2]  = (int16_t) (buff[16]&0x00FF);
    data.compass[2] |= (int16_t) ((buff[17]&0x00FF)<<8);
    data.temp  = (int32_t) ((buff[18]&0x00FF)<<0);
    data.temp |= (int32_t) ((buff[19]&0x00FF)<<8);
    data.temp |= (int32_t) ((buff[20]&0x00FF)<<16);
    data.temp |= (int32_t) ((buff[21]&0x00FF)<<24);
    
    
    //Print to Output File
    fprintf(outfile, " %d %d %d %d %d %d %d %d %d %d\r\n", data.gyro[0], data.gyro[1], data.gyro[2], data.accel[0], data.accel[1], data.accel[2], data.compass[0], data.compass[1], data.compass[2], data.temp);
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