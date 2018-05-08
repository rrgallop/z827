/*
  Ryan R. Gallop
  3/4/2018
  COS-350

  PROGRAM 2: .z827 Compression

 */

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[])
{
  
  // Open the files
  int fd = open(argv[1], O_RDONLY); 

  if (fd < 0)
    {
      fprintf(stderr, "Error loading file. Please try again.\n");
      return 1;
    }


  // create input and output buffers
  unsigned char* buffer;
  unsigned char* output;

  // set the size of the buffers using malloc
  int bytesize = lseek(fd, 0, SEEK_END);
  buffer = (unsigned char*)malloc(bytesize);
  
  
  // reset the file pointer, then read the file
  lseek(fd,0, SEEK_SET);
  int r = read(fd, buffer, bytesize);
  printf("r = %d\n", r);
  if (r < 0)
    {
      fprintf(stderr, "Error reading file. Please try again.\n");
    }



  // begin compression/decompression

  // first,
  // detect whether the file is already compressed
  int compressed = 0;
  static const char *comp_ext = ".z827";
  const char *file_ext = strrchr(argv[1], '.');
  if (!strcmp(file_ext,comp_ext)){
    compressed = 1;
  }

  // ===================================================
  // COMPRESSION ALGORITM
  // ===================================================
  if (!compressed){

    
    
    // create a new string for the new file name, fill it with
    // original filename + .z827 ext
    char *newfilename = malloc(strlen(argv[1]) + strlen(comp_ext) +1);
    sprintf(newfilename,"%s%s", argv[1],comp_ext);
    printf("%s\n", newfilename);
    int fdout = creat(newfilename, 0644);

    // initialize some variables
    output = (unsigned char*)malloc(bytesize);
    unsigned long long intbuffer = 0;
    int bitsinbuffer = 0;
    int i = 0, j = 0;

    // write the length of the input file to output
    if (write(fdout, &bytesize, 4) < 0){
      fprintf(stderr,"Something went wrong writing the integer to the file.");
      return 1;
    }

    // and go
    while (i < bytesize){

      // the goal is to always have seven 0's at the bottom of the buffer
      intbuffer |= ((unsigned long long)buffer[i] & 0x7f) << bitsinbuffer;
      bitsinbuffer += 7;
      i++;

      // 56 chars in buffer, dump them to output
      if (bitsinbuffer == 56){
	while (bitsinbuffer > 0){
	  output[j] |= intbuffer & 0xff;
	  intbuffer >>= 8;
	  bitsinbuffer -= 8;
	  j++;
	    
	}
     
	// reset intbuffer & bitsinbuffer just to be safe, and repeat
	intbuffer = 0;
	bitsinbuffer = 0;
      }
    }

    // when done, send any remaining bytes in intbuffer to output
    while (bitsinbuffer > 0){
      output[j] |= intbuffer & 0xff;
      intbuffer >>= 8;
      bitsinbuffer -=8;
      j++; 
    }    
    
    
    // and write the file! end ocmpression
    if (write(fdout, output, j) < 0){
      fprintf(stderr,
	      "Something went wrong. Stringlength: %d\n", (int)strlen(output));
    }
    // finally, remove the original file
    if (unlink(argv[1]) != 0){
      fprintf(stderr, "Something went wrong on file deletion.");
      }
  }

  
  // =================================================
  // DECOMPRESSION ALGORITHM
  // =================================================
  if (compressed) {
    
    // first, create the proper filename
    // I do this by using strtok to split the original string by '.'
    // Then concatenating '.txt' to the end
    
    char *newfilename = malloc(strlen(argv[1]) + 1);
    //sprintf(newfilename,"output.txt");
    sprintf(newfilename, "%s", argv[1]);
    strtok(newfilename,comp_ext);
    strcat(newfilename,".txt");
    int fdout = creat(newfilename, 0644);

    // iniialize variables
    
    unsigned long long intbuffer = 0;
    int bitsinbuffer = 0;
    int i = 0, j = 0;

    // read the first byte of the file to get its original size
    int filesize = (int)buffer[i];
    printf("filesize = %d\n", filesize);
    // the next 3 bytes are empty
    i = 4;
    output = (unsigned char*)malloc(r * 7 / 8);
    
    while (i <= bytesize){

      // by doing the opposite of what we did in the compression loop,
      // we can get the original data back
      intbuffer |= ((unsigned long long)buffer[i] & 0xff) << bitsinbuffer;
      bitsinbuffer += 8;
      i++;

      // consider bitbuffer full once it contains 56 bits
      // send contents to output
      if (bitsinbuffer == 56){
	while (bitsinbuffer > 0){
	  
	  output[j] = intbuffer & 0x7f;
	  intbuffer >>= 7;
	  bitsinbuffer -= 7;
	  j++;
	  
	}
	
	// reset these values before looping again,
	// just in case
	bitsinbuffer = 0;
	intbuffer = 0;
      }
    }

    // when done, send any remaining bits in bitbuffer
    // to the output buffer
    while (bitsinbuffer > 0){
      output[j] = intbuffer & 0x7f;
      intbuffer >>= 7;
      bitsinbuffer -= 7;
      j++;
    }
    

    // write the output buffer to file
    if (write(fdout, output, strlen(output)) < 0){
      fprintf(stderr,
	      "Something went wrong. Stringlength: %d\n",
	      (int)strlen(output));
    }
    // finally, remove the original file
    if (unlink(argv[1]) != 0){
      fprintf(stderr, "Something went wrong on file deletion.\n");
    }
  }
}



