/*==============================================================================
** File Name: to_crc.h
**
** Title:  CRC Functions Header File
**
** $Author: $
** $Revision: $
** $Date:  $
**
** Purpose:   To provide crc #defines and prototypes
**
** Functions Contained:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**  1.   None
**
** Modification History:
**   MM/DD/YY  SCR/SDR     Author          DESCRIPTION
**   --------  ----------  -------------   -----------------------------
**   mm/dd/yy  $$$SCRxxxx  C. Writer       Build #: Code Started
**
**
**==============================================================================
*/

#ifndef __TO_CRC_H__
#define __TO_CRC_H__

/*
** Pragmas
*/

/*
** Include Files
*/

/*
** Local Defines
*/

#define INITIAL_CRC    0xFFFF
#define CRC_POLYNOMIAL 0x1021

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/

void fInitCRCTable(void);
unsigned short fCalcCRC( unsigned char* , int,int );

#endif /* __TO_CRC_H__ */

/* ---------- end of file to_crc.h ----------*/
