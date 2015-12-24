/*==============================================================================
** File Name: to_msg.h
**
** Title:     TO Message Header
**
** $Author: $
** $Revision: $
** $Date:  $
**
** Purpose: To define TO messages
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
