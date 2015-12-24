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

#ifndef _TO_PRIVATE_TYPES_H_
#define _TO_PRIVATE_TYPES_H_

#define "common_types.h"

/*
** Local Structure Declarations
*/
#if defined(_ix86_)

typedef struct
{
	uint16 spare:2;
	uint16 chann_id:6;
	uint16 cop_in_effect:2;
	uint16 status:3;
	uint16 version:2;
    uint16 word_type:1;

    uint16 value:8;
    uint16 rep_type:1;
    uint16 farm_b_ctr:2;
    uint16 retransmit:1;
    uint16 wait:1;
    uint16 lockout:1;
    uint16 no_bitlock:1;
    uint16 no_rf:1;

} RCM_CLCW_T;

#else

typedef struct
{
     uint16 word_type:1;
     uint16 version:2;
     uint16 status:3;
     uint16 cop_in_effect:2;
     uint16 chann_id:6;
     uint16 spare:2;

     uint16 no_rf:1;
     uint16 no_bitlock:1;
     uint16 lockout:1;
     uint16 wait:1;
     uint16 retransmit:1;
     uint16 farm_b_ctr:2;
     uint16 rep_type:1;
     uint16 value:8;

} RCM_CLCW_T;

#endif

typedef struct
{
     CCSDS_PriHdr_t               hdr;
     CCSDS_CmdSecHdr_t            sec_hdr;
     RCM_CLCW_T                  ClcwData;

} RCM_CLCW_TLM_T;

#define RCM_CLCW_MSG_LENGTH sizeof(RCM_CLCW_TLM_T)

/* Set File Descriptor Cmd Data Structure */
typedef struct
{
     CCSDS_PriHdr_t               hdr;
     CCSDS_CmdSecHdr_t            sec_hdr;
     int                          iSerialFD;

} RCM_SET_FD_T;

#define RCM_SET_FD_MSG_LENGTH sizeof(RCM_SET_FD_T)


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

#endif
