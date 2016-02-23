/*
 * Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * Parameters
 */
//#define ROBOT_DATA		//Uncomment to get MPU Data only
#define INFILEDESC		"data.bin"
#define OUTFILEDESC		"data.txt"
#define BYTE0			0x000000FF
#define BYTE1			0x0000FF00
#define BYTE2			0x00FF0000
#define BYTE3			0xFF000000
#define NUMBER_OF_AXES		3
#define NUMBER_OF_JOINTS	6

#ifdef ROBOT_DATA
#define NUM_OF_VALUES		46
#else
#define NUM_OF_VALUES		22
#endif

/*
 * Typedefs
 */
typedef struct {
	int16_t gyro[NUMBER_OF_AXES];
	int16_t accel[NUMBER_OF_AXES];
	int16_t compass[NUMBER_OF_AXES];
	int32_t temp;
} MpuRegisterData;

typedef struct {
	uint32_t pwmValues[NUMBER_OF_JOINTS];
	MpuRegisterData mpuData;
} RobotMpuData;

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
  unsigned char buff[NUM_OF_VALUES];
  uint32_t samples, i;
#ifdef ROBOT_DATA
  RobotMpuData data;
#else
  MpuRegisterData data;
#endif

  //Open Files
  status = openFiles();
  if (status != 0){
    return 1;
  }
  
  //Read out number of samples
  if (fread(buff, sizeof(samples), 1, (FILE*)infile)){
    samples  = (int32_t) ((buff[0]&BYTE0)<<0);
    samples |= (int32_t) ((buff[1]&BYTE0)<<8);
    samples |= (int32_t) ((buff[2]&BYTE0)<<16);
    samples |= (int32_t) ((buff[3]&BYTE0)<<24); 
    
    printf("Reading in %d samples...\r\n", samples);
  } else {
    return 1;
  }
  
  //Go through samples
  for (i = 0; i < samples; i++){
    //Read One Sample from Input File
    fread(buff, NUM_OF_VALUES, 1, (FILE*)infile);
  
    //Convert back
#ifdef ROBOT_DATA
    data.pwmValues[0]  = (uint32_t) (buff[0]&BYTE0);
    data.pwmValues[0] |= (uint32_t) ((buff[1]&BYTE0)<<8);
    data.pwmValues[0] |= (uint32_t) ((buff[2]&BYTE0)<<16);
    data.pwmValues[0] |= (uint32_t) ((buff[3]&BYTE0)<<24);
    data.pwmValues[1]  = (uint32_t) (buff[4]&BYTE0);
    data.pwmValues[1] |= (uint32_t) ((buff[5]&BYTE0)<<8);
    data.pwmValues[1] |= (uint32_t) ((buff[6]&BYTE0)<<16);
    data.pwmValues[1] |= (uint32_t) ((buff[7]&BYTE0)<<24);
    data.pwmValues[2]  = (uint32_t) (buff[8]&BYTE0);
    data.pwmValues[2] |= (uint32_t) ((buff[9]&BYTE0)<<8);
    data.pwmValues[2] |= (uint32_t) ((buff[10]&BYTE0)<<16);
    data.pwmValues[2] |= (uint32_t) ((buff[11]&BYTE0)<<24);
    data.pwmValues[3]  = (uint32_t) (buff[12]&BYTE0);
    data.pwmValues[3] |= (uint32_t) ((buff[13]&BYTE0)<<8);
    data.pwmValues[3] |= (uint32_t) ((buff[14]&BYTE0)<<16);
    data.pwmValues[3] |= (uint32_t) ((buff[15]&BYTE0)<<24);
    data.pwmValues[4]  = (uint32_t) (buff[16]&BYTE0);
    data.pwmValues[4] |= (uint32_t) ((buff[17]&BYTE0)<<8);
    data.pwmValues[4] |= (uint32_t) ((buff[18]&BYTE0)<<16);
    data.pwmValues[4] |= (uint32_t) ((buff[19]&BYTE0)<<24);
    data.pwmValues[5]  = (uint32_t) (buff[20]&BYTE0);
    data.pwmValues[5] |= (uint32_t) ((buff[21]&BYTE0)<<8);
    data.pwmValues[5] |= (uint32_t) ((buff[22]&BYTE0)<<16);
    data.pwmValues[5] |= (uint32_t) ((buff[23]&BYTE0)<<24);
    
    data.mpuData.gyro[0]  = (int16_t) (buff[24]&BYTE0);
    data.mpuData.gyro[0] |= (int16_t) ((buff[25]&BYTE0)<<8);
    data.mpuData.gyro[1]  = (int16_t) (buff[26]&BYTE0);
    data.mpuData.gyro[1] |= (int16_t) ((buff[27]&BYTE0)<<8);
    data.mpuData.gyro[2]  = (int16_t) (buff[28]&BYTE0);
    data.mpuData.gyro[2] |= (int16_t) ((buff[29]&BYTE0)<<8);
    data.mpuData.accel[0]  = (int16_t) (buff[30]&BYTE0);
    data.mpuData.accel[0] |= (int16_t) ((buff[31]&BYTE0)<<8);
    data.mpuData.accel[1]  = (int16_t) (buff[32]&BYTE0);
    data.mpuData.accel[1] |= (int16_t) ((buff[33]&BYTE0)<<8);
    data.mpuData.accel[2]  = (int16_t) (buff[34]&BYTE0);
    data.mpuData.accel[2] |= (int16_t) ((buff[35]&BYTE0)<<8);
    data.mpuData.compass[0]  = (int16_t) (buff[36]&BYTE0);
    data.mpuData.compass[0] |= (int16_t) ((buff[37]&BYTE0)<<8);
    data.mpuData.compass[1]  = (int16_t) (buff[38]&BYTE0);
    data.mpuData.compass[1] |= (int16_t) ((buff[39]&BYTE0)<<8);
    data.mpuData.compass[2]  = (int16_t) (buff[40]&BYTE0);
    data.mpuData.compass[2] |= (int16_t) ((buff[41]&BYTE0)<<8);
    data.mpuData.temp  = (int32_t) ((buff[42]&BYTE0)<<0);
    data.mpuData.temp |= (int32_t) ((buff[43]&BYTE0)<<8);
    data.mpuData.temp |= (int32_t) ((buff[44]&BYTE0)<<16);
    data.mpuData.temp |= (int32_t) ((buff[45]&BYTE0)<<24);
    
    //Print to Output File
    fprintf(outfile, "%d %d %d %d %d %d %d %d %d %d %u %u %u %u %u %u\r\n", data.mpuData.gyro[0], data.mpuData.gyro[1], data.mpuData.gyro[2], data.mpuData.accel[0], data.mpuData.accel[1], data.mpuData.accel[2], data.mpuData.compass[0], data.mpuData.compass[1], data.mpuData.compass[2], data.mpuData.temp, data.pwmValues[0], data.pwmValues[1], data.pwmValues[2], data.pwmValues[3], data.pwmValues[4], data.pwmValues[5]);
#else
    data.gyro[0]  = (int16_t) (buff[0]&BYTE0);
    data.gyro[0] |= (int16_t) ((buff[1]&BYTE0)<<8);
    data.gyro[1]  = (int16_t) (buff[2]&BYTE0);
    data.gyro[1] |= (int16_t) ((buff[3]&BYTE0)<<8);
    data.gyro[2]  = (int16_t) (buff[4]&BYTE0);
    data.gyro[2] |= (int16_t) ((buff[5]&BYTE0)<<8);
    data.accel[0]  = (int16_t) (buff[6]&BYTE0);
    data.accel[0] |= (int16_t) ((buff[7]&BYTE0)<<8);
    data.accel[1]  = (int16_t) (buff[8]&BYTE0);
    data.accel[1] |= (int16_t) ((buff[9]&BYTE0)<<8);
    data.accel[2]  = (int16_t) (buff[10]&BYTE0);
    data.accel[2] |= (int16_t) ((buff[11]&BYTE0)<<8);
    data.compass[0]  = (int16_t) (buff[12]&BYTE0);
    data.compass[0] |= (int16_t) ((buff[13]&BYTE0)<<8);
    data.compass[1]  = (int16_t) (buff[14]&BYTE0);
    data.compass[1] |= (int16_t) ((buff[15]&BYTE0)<<8);
    data.compass[2]  = (int16_t) (buff[16]&BYTE0);
    data.compass[2] |= (int16_t) ((buff[17]&BYTE0)<<8);
    data.temp  = (int32_t) ((buff[18]&BYTE0)<<0);
    data.temp |= (int32_t) ((buff[19]&BYTE0)<<8);
    data.temp |= (int32_t) ((buff[20]&BYTE0)<<16);
    data.temp |= (int32_t) ((buff[21]&BYTE0)<<24);
    
    //Print to Output File
    fprintf(outfile, "%d %d %d %d %d %d %d %d %d %d\r\n", data.gyro[0], data.gyro[1], data.gyro[2], data.accel[0], data.accel[1], data.accel[2], data.compass[0], data.compass[1], data.compass[2], data.temp);
#endif
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