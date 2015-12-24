/*==============================================================================
Copyright (c) 2015, Windhover Labs
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of TlmOut nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

