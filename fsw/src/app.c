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

#include "cfe.h"
#include "to_mission_cfg.h"
#include "app.h"
#include "msg.h"
#include "to_platform_cfg.h"
#include "events.h"
#include "version.h"
#include <termio.h>
#include <stdlib.h>

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

TO_AppData_T TO_AppData;

/*
** Local Variables
*/


/*
** Local Function Prototypes
*/
void TO_SendToChannel(uint32 ChannelID, CFE_SB_MsgPtr_t Msg, uint32 Size);



void TO_AppMain(void)
{
	int32     iStatus;

	CFE_ES_PerfLogEntry(TO_MAIN_TASK_PERF_ID);

	/* Initialize the TO app*/
	iStatus = TO_AppInit();

	if (iStatus != CFE_SUCCESS)
	{
		TO_AppData.RunStatus= CFE_ES_APP_ERROR;
	}

	/* Application Main Loop */
	while(CFE_ES_RunLoop(&TO_AppData.RunStatus) == TRUE)
	{
		/* Call RecvMsg to pend on sch wakeup msg */
   		TO_RecvMsg(TO_WAKEUP_TIMEOUT);
	}

	/*  Exit the Application */
	CFE_ES_ExitApp(TO_AppData.RunStatus);
}



int32 TO_RecvMsg( int32 timeout )
{
	int32 iStatus ;

    /*  Performance Log Exit Stamp */
    CFE_ES_PerfLogExit(TO_MAIN_TASK_PERF_ID);

    /* Wait for SCH software bus Wakeup or HK message */
    iStatus = CFE_SB_RcvMsg(&TO_AppData.MsgPtr,
    		                 TO_AppData.SchPipe,
    		                 timeout);
    /* Message received now process message */
    if (iStatus == CFE_SUCCESS)
    {
        TO_ProcessSchMsg();
    }
    else if ( iStatus == CFE_SB_TIME_OUT )
	{
    	/* SCH was probably unloaded or the entry was disabled, causing the
    	 * wakeup to timeout.  Rather than completely blackout all telemetry
    	 * we'll go ahead and send telemetry after the timeout anyway.
    	 */
        TO_OutputStatus();
        TO_ForwardTelemetry(TO_AppData.Config->Timeout);
    }
    else if ( iStatus == CFE_SB_NO_MESSAGE )
    {
        /* nothing to be done if no message is present */
    }

    /* Performance Log Exit Stamp */
    CFE_ES_PerfLogEntry(TO_MAIN_TASK_PERF_ID);

    return iStatus ;

}



void TO_Cleanup(void)
{
	uint32 i=0;

    /* If sockets connected close the sockets */
	if(TO_AppData.Config != 0)
	{
		for(i=0; i < TO_MAX_TLM_CHANNELS; i++)
		{
			if(TO_AppData.Config->Channel[i].Mode == TO_CHANNEL_ENABLED)
			{
				close(TO_AppData.Config->Channel[i].Socket);
			}
		}
	}
}



int32 TO_AppInit(void)
{
    int32            iStatus;
    uint16           i;

    TO_AppData.RunStatus = CFE_ES_APP_RUN;

    /* Register the TO app */
    CFE_ES_RegisterApp();

    /*  Setup CMD Pipes */
    TO_AppData.CmdPipeDepth = TO_CMD_PIPE_DEPTH ;
    strcpy(TO_AppData.CmdPipeName,  "TO_CMD_PIPE");

    TO_AppData.SchPipeDepth = TO_SCH_PIPE_DEPTH;
    strcpy(TO_AppData.SchPipeName,  "TO_SCH_PIPE");

    /* Initialize event filter table */
    TO_AppData.EventFilters[0].EventID = TO_INIT_INF_EID;
    TO_AppData.EventFilters[0].Mask    = CFE_EVS_NO_FILTER;
    TO_AppData.EventFilters[1].EventID = TO_CTODPIPE_ERR_EID;
    TO_AppData.EventFilters[1].Mask    = CFE_EVS_NO_FILTER;
    TO_AppData.EventFilters[2].EventID = TO_SUBSCRIBE_ERR_EID;
    TO_AppData.EventFilters[2].Mask    = CFE_EVS_NO_FILTER;
    TO_AppData.EventFilters[3].EventID = TO_TLMOUTSOCKET_ERR_EID;
    TO_AppData.EventFilters[3].Mask    = CFE_EVS_NO_FILTER;
    TO_AppData.EventFilters[4].EventID = TO_TLMOUTSTOP_ERR_EID;
    TO_AppData.EventFilters[4].Mask    = CFE_EVS_NO_FILTER;
    TO_AppData.EventFilters[5].EventID = TO_MSGID_ERR_EID;
    TO_AppData.EventFilters[5].Mask    = CFE_EVS_NO_FILTER;
    TO_AppData.EventFilters[6].EventID = TO_FNCODE_ERR_EID;
    TO_AppData.EventFilters[6].Mask    = CFE_EVS_EVERY_OTHER_TWO;
    TO_AppData.EventFilters[7].EventID = TO_NOOP_INF_EID;
    TO_AppData.EventFilters[7].Mask    = CFE_EVS_NO_FILTER;

    /* Register event filter table */
    iStatus = CFE_EVS_Register(TO_AppData.EventFilters,
    		                   TO_EVT_COUNT,
    		                   CFE_EVS_BINARY_FILTER);
    if ( iStatus != CFE_SUCCESS )
    {
    	CFE_ES_WriteToSysLog("TO App: Error Registering Events");
    }

    /* Initialize housekeeping packet (clear user data area) */
    CFE_SB_InitMsg(&TO_AppData.HkPacket,
                    TO_HK_TLM_MID,
                    sizeof(TO_AppData.HkPacket), TRUE);

    /* Subscribe for Commands on the Command Pipe */
    iStatus = CFE_SB_CreatePipe(&TO_AppData.CmdPipe,
    		                    TO_AppData.CmdPipeDepth,
    		                    TO_AppData.CmdPipeName);
    if (iStatus == CFE_SUCCESS)
    {
    	CFE_SB_Subscribe(TO_CMD_MID, TO_AppData.CmdPipe);
    }
    else
    {
    	CFE_EVS_SendEvent(TO_CTODPIPE_ERR_EID,CFE_EVS_ERROR,
    		             "TO: Can't create cmd pipe status %i",iStatus);
    }

    /* Subscribe for the Wakeup and Send_HK messages on the SCH Pipe*/
    iStatus = CFE_SB_CreatePipe(&TO_AppData.SchPipe,
      		                    TO_AppData.SchPipeDepth,
       		                    TO_AppData.SchPipeName);
    if (iStatus == CFE_SUCCESS)
    {
    	CFE_SB_Subscribe(TO_SEND_HK_MID, TO_AppData.SchPipe);
    	CFE_SB_Subscribe(TO_SEND_TLM_MID,  TO_AppData.SchPipe);
    }
    else
    {
    	CFE_EVS_SendEvent(TO_CTODPIPE_ERR_EID,CFE_EVS_ERROR,
    	                 "TO: Can't create cmd pipe status %i",iStatus);
    }

    /* Install the delete handler */
    OS_TaskInstallDeleteHandler((void *)(&TO_Cleanup));

    /* Register and Load iLoad Table */
    iStatus = TO_TableInit();
    if(iStatus != CFE_SUCCESS)
    {
    	goto end_of_function;
    }

    /*  Setup TLM Pipes */
    for (i=0; i < TO_MAX_TLM_CLASS_QUEUES; i++)
    {
    	if(TO_AppData.Config->ClassQueue[i].Mode == TO_QUEUE_ENABLED)
        {
    		char pipeName[OS_MAX_API_NAME];

    		sprintf(pipeName, "TO_CLSQUEUE_%u", i);
    		iStatus = CFE_SB_CreatePipe(&TO_AppData.Config->ClassQueue[i].PipeId,
    				TO_AppData.Config->ClassQueue[i].Size, pipeName);
    	    if (iStatus != CFE_SUCCESS)
    	    {
    	    	CFE_EVS_SendEvent(TO_TLMPIPE_ERR_EID,CFE_EVS_ERROR,
    	    		             "TO: Can't create Tlm pipe status %i",iStatus);
    	    }
        }
    }

    /* Subscriptions for TLM pipe*/
    for (i=0; i < TO_AppData.Config->MaxSubscriptions; i++)
    {
    	if(TO_AppData.Config->Subscription[i].MsgId != TO_UNUSED )
    	{
    		CFE_SB_Qos_t qos = {0,0};

    		uint32 clsQueueIdx = TO_AppData.Config->Subscription[i].ClassQueueIdx;
            iStatus = CFE_SB_SubscribeEx(TO_AppData.Config->Subscription[i].MsgId,
            		TO_AppData.Config->ClassQueue[clsQueueIdx].PipeId,
					qos,
        		    TO_AppData.Config->Subscription[i].MsgLimit);
    		if (iStatus != CFE_SUCCESS)
    		{
    			CFE_EVS_SendEvent(TO_SUBSCRIBE_ERR_EID,CFE_EVS_ERROR,
        		             "TO: Can't subscribe to MID 0x%x status 0x%08X  index %u",
        		             TO_AppData.Config->Subscription[i].MsgId, iStatus, i);
    		}
            else
            {
                TO_AppData.HkPacket.PktSubCount++;
            }
    	}
    }

    for (i=0; i < TO_MAX_TLM_CHANNELS; i++)
    {
    	if(TO_AppData.Config->Channel[i].Mode == TO_CHANNEL_ENABLED)
    	{
			struct sockaddr_in servaddr;
			int status;
			int reuseaddr=1;

			if((TO_AppData.Config->Channel[i].Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
			{
				TO_AppData.Config->Channel[i].Mode = TO_CHANNEL_DISABLED;
				CFE_EVS_SendEvent(TO_TLMOUTSOCKET_ERR_EID,CFE_EVS_ERROR,
					   "TO: TLM socket errno: %i on channel %u", errno, i);
				continue;
			}

			setsockopt(TO_AppData.Config->Channel[i].Socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr,sizeof(reuseaddr));

			bzero(&servaddr,sizeof(servaddr));
			servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
			servaddr.sin_port=0;
			status = bind(TO_AppData.Config->Channel[i].Socket, (struct sockaddr *)&servaddr,sizeof(servaddr));
			if(status < 0)
			{
				CFE_EVS_SendEvent(TO_TLMOUTSOCKET_ERR_EID, CFE_EVS_ERROR,
						   "TO: TLM bind errno: %i on channel %u", errno, i);
				TO_AppData.Config->Channel[i].Mode = TO_CHANNEL_DISABLED;
				continue;
			}

			CFE_EVS_SendEvent(TO_TLMOUTENA_INF_EID, CFE_EVS_INFORMATION,
							  "TO UDP telemetry output enabled channel %u to %s:%u",
							  i, TO_AppData.Config->Channel[i].IP, TO_AppData.Config->Channel[i].DstPort);

    	}
    }

    TO_AppData.DropFileData = FALSE;
    TO_AppData.DropEOF = FALSE;
    TO_AppData.DropFIN = FALSE;
	TO_AppData.DropACK = FALSE;
	TO_AppData.DropMetaData = FALSE;
	TO_AppData.DropNAK = FALSE;
	TO_AppData.CorruptChecksum = FALSE;
	TO_AppData.AdjustFileSize = FALSE;

    CFE_EVS_SendEvent (TO_INIT_INF_EID, CFE_EVS_INFORMATION,
               "TO Initialized. Version %d.%d.%d.%d Awaiting enable command.",
                TO_MAJOR_VERSION,
                TO_MINOR_VERSION,
                TO_REVISION,
                TO_MISSION_REV);

end_of_function:
    return iStatus;
}



void TO_ProcessSchMsg(void)
{
	CFE_SB_MsgId_t  MsgId;

	MsgId = CFE_SB_GetMsgId(TO_AppData.MsgPtr);
    switch (MsgId)
    {
    	case TO_SEND_TLM_MID:
    	{
    		/* Ouput Telemetry */
    		TO_SendTlm_t *msg = (TO_SendTlm_t *)TO_AppData.MsgPtr;
            TO_ForwardTelemetry(msg->PrioMask);
            break;
    	}

        case TO_SEND_HK_MID:
    	{
             TO_OutputStatus();

             break;
    	}

        default:
    	{
             CFE_EVS_SendEvent(TO_MSGID_ERR_EID,CFE_EVS_ERROR,
            		           "TO: Sch Pipe Invalid Msg ID Rcvd 0x%x",MsgId);
             break;
    	}
    }
}



void TO_ProcessCmdMsg(void)
{
    CFE_SB_Msg_t    *MsgPtr;
    CFE_SB_MsgId_t  MsgId;

    while(1)    /* While loop to execute until all commands received */
    {
       /* Check for message on cmd pipe and process */
       switch (CFE_SB_RcvMsg(&MsgPtr, TO_AppData.CmdPipe, CFE_SB_POLL))
       {
          case CFE_SUCCESS:

               MsgId = CFE_SB_GetMsgId(MsgPtr);

               switch (MsgId)
               {
                  case TO_CMD_MID:
                	   /* Call Command Exec Function */
                       TO_CmdExec(MsgPtr);
                       break;

                  default:
                       CFE_EVS_SendEvent(TO_MSGID_ERR_EID,CFE_EVS_ERROR,
                    		         "TO: Cmd Pipe Invalid Msg ID Rcvd 0x%x",
                    		         MsgId);
                       break;
               }
               break;
            default:
               /* Exit command processing loop if no message received. */
               return;
       }
    }
}



void TO_OutputStatus(void)
{
	/* Timestamp Message */
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &TO_AppData.HkPacket);

    CFE_SB_SendMsg((CFE_SB_Msg_t *) &TO_AppData.HkPacket);
}



void TO_ForwardTelemetry(uint32 PrioMask)
{
    int32   cfeStatus;
    uint32	msgSize;
    uint32	Surplus = 0;
    uint32 	i=0;

    for(i=0; i < TO_MAX_TLM_CLASS_QUEUES; i++)
    {
    	TO_TlmClassQueue_t *clsQueue = &TO_AppData.Config->ClassQueue[i];

    	uint32 budget = clsQueue->Quantum + clsQueue->Deficit + Surplus;
    	while(1)
    	{
    		if(TO_AppData.Config->ClassQueue[i].PktPtr == 0)
    		{
				cfeStatus = CFE_SB_RcvMsg(&TO_AppData.Config->ClassQueue[i].PktPtr, clsQueue->PipeId, CFE_SB_POLL);
				if((cfeStatus != CFE_SUCCESS))
				{
					clsQueue->Deficit = 0;
					Surplus = budget;
					break;
				}
    		}

    		if(i == 3)
    		{
    			OS_printf("Video telemetry out.");
    		}

    		msgSize = CFE_SB_GetTotalMsgLength(TO_AppData.Config->ClassQueue[i].PktPtr);
			if(msgSize <= budget)
			{
				TO_SendToChannel(TO_AppData.Config->ClassQueue[i].ChannelIdx,
						TO_AppData.Config->ClassQueue[i].PktPtr,
						msgSize);

				TO_AppData.Config->ClassQueue[i].PktPtr = 0;
				budget -= msgSize;
			}
			else
			{
				clsQueue->DeferredCount++;
				clsQueue->Deficit = budget - Surplus;
				Surplus = budget;
				break;
			}
		}
   	}
}



int32 TO_TableInit (void)
{
	int32 iStatus = CFE_SUCCESS;

    /* Register The TO config table */
    iStatus = CFE_TBL_Register (&TO_AppData.ConfigHandle,
    		TO_CONFIG_TABLENAME, (sizeof (TO_Table_t)),
    		CFE_TBL_OPT_DEFAULT,
    		(CFE_TBL_CallbackFuncPtr_t)TO_ValidateTable);

    if (iStatus != CFE_SUCCESS)
    {
    	CFE_EVS_SendEvent(TO_CONFIG_REG_ERR_EID, CFE_EVS_ERROR,
    			"CFE_TBL_Register() returned error %i.  Aborting table init.",
    			iStatus);

        goto end_of_function;
    }

    /* Load the TO configuration table file */
    iStatus = CFE_TBL_Load (TO_AppData.ConfigHandle, CFE_TBL_SRC_FILE,
    		TO_CONFIG_FILENAME);

    if (iStatus != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(TO_CONFIG_LD_ERR_EID, CFE_EVS_ERROR,
                  "CFE_TBL_Load() returned error %i.  Aborting table init.",
                  iStatus);

        goto end_of_function;
    }

    /* Manage the TO config table */
    iStatus = CFE_TBL_Manage(TO_AppData.ConfigHandle);
    if (iStatus != CFE_SUCCESS)
    {
    	CFE_EVS_SendEvent(TO_CONFIG_MNG_ERR_EID, CFE_EVS_ERROR,
           "CFE_TBL_Manage() returned error %i.  Aborting table init.",
           iStatus);

        goto end_of_function;
    }

    /* Make sure the CI iLoad Table is accessible */
    iStatus = CFE_TBL_GetAddress ((void **) (& TO_AppData.Config),
    		                                TO_AppData.ConfigHandle);
    /* Status should be CFE_TBL_INFO_UPDATED because we loaded it above */
    if (iStatus != CFE_TBL_INFO_UPDATED)
    {
    	CFE_EVS_SendEvent(TO_CONFIG_GADR_ERR_EID, CFE_EVS_ERROR,
    			"CFE_TBL_GetAddress() returned error %i.  Aborting table init.",
    			iStatus);

    	goto end_of_function;
    }

    iStatus = CFE_SUCCESS;

end_of_function:
    return iStatus;

}



int32 TO_ValidateTable (TO_Table_t* table)
{
	return 0;
}



void TO_SendToChannel(uint32 ChannelID, CFE_SB_MsgPtr_t Msg, uint32 Size)
{
	static struct sockaddr_in s_addr;
    int						  status = 0;

    bzero((char *) &s_addr, sizeof(s_addr));
    s_addr.sin_family      = AF_INET;

    if(ChannelID < TO_MAX_TLM_CHANNELS)
    {
    	char *Buffer = (char*)Msg;
    	TO_TlmChannels_t *channel = &TO_AppData.Config->Channel[ChannelID];

    	if(channel->Mode == TO_CHANNEL_ENABLED)
    	{
			CFE_ES_PerfLogEntry(TO_SOCKET_SEND_PERF_ID);
			/* Send message via UDP socket */
			s_addr.sin_addr.s_addr = inet_addr(channel->IP);
			s_addr.sin_port        = htons(channel->DstPort);
			status = sendto(channel->Socket, (char *)Buffer, Size, 0,
									(struct sockaddr *) &s_addr,
									 sizeof(s_addr));
			if (status < 0)
			{
				if(errno == 90)
				{
					CFE_EVS_SendEvent(TO_TLMOUTSTOP_ERR_EID,CFE_EVS_ERROR,
								   "L%d TO sendto errno %d.  Message too long.  Size=%u\n", __LINE__, errno, Size);
				}
				else
				{
					CFE_EVS_SendEvent(TO_TLMOUTSTOP_ERR_EID,CFE_EVS_ERROR,
							   "L%d TO sendto errno %d.\n", __LINE__, errno);
					channel->Mode = TO_CHANNEL_DISABLED;
				}
			}
			CFE_ES_PerfLogExit(TO_SOCKET_SEND_PERF_ID);
    	}
    }
}



#ifdef __cplusplus
}
#endif
