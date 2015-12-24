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

#ifndef _TO_TBL_
#define _TO_TBL_

/*
** Pragmas
*/

/*
** Include Files
*/
#include "cfe.h"
#include "to_mission_cfg.h"
#include "to_platform_cfg.h"
#include <arpa/inet.h>
#include "app.h"
#include "msg.h"


/*
** Local Defines
*/

#define TO_CONFIG_TABLENAME    "to_config"

typedef enum
{
	TO_QUEUE_UNUSED 	= 0,
	TO_QUEUE_DISABLED 	= 1,
	TO_QUEUE_ENABLED 	= 2
} TO_TLM_QueueMode_t;

typedef enum
{
	TO_CHANNEL_UNUSED 		= 0,
	TO_CHANNEL_DISABLED 	= 1,
	TO_CHANNEL_ENABLED 	= 2
} TO_TLM_ChannelMode_t;


/*
** Local Structure Declarations
*/

typedef struct
{
	CFE_SB_MsgId_t	MsgId;
	uint16          MsgLimit;
	uint32			ClassQueueIdx;
} TO_TlmSubscription_t;


typedef struct
{
	TO_TLM_QueueMode_t		Mode;
	uint32					Size;
	uint32					Quantum;
    uint32					ChannelIdx;
	uint32					GroupID;
	uint32					Deficit;
	CFE_SB_PipeId_t			PipeId;
	uint32					DeferredCount;
    CFE_SB_Msg_t        	*PktPtr;
} TO_TlmClassQueue_t;


typedef struct
{
	TO_TLM_ChannelMode_t	Mode;
	char					IP[INET_ADDRSTRLEN];
	uint32					DstPort;
	int32					Socket;
} TO_TlmChannels_t;


typedef struct
{
    uint32					MaxSubscriptions;
    uint32					PDUMessageID;
    uint32					Timeout;
    TO_TlmSubscription_t	Subscription[TO_MAX_TLM_SUBSCRIPTIONS];
    TO_TlmClassQueue_t		ClassQueue[TO_MAX_TLM_CLASS_QUEUES];
    TO_TlmChannels_t		Channel[TO_MAX_TLM_CHANNELS];
} TO_Table_t;

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


#endif /*_TO_TBL_*/

/* ---------- end of file to_tbl.h ----------*/
