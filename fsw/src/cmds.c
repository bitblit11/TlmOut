
/*==============================================================================
** File Name: to_app.c
**
** Title:  TO Telemetry Output Main .c File
**
** $Author: $
** $Revision: $
** $Date:  $
**
** Purpose:  To file contains all the functions for TO initialization and
**           routines to send telemetry data using either a socket or serial
**           port.
**
** Functions Contained:
**    function name   -       Brief purpose of function
**
** Limitations, Assumptions, External Events, and Notes:
**  1.   List assumptions that are made that apply to all functions in the
**       file.
**  2.   Provide the external source and events that can cause the
**       functions in this file to execute.
**  3.   List known limitations that apply to the functions in this file.
**  4.   If there are no assumptions, external events, or notes then
**       enter NONE.  Do not omit the section.
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



/*==============================================================================
** Name: TO_CmdExec
**
** Purpose: To determine the command code within the incoming command message
**          and call the correct function or set the corresponding variables.
**
** Arguments:
**     CFE_SB_MsgPtr_t cmd - Pointer to command message
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**     CFE_SB_GetCmdCode
**     CFE_EVS_SendEvent
**     TO_ResetStatus
**     TO_OutputDataTypesPkt
**     TO_AddPkt
**     TO_RemovePkt
**     TO_RemoveAllPkt
**     TO_StartSending
**     TO_SetTlmSerialFD
**
** Called By:
**     TO_ProcessCmdMsg
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     None
**
** Global Outputs/Writes:
**     TO_AppData.Frame.OutputRealTimeVCDU - Flag to output VCDU frames
**     TO_AppData.serial_downlink_on - Flag to indicate serial downlink on
**     TO_AppData.HkPacket.ucTOCmdCount - Command Counter
**     TO_AppData.HkPacket.ucTOCmdErrCount - Command error counter
**
** Programmer(s): Brian Butcher
**
** History:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
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



/*==============================================================================
** Name:  TO_ResetStatus
**
** Purpose: To reset HK status variables.
**
** Arguments:
**     None
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**     None
**
** Called By:
**     TO_CmdExec
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     None
**
** Global Outputs/Writes:
**     TO_AppData.HkPacket.ucTOCmdErrCount = 0;
**	   TO_AppData.HkPacket.ucTOCmdCount = 0;
**
** Programmer(s): Brian Butcher
**
** History:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
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



/*==============================================================================
** Name:  TO_OutputDataTypesPkt
**
** Purpose:  To output the data types telem packet. This is legacy CFS code and
**           was preserved in case we need it in the future for testing.
**
** Arguments:
**     None
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**
**
** Called By:
**     TO_CmdExec
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     None
**
** Global Outputs/Writes:
**     TO_AppData.DataTypesPacket
**
** Programmer(s):LRO Re-use
**
** History:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
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



/*==============================================================================
** Name:  TO_AddPkt
**
** Purpose: To subscribe for a parameterized packet id on the TLM PIPE. This
**          gives the flexibility of added packets to download in flight.
**
** Arguments:
**     TO_ADD_PKT_t * pCmd - Pointer to command message
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**
**
** Called By:
**     TO_CmdExec
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     TO_AppData.TlmPipe - Telemetry Pipe Identifier
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



/*=============================================================================
** Name:  TO_RemovePkt
**
** Purpose: To remove a specific packet from the TLM Pipe subscriptions.
**
** Arguments:
**     TO_ADD_PKT_t * pCmd - Pointer to command message
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**     CFE_SB_Unsubscribe
**     CFE_EVS_SendEvent
**
** Called By:
**     TO_CmdExec
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     TO_AppData.TlmPipe - Tlm pipe Identifier
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



/*==============================================================================
** Name: TO_RemoveAllPkt
**
** Purpose: To remove all message subscriptions from the TLM Pipe.
**
** Arguments:
**     None
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   Note: This will only remove the message id'd in the subscription
**             table, so if additional have been added via the AddPkt command
**             then they will remain. This could be bad and I may look into
**             removing it or at least the remove command aspect.
**
** Routines Called:
**     CFE_SB_Unsubscribe
**     CFE_EVS_SendEvent
**
** Called By:
**     None
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     TO_SubTable[].MessageId
**     TO_AppData.TlmPipe
**     TO_AppData.CmdPipe
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


/*==============================================================================
** Name: TO_StartSending
**
** Purpose: To setup the ethernet sockets using the IP address from the
**          incoming command that triggered the function execution.
**
** Arguments:
**     TO_OUTPUT_ENABLE_PKT_t - Enable socket tlm command msg struct
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**     TO_OpenSockTlm
**
** Called By:
**     TO_CmdExec
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     TO_AppData.downlink_on - Flag to indicate if socket already setup
**
** Global Outputs/Writes:
**     TO_AppData.cTlmDestIP - Destination IP Address
**     TO_AppData.suppress_sendto - Flag to suppress telemetry
**     TO_AppData.downlink_on - Flag to indicate if sockets opened
**     TO_AppData.Frame.OutputRealTimeVCDU - Flag to begin sending VCDU frames
**
** Programmer(s): Brian Butcher
**
** History:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
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
