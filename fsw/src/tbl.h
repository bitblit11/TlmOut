/*==============================================================================
** File Name: to_tbl.h
**
** Title: TO iLoad Table Header File
**
** $Author: $
** $Revision: $
** $Date:  $
**
** Purpose:   To define the iLoad data structure definition.
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
