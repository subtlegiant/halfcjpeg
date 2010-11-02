#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define _USE_MATH_DEFINES
#define u8 unsigned char

int get_macro_block(u8 * data_buf, u8 ** mblock, int offset)
{
    int i,j;
    data_buf = data_buf + offset;
    
    // pull a 16bit x 16bit macro block of data 
    // from data_buf, starting at offset
    for (i=0; i<16; ++i) {
        for (j=0; j<16; ++j) {
            mblock[i][j] = *(data_buf);
    //        printf("mblock[%d][%d]: %x\n", i, j, mblock[i][j]); 
            ++data_buf;
        }

     }

     return 1;
}

int get_block(u8 ** mblock, u8 ** block, int offsetx, int offsety)
{
    int i, j;
    
    // pull an 8byte x 8byte block of data
    // from mblock, starting at the given
    // index
    for (i=0; i<8; ++i) {
        for (j=0; j<8; ++j) {
            block[i][j] = mblock[offsetx+i][offsety+j];
          //  printf("%d %d mblock %x\n", offsetx+i, offsety+j, mblock[offsetx+i][offsety+i]);
          //  printf("block val: %f\n",(double) block[i][j]);
        }
     }
     
    
    return 1;
}        

int transform_block(u8 ** block, float ** dct_matrix, int x_index, int y_index)
{

    int u, v, x, y;
    float tval_x, tval_y = 0.0;
    float cu, cv;
  //  printf("%d %d", x_index, y_index);

    for (u=0; u<8; ++u) {
        if (u == 0) {
            cu = sqrt(0.5);
        }
        else {
            cu = 1;
        }
        for (v=0; v<8; ++v) {
            if (v==0) {
                cv = sqrt(0.5);
            }
            else {
                cv = 1;
            }
            tval_x = 0;
            for (x=0; x<8; ++x) {
                for (y=0; y<8; ++y) {
//                    printf("block_val: %f\n ", tval_y);
//                    printf("x %d y %d u %d v %d:", x, y, u, v);
//                    printf("form1: %lf\n", cosf(((2*x+1)*u*M_PI)/16));
                    tval_y = (double)tval_y + (((double) block[x][y]) *
                             cos(((2*x+1)*u*M_PI)/16) * cos(((2*y+1)*v*M_PI)/16));
                } 
            //    printf("%f\n", tval_y);
            //    printf("%f\n", tval_x);          
                tval_x = (double)tval_x + (double)tval_y;
                tval_y = 0;
            }
         //   printf("cu %f, cv %f\n", cu, cv);
            dct_matrix[u][v] = (float) ((cu/2)*(cv/2)*tval_x);
        //    printf("dct_matrix[%d][%d]: %f\n", u+x_index, v+y_index, dct_matrix[u+x_index][v+y_index]);
         }
    }

  return 1;
     
}

int quantize_block_and_ouput(float ** dct_matrix, int ** q_matrix, int ** q_coeff, char * fname) 
{
    int x, y;
    u8 temp;
    FILE * outfile;

    outfile = fopen(fname, "a");
    
    for (x=0; x<8; ++x) {
        for (y=0; y<8; ++y) {
            q_coeff[x][y] = round( dct_matrix[x][y] / q_matrix[x][y] );
         //   printf("q_coeff[%d][%d] = %d\n", x, y, q_coeff[x][y]);
          }
    }
    
    // crop value and remap, then write to ouput file 
    for (x=0; x<8; ++x) {
        for (y=0; y<8; ++y) {
            if (q_coeff[x][y] <= -127) {
                q_coeff[x][y] = -127;
            }
            else if (q_coeff[x][y] >= 128) {
               q_coeff[x][y] = 128;
            }
            q_coeff[x][y] += 127;
       //     printf("q_coeff[%d][%d] = %d\n", x, y, q_coeff[x][y]);
            temp = (u8)q_coeff[x][y];
            fprintf(outfile, "%d ", q_coeff[x][y]);
          //  fwrite(&temp, sizeof(u8), 1, outfile);
            
        }
    }
        
    return 1;
}
    
    
int main(int argc, char** argv)
{
    // check input
    if (argc != 5) {
	perror("Wrong number of arguments. Exiting.");
	exit(EXIT_FAILURE);
    }
    
    // user arguments
    char* image = argv[1];
    char* quantFileName = argv[2];
    int qScale = atoi(argv[3]);
    char* outputFileName = argv[4];
    printf("%s\n", image);
    printf("%s\n", quantFileName);    
  //  printf("%d\n", qScale);
  //  printf("%s\n", outputFileName);


    // PGM header file information
    char* head = "";
    char* xValTemp = "";
    int xVal = 0;
    char* yValTemp = "";
    int yVal = 0;
    char* colorVal;
    int i;
 
    // read input from PGM file image
    FILE* dataFile = fopen(image,"r");
    FILE* outfile = fopen(outputFileName,"w");

    // read the first line of the PGM file, should be P5
    char tempBuff[256];
    fgets(tempBuff, 10, dataFile);
    //printf("tempBuff: %s\n", tempBuff);

    // read the second line of input, x and y pixel sizes
    fgets(tempBuff,256,dataFile);
    //printf("tempBuff line 2: %s\n", tempBuff);
   

    // get each pixel number, delimited by a space and
    // copy x value from buffer 
    xValTemp = strtok(tempBuff, " ");
    xVal = atoi(xValTemp);
    //printf("xVal: %d\n", xVal);

    // get and copy y value from buffer
    yValTemp = strtok(NULL," ");
    yVal = atoi(yValTemp);
    //printf("yVal: %d\n", yVal);

    // get and copy color value from PGM file
    fgets(tempBuff, 256, dataFile);
    colorVal = (char *) malloc(sizeof(tempBuff)/sizeof(tempBuff[0]));
    strcpy(colorVal, tempBuff);
    //printf("colorval: %s\n", colorVal);

    // create buffer and store grayscale data
    u8 * dataBuffer = (u8 *) malloc(xVal*yVal + 1);  
    fread(dataBuffer, sizeof(u8), (xVal*yVal), dataFile);
    fclose(dataFile);
    //printf("%x\n",  dataBuffer);

    // write header to output file
    fprintf(outfile, "MJPEG\n");
    fprintf(outfile, "%d %d\n", xVal, yVal);
    fprintf(outfile, "%d\n", qScale);
    fclose(outfile);

    // read in quantfile matrix
    int ** q_matrix;
    int temp;
    int x = 0;
    int y = 0;
    FILE * q_file = fopen(quantFileName, "r");
    
    // allocate quantization matrix
    q_matrix = (int **) malloc(8 * sizeof(int *));
    for (i=0; i<8; ++i) {
        q_matrix[i] = (int *) malloc(8 * sizeof(int));
    }   
 
    while (fscanf(q_file, "%d", &temp) != EOF) {
        q_matrix[x][y] = temp;
        ++x;
        if (x > 7) {
           x = 0;
           ++y;
        }
    }
  
   // printf("q_matrix[0][0] %d\n", q_matrix[0][0]);

    int offsetx = 0;
    int offsety = 0;
    int offset = 0;
    u8 ** mb;
    u8 ** block;
    float **  dct_matrix;
    int ** q_coeff;
    int j, k;
    
    // allocate memory for the dct matrix
    dct_matrix = (float **) malloc(8 * sizeof(float *));
    for (i=0; i<8; ++i) {
        dct_matrix[i] = (float *) malloc(8 * sizeof(float));
    }
  //  printf("xVal: %d, yVal: %d\n", xVal, yVal);
  
    // allocate memory for the quantization matrix
    q_coeff = (int **) malloc(8 * sizeof(int *));
    for (i=0; i<8; ++i) {
        q_coeff[i] = (int *) malloc(8 * sizeof(int));
    }

    // allocate memory for the next macro block
    mb = (u8 **) malloc(16 * sizeof(u8*));
    for (i=0; i<16; ++i) {
        mb[i] = (u8 *) malloc(16 * sizeof(u8));
    }

    // allocate memory for block 
    block = (u8 **) malloc(8 * sizeof(u8*));
    for (i=0; i<8; ++i) {
        block[i] = (u8 *) malloc(8 * sizeof(u8));
    }
    
    while (*(dataBuffer) != '\0') {
      get_macro_block(dataBuffer, mb, offsetx);
   //   printf("%x\n", mb[0][0]);
      for (j=0; j<2; j++) {
          for (k=0; k<2; k++) {
            get_block(mb, block, offsetx, offsety);
            transform_block(block, dct_matrix, offsetx, offsety);
            quantize_block_and_ouput(dct_matrix, q_matrix, q_coeff, outputFileName);
            offsetx += 8;
          }
          offsetx = 0;
          offsety += 8; 
      }
      dataBuffer += 16*16;
    }        
   
    return 1;
}
