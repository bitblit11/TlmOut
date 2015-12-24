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
#include "to_mission_cfg.h"
#include "app.h"
#include "cmds.h"
#include "events.h"
#include "cmd_codes.h"



/*
** Local Defines
*/

/*
** Local Structure Declarations
*/



/*
** External Global Variables
*/
extern TO_AppData_T TO_AppData;



/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/



void TO_CmdExec(CFE_SB_MsgPtr_t msg)
{
    uint16 CommandCode;
    boolean bValid = TRUE;

    /* Get Command Code */
    CommandCode = CFE_SB_GetCmdCode(msg);

    switch (CommandCode)
    {
       case TO_NOP_CC:
            CFE_EVS_SendEvent(TO_NOOP_INF_EID, CFE_EVS_INFORMATION,
            		"TO: No-op Command Rcvd");
            break;

       case TO_RESET_STATUS_CC:
            TO_ResetStatus();
            break;

       case TO_SEND_DATA_TYPES_CC:
            TO_OutputDataTypesPkt();
            break;

       case TO_ADD_PKT_CC:
            TO_AddPkt((TO_ADD_PKT_t *)msg);
            break;

       case TO_REMOVE_PKT_CC:
            TO_RemovePkt( (TO_REMOVE_PKT_t *)msg);
            break;

       case TO_REMOVE_ALL_PKT_CC:
            TO_RemoveAllPkt();
            break;

       case TO_OUTPUT_ENABLE_CC:
 	        CFE_EVS_SendEvent(TO_NOOP_INF_EID,CFE_EVS_INFORMATION,
 	        		"TO: Enable Ethernet Command Rcvd");
            TO_EnableDownlink( (TO_OUTPUT_ENABLE_PKT_t *)msg );

            break;

       default:
            CFE_EVS_SendEvent(TO_FNCODE_ERR_EID,CFE_EVS_ERROR,
            		  "TO: Invalid Function Code Rcvd In Ground Command 0x%x",
                      CommandCode);
            bValid = FALSE;
    }

    if (bValid)
    {
       ++TO_AppData.HkPacket.CmdCount;
    }
    else
    {
       ++TO_AppData.HkPacket.CmdErrCount;
    }
}



void TO_ResetStatus(void)
{
	TO_AppData.HkPacket.CmdCount = -1;
	TO_AppData.HkPacket.CmdErrCount = 0;
    TO_AppData.HkPacket.IngestMsgCount = 0;
    TO_AppData.HkPacket.IngestErrorCount = 0;
    TO_AppData.HkPacket.FDPdusDropped = 0;
    TO_AppData.HkPacket.EOFPdusDropped = 0;
    TO_AppData.HkPacket.FINPdusDropped = 0;
    TO_AppData.HkPacket.ACKPdusDropped = 0;
    TO_AppData.HkPacket.MDPdusDropped = 0;
    TO_AppData.HkPacket.NAKPdusDropped = 0;
    TO_AppData.HkPacket.PDUsCaptured = 0;
}



void TO_OutputDataTypesPkt(void)
{
    uint16 i = 0;
    char string_variable[] = "ABCDEFGHIJ";

    /* initialize data types packet */
    CFE_SB_InitMsg(&TO_AppData.DataTypesPacket, TO_DATA_TYPES_MID,
    		sizeof(TO_AppData.DataTypesPacket), TRUE);

    CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &TO_AppData.DataTypesPacket);

    /* initialize the packet data */
    TO_AppData.DataTypesPacket.synch = 0x6969;
    
    TO_AppData.DataTypesPacket.bit1 = 1;
    TO_AppData.DataTypesPacket.bit2 = 0;
    TO_AppData.DataTypesPacket.bit34 = 2;
    TO_AppData.DataTypesPacket.bit56 = 3;
    TO_AppData.DataTypesPacket.bit78 = 1;
    TO_AppData.DataTypesPacket.nibble1 = 0xA;
    TO_AppData.DataTypesPacket.nibble2 = 0x4;
  
    TO_AppData.DataTypesPacket.bl1 = FALSE;
    TO_AppData.DataTypesPacket.bl2 = TRUE;
    TO_AppData.DataTypesPacket.b1 = 16;
    TO_AppData.DataTypesPacket.b2 = 127;
    TO_AppData.DataTypesPacket.b3 = 0x7F;
    TO_AppData.DataTypesPacket.b4 = 0x45;
    TO_AppData.DataTypesPacket.w1 = 0x2468;
    TO_AppData.DataTypesPacket.w2 = 0x7FFF;
    TO_AppData.DataTypesPacket.dw1 = 0x12345678;
    TO_AppData.DataTypesPacket.dw2 = 0x87654321;
    TO_AppData.DataTypesPacket.f1 = 90.01;
    TO_AppData.DataTypesPacket.f2 = .0000045;
    TO_AppData.DataTypesPacket.df1 = 99.9;
    TO_AppData.DataTypesPacket.df2 = .4444;

    for(i=0; i < 10; i++)
    {
    	TO_AppData.DataTypesPacket.str[i] = string_variable[i];
    }

    CFE_SB_SendMsg((CFE_SB_Msg_t *)&TO_AppData.DataTypesPacket);
}



void TO_AddPkt( TO_ADD_PKT_t * cmd)
{
	int32  iStatus = CFE_SUCCESS;
    uint32 i = 0;
    int32 index = -1;
    boolean found = FALSE;
    boolean free_room = FALSE;

    if(cmd->ClassQueueIdx >= TO_MAX_TLM_CLASS_QUEUES)
    {
    	TO_AppData.HkPacket.CmdErrCount++;
        CFE_EVS_SendEvent(TO_ADDPKT_ERR_EID, CFE_EVS_ERROR,
        		"Failed to add message 0x%04x.  Class queue index out of bounds.");
    }

    if((TO_AppData.Config->ClassQueue[cmd->ClassQueueIdx].Mode != TO_QUEUE_DISABLED) &&
       (TO_AppData.Config->ClassQueue[cmd->ClassQueueIdx].Mode != TO_QUEUE_ENABLED))
    {
    	TO_AppData.HkPacket.CmdErrCount++;
        CFE_EVS_SendEvent(TO_ADDPKT_ERR_EID, CFE_EVS_ERROR,
        		"Failed to add message 0x%04x.  Class queue index unused.");
    }

    /* Search for the corresponding entry in the subscription table to ensure
     * its not already added.
     */
    for(i=0; i < TO_AppData.Config->MaxSubscriptions; i++)
    {
    	if(TO_AppData.Config->Subscription[i].MsgId == cmd->MsgId )
    	{
    		found = TRUE;
            break;
        }
    	else if(TO_AppData.Config->Subscription[i].MsgId == TO_UNUSED)
        {
            free_room = TRUE;
        
            if (index == -1)
            {
                index = i;
            }
        }
    }

    if(found == TRUE)
    {
    	TO_AppData.HkPacket.CmdErrCount++;
        CFE_EVS_SendEvent(TO_ADDPKT_ERR_EID, CFE_EVS_ERROR,
        		"Failed to add message 0x%04x.  Message already added.",
        		cmd->MsgId);
    }
    else if(free_room == FALSE)
    {
    	TO_AppData.HkPacket.CmdErrCount++;
        CFE_EVS_SendEvent(TO_ADDPKT_ERR_EID, CFE_EVS_ERROR,
        		"Failed to add message 0x%04x.  Reached maximum subscriptions.",
        		cmd->MsgId);
    }
    else
    {
    	CFE_SB_Qos_t qos = {0,0};

        /* Subcribe for the new message id on the TLM pipe */
    	iStatus = CFE_SB_SubscribeEx(cmd->MsgId, TO_AppData.Config->ClassQueue[cmd->ClassQueueIdx].PipeId,
    			qos, cmd->MsgLimit);
        if(iStatus != CFE_SUCCESS)
        {
        	TO_AppData.HkPacket.CmdErrCount++;
        	CFE_EVS_SendEvent(TO_ADDPKT_ERR_EID, CFE_EVS_ERROR,
        			"Failed to add message 0x%04x.  CFE_SB_SubscribeEx() returned %i.",
        			cmd->MsgId, iStatus);
        }
        else
        {
            TO_AppData.HkPacket.PktSubCount++;
            TO_AppData.Config->Subscription[index].MsgId = cmd->MsgId;
            TO_AppData.Config->Subscription[index].MsgLimit = cmd->MsgLimit;
            CFE_EVS_SendEvent(TO_ADDPKT_INF_EID,CFE_EVS_INFORMATION,
    		             "Added message 0x%04x with buffer depth %u.",
                         cmd->MsgId,
                         cmd->MsgLimit);
        }
    }
}



void TO_RemovePkt(TO_REMOVE_PKT_t * pCmd)
{
	int32  iStatus = CFE_SUCCESS;
    uint32 i = 0;
    boolean found = FALSE;

    /* Search for the corresponding entry in the subscription table */
    for(i=0; i < TO_AppData.Config->MaxSubscriptions; i++)
    {
    	if(TO_AppData.Config->Subscription[i].MsgId == pCmd->MsgId )
    	{
            found = TRUE;
    		iStatus = CFE_SB_Unsubscribe(
    				TO_AppData.Config->Subscription[i].MsgId,
    				TO_AppData.Config->ClassQueue[TO_AppData.Config->Subscription[i].ClassQueueIdx].PipeId);

    		if(iStatus != CFE_SUCCESS)
    		{
        	    TO_AppData.HkPacket.CmdErrCount++;
    			CFE_EVS_SendEvent(TO_REMOVEPKT_ERR_EID, CFE_EVS_ERROR,
    					"Failed to unsubscribe from MID 0x%04x.  CFE_SB_Unsubscribe() returned %i.",
    					TO_AppData.Config->Subscription[i].MsgId, iStatus);
    		}
    		else
    		{
        	    TO_AppData.HkPacket.PktSubCount--;
    			TO_AppData.Config->Subscription[i].MsgId = TO_UNUSED;
    			TO_AppData.Config->Subscription[i].MsgLimit = 0;
                CFE_EVS_SendEvent(TO_REMOVEPKT_INF_EID, CFE_EVS_INFORMATION,
                        "Unsubscribed message 0x%04x.", pCmd->MsgId);
                break;
            }
    	}
    }

    if  (found == FALSE)
    {
        CFE_EVS_SendEvent(TO_REMOVEPKT_ERR_EID, CFE_EVS_ERROR,
                "Cannot unsubscribe 0x%04x. Not currently subscribed.", pCmd->MsgId);
    }
}



void TO_RemoveAllPkt(void)
{
	int32  iStatus = CFE_SUCCESS;
    uint32 i = 0;

    /* Iterate through and unsubscribe from each entry in the subscription
     * table
     */
    for(i=0; i < TO_AppData.Config->MaxSubscriptions; i++)
    {
    	if(TO_AppData.Config->Subscription[i].MsgId != TO_UNUSED )
    	{
    		iStatus = CFE_SB_Unsubscribe(
    				TO_AppData.Config->Subscription[i].MsgId,
    				TO_AppData.Config->ClassQueue[TO_AppData.Config->Subscription[i].ClassQueueIdx].PipeId);

    		if(iStatus != CFE_SUCCESS)
    		{
        	    TO_AppData.HkPacket.CmdErrCount++;
    			CFE_EVS_SendEvent(TO_REMOVEALLPTKS_ERR_EID, CFE_EVS_ERROR,
    					"Failed to unsubscribe from MID 0x%04x.  CFE_SB_Unsubscribe() returned %i.",
    					TO_AppData.Config->Subscription[i].MsgId, iStatus);
    		}
            else
            {
        	    TO_AppData.HkPacket.PktSubCount--;
    			TO_AppData.Config->Subscription[i].MsgId = TO_UNUSED;
    			TO_AppData.Config->Subscription[i].MsgLimit = 0;
            }
    	}
    }

    CFE_EVS_SendEvent(TO_REMOVEALLPKTS_INF_EID, CFE_EVS_INFORMATION,
    		"Unsubscribed all telemetry");
}



void TO_EnableDownlink( TO_OUTPUT_ENABLE_PKT_t * cmd )
{
	uint32 i=0;

    for (i=0; i < TO_MAX_TLM_CHANNELS; i++)
    {
    	if((TO_AppData.Config->Channel[i].Mode == TO_CHANNEL_DISABLED) ||
    	   (TO_AppData.Config->Channel[i].Mode == TO_CHANNEL_ENABLED))
    	{
        	strncpy(TO_AppData.Config->Channel[i].IP, cmd->Channel[i].IP, sizeof(cmd->Channel[i].IP));
        	TO_AppData.Config->Channel[i].DstPort = cmd->Channel[i].Port;

			if((TO_AppData.Config->Channel[i].Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
			{
				TO_AppData.Config->Channel[i].Mode = TO_CHANNEL_DISABLED;
				CFE_EVS_SendEvent(TO_TLMOUTSOCKET_ERR_EID,CFE_EVS_ERROR,
					   "TO: TLM socket errno: %i on channel %u", errno, i);
				continue;
			}

			CFE_EVS_SendEvent(TO_TLMOUTENA_INF_EID, CFE_EVS_INFORMATION,
							  "TO UDP telemetry output enabled channel %u to %s:%u",
							  i, TO_AppData.Config->Channel[i].IP, TO_AppData.Config->Channel[i].DstPort);
			TO_AppData.Config->Channel[i].Mode = TO_CHANNEL_ENABLED;
    	}
    }
}



#ifdef __cplusplus
}
#endif
