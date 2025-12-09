/*--------------------------------------------------------------------*/
/* miniassembler.c                                                    */
/* Author: Bob Dondero, Donna Gabai, Zara Hommez                      */
/*--------------------------------------------------------------------*/

#include "miniassemblertest.h"
#include <assert.h>
#include <stddef.h>
/* ask about implementation specifications */
/*--------------------------------------------------------------------*/
/* Modify *puiDest in place,
   setting uiNumBits starting at uiDestStartBit (where 0 indicates
   the least significant bit) with bits taken from uiSrc,
   starting at uiSrcStartBit.
   uiSrcStartBit indicates the rightmost bit in the field.
   setField sets the appropriate bits in *puiDest to 1.
   setField never unsets any bits in *puiDest.                        */
static void setField(unsigned int uiSrc, unsigned int uiSrcStartBit,
                     unsigned int *puiDest, unsigned int uiDestStartBit,
                     unsigned int uiNumBits)
{
   unsigned int uiBase;
   unsigned int uiSrcBits;
   
   /* Create a base for the number of bits (111..# bits) to get from uiSrc*/
   uiBase = (1U << uiNumBits) - 1;
   
   /* gets the bits from uiSrc starting at uiSrcStartBit, truncating 
   off higher bit through uiBase*/
   uiSrcBits = (uiSrc >> uiSrcStartBit) & uiBase;
   
   /* out bits into puiDest, starting at uiDestStartBit, using 
   the or to not unset the already present bits */
   *puiDest |= (uiSrcBits << uiDestStartBit);
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_mov(unsigned int uiReg, int iImmed)
{
   unsigned int uiInstr;
   
   /* sf = 0 -> 32 bit, default shift*/
   /* Base Instruction Code*/
   uiInstr = 0x52800000;
   
   /* register to be inserted in instruction */
   setField(uiReg, 0, &uiInstr, 0, 5);
   
   /* insert the 16-bit immediate value */
   setField((unsigned int)iImmed, 0, &uiInstr, 5, 16);
   
   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_adr(unsigned int uiReg, unsigned long ulAddr,
   unsigned long ulAddrOfThisInstr)
{
   unsigned int uiInstr;
   unsigned int uiDisp;

   /* Base Instruction Code */
   uiInstr = 0x10000000;

   /* register to be inserted in instruction */
   setField(uiReg, 0, &uiInstr, 0, 5);

   /* displacement to be split into immlo and immhi and inserted */
   uiDisp = (unsigned int)(ulAddr - ulAddrOfThisInstr);

   setField(uiDisp, 0, &uiInstr, 29, 2);
   setField(uiDisp, 2, &uiInstr, 5, 19);

   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_strb(unsigned int uiFromReg,
   unsigned int uiToReg)
{
   unsigned int uiInstr;
   
   /* Base Instruction Code */
   uiInstr = 0x39000000;
   
   /* Insert the fromreg, W register */
   setField(uiFromReg, 0, &uiInstr, 0, 5);
   
   /* Insert the toreg, X register*/
   setField(uiToReg, 0, &uiInstr, 5, 5);
   
   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_b(unsigned long ulAddr,
   unsigned long ulAddrOfThisInstr)
{
   unsigned int uiInstr;
   unsigned int uiDisp;
   
   /* Base Instruction Code*/
   uiInstr = 0x14000000;

   /* calculate jump difference between instructs */
   uiDisp = (unsigned int)(ulAddr - ulAddrOfThisInstr);
   /* drop the last two bits since instructions are 4 byte aligned*/
   /* insert 26-bit immediate into correct slot*/
   setField(uiDisp, 2, &uiInstr, 0, 26);
   
   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_bl(unsigned long ulAddr,
   unsigned long ulAddrOfThisInstr)
{
   unsigned int uiInstr;
   unsigned int uiDisp;

   /* Base Instruction Code for BL */
   uiInstr = 0x94000000;

   /* calculate jump difference between instructs */
   uiDisp = (unsigned int)(ulAddr - ulAddrOfThisInstr);
   /* drop the last two bits since instructions are 4 byte aligned*/
   /* insert 26-bit immediate into correct slot*/
   setField(uiDisp, 2, &uiInstr, 0, 26);

   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_strb_imm(unsigned int uiFromReg,
   unsigned int uiToReg, unsigned int uiImm12)
{
   unsigned int uiInstr;

   /* Base Instruction Code for STRB (unsigned immediate) */
   uiInstr = 0x39000000;

   /* imm12 at bits [21:10] */
   setField(uiImm12, 0, &uiInstr, 10, 12);

   /* register to be inserted: Rt (W register) at bits [4:0] */
   setField(uiFromReg, 0, &uiInstr, 0, 5);

   /* base register Rn (X register) at bits [9:5] */
   setField(uiToReg, 0, &uiInstr, 5, 5);

   return uiInstr;
}
