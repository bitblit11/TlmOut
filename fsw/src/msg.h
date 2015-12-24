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

#ifndef _TO_MSG_H_
#define _TO_MSG_H_

/*
** Pragmas
*/

/*
** Include Files
*/

#include "cfe.h"
#include "to_mission_cfg.h"
#include <netinet/in.h>

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

typedef struct
{
    uint8      	CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32     	PrioMask;
}  TO_SendTlm_t;

typedef struct
{
    uint8      		CmdHeader[CFE_SB_CMD_HDR_SIZE];
	CFE_SB_MsgId_t	MsgId;
	uint16          MsgLimit;
	uint16			ClassQueueIdx;
}  TO_ADD_PKT_t;

typedef struct
{
    uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
    CFE_SB_MsgId_t     MsgId;
}  TO_REMOVE_PKT_t;


typedef struct
{
	char					IP[INET_ADDRSTRLEN];
	uint32					Port;
} TO_TlmChannelCmd_t;


typedef struct
{
    uint8              		CmdHeader[CFE_SB_CMD_HDR_SIZE];
    TO_TlmChannelCmd_t		Channel[TO_MAX_TLM_CHANNELS];
}  TO_OUTPUT_ENABLE_PKT_t;


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

#endif /* _TO_MSG_H_ */

/* ---------- end of file to_msg.h ----------*/
