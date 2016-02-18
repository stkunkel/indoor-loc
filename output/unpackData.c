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
#define NUMBER_OF_AXES		3
#define DATA_NUMBER_OF_BYTES	sizeof(MpuRegisterData)
#define BYTE0			0x000000FF
#define BYTE1			0x0000FF00
#define BYTE2			0x00FF0000
#define BYTE3			0xFF000000
#define SAMPLES_PER_PACKET	((int) (XMODEM_PKG_CONT_SIZE / DATA_NUMBER_OF_BYTES))
#define XMODEM_PADDING_BYTES	((int) (XMODEM_PKG_CONT_SIZE % DATA_NUMBER_OF_BYTES))
#define XMODEM_PACKET_SIZE	133
#define XMODEM_PKG_CONT_SIZE	128
#define XMODEM_INDEX_HEAD	0
#define XMODEM_INDEX_CONTENT	3
#define XMODEM_INDEX_CRC	131

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
  int status, i;
  char buff[XMODEM_PACKET_SIZE];
  MpuRegisterData *pdata;

  //Open Files
  status = openFiles();
  if (status != 0){
    return 1;
  }
  
  
  //Read One XModem Packet from Input File
  while (fgets(buff, XMODEM_PACKET_SIZE, (FILE*)infile)){
    
    
    //Get Address of packet content
    pdata = (MpuRegisterData*) (&buff);
    
    //Go through samples
    for (i = 0; i < SAMPLES_PER_PACKET; i++){
      //Print to Output File
      fprintf(outfile, "%hi %hi %hi %hi %hi %hi %hi %hi %hi %d\r\n", pdata->gyro[0], pdata->gyro[1], pdata->gyro[2], pdata->accel[0], pdata->accel[1], pdata->accel[2], pdata->compass[0], pdata->compass[1], pdata->compass[2], pdata->temp);
      
      //Increase Pointer
      pdata++;
    }
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