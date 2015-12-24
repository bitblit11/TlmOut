
/*==============================================================================
** File Name: to_crc.c
**
** Title:    TO CRC Functions
**
** $Author: $
** $Revision: $
** $Date:  $
**
** Purpose:   To provide function for use with the CRC.
**
** Functions Contained:
**    fInitCRCTable - Creates the CRC Lookup Table
**    fCalcCRC - Calculates a CRC value
**
** Limitations, Assumptions, External Events, and Notes:
**  1.  None
**
** Modification History:
**   MM/DD/YY  SCR/SDR     Author          DESCRIPTION
**   --------  ----------  -------------   -----------------------------
**   mm/dd/yy  $$$SCRxxxx  C. Writer       Build #: Code Started
**
**
**==============================================================================
*/
#ifdef __cplusplus
extern "C" {
#endif

/*
** Pragmas
*/

/*
** Include Files
*/

#include "crc.h"
#include <stdio.h>
#include <stddef.h>

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/

unsigned short  usCRCTable[256]; /* contains pre-calcuated remainders */

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/


/*==============================================================================
** Name: to_crc.c
**
** Purpose: CRC Lookup table for efficient CRC calculations, the table
**			contains the remainder of each possible dividend.
**
** Arguments:
**     None
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   The CRC_POLYNOMIAL variable must be defined for the CRC.
**
**
** Routines Called:
**     None
**
**
** Called By:
**     None
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     None
**
** Global Outputs/Writes:
**     usCRCTable - Table to contain the values
**
** Programmer(s): Brian Butcher
**
** History:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
void fInitCRCTable(void)
{

	int i;
	int j;

	unsigned short usRemainder;
	unsigned short usData;


	/* calculate the remainder of each possible dividend */
	for (i=0; i<256; i++)
	{
		usRemainder = 0;
		usData = ((unsigned short) i) << 8;

		/* perform modulo-2 division a bit at a time */
		for (j=0; j<8; j++)
		{
			if ( (usRemainder ^ usData) & 0x8000 )
			{
				usRemainder = ( usRemainder << 1 ) ^ CRC_POLYNOMIAL;
			}
			else
			{
				usRemainder = usRemainder << 1;
			}

			usData = usData << 1;
		}

		/* place remainder in table */
    	usCRCTable[i] = usRemainder;
	}
}

/*==============================================================================
** Name:   fCalcCRC
**
** Purpose: Calculate the CRC16 CCITT value for the message.
**
** Arguments:
**      unsigned char* ucData - Pointer to a data block
**      int iOffset - Offset into message to begin calc
**      int iSize - Size of data block
**
** Returns:
**     unsigned short usCRC - CRC Value
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**     None
**
** Called By:
**     None
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     None
**
** Global Outputs/Writes:
**     None
**
** Programmer(s): Brian Butcher
**
** History:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
unsigned short fCalcCRC(  unsigned char* ucData, int iOffset, int iSize )
{
	unsigned short usCRC = INITIAL_CRC;
    unsigned short temp;
    unsigned short usData;
    int i = 0;

    if( ucData != NULL)
    {
		/*  Calculate the CRC starting at the byte after the offset for the
		 *  number of bytes specified in the input iSize param. The pointer
		 *  passed in is the complete frame include sync and reed-soloman at
		 *  the end and neither are part of the CRC calculation.
		 */
		for(i=iOffset;i<iSize+iOffset;i++)
		{
			usData  = 0x00ff & (unsigned short) ucData[i];

			temp = (usCRC >> 8) ^ usData;
			usCRC = (usCRC << 8) ^ usCRCTable[temp];

		}
    }

    return usCRC;

}

#ifdef __cplusplus
}
#endif

/* ---------- end of file to_crc.c ----------*/

