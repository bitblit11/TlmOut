
/*==============================================================================
** File Name: to__frame.c
**
** Title: TO Transfer Frame Functions
**
** $Author: $
** $Revision: $
** $Date:  $
**
** Purpose:  To build the TO transfer frame and update housekeeping fields
**           related to_ the transfer frame.
**
** Functions Contained:
**    TO_InitializeRealTimeFrame - Initializes telem frame for use
**    TO_InitializeOverflowBuffer - Initializes the overflow buffer
**    TO_InitializeFrames
**    TO_InitializeFillPacket - Initializes fill packet when frame not full
**    TO_AddOverFlowToFrame - Adds overflow buffer to_ current frame
**    TO_BuildRealTimeFrame - Builds current transfer frame
**    TO_PacketsAdded - Adds incoming messages to_ the frame
**    TO_CompleteFrame - Completes frame including fill packet
**    TO_AddPacketToFrame -
**    TO_OverflowBytesLeft
**    TO_SaveToOverflow
**    TO_CalculateHeaderPointer
**    TO_OutputFrame
**    TO_DiscardTelemetry
**
**
** Limitations, Assumptions, External Events, and Notes:
**  1.  None
**
** Modification Histo_ry:
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
#include "cfe.h"
#include "frame.h"
#include "app.h"
#include "events.h"
#include "crc.h"

/*
** Local Defines
*/

/* Telemetry pipe definitions */
#define TO_SYNC_TLM_PIPE_DEPTH       CFE_SB_MAX_PIPE_DEPTH
#define TO_SYNC_TLM_PIPE_NAME        "TO_SYNC_PIPE"
#define TO_ASYNC_TLM_PIPE_DEPTH      100
#define TO_ASYNC_TLM_PIPE_NAME       "TO_ASYNC_PIPE"
#define TO_PLAYBACK_TLM_PIPE_DEPTH   10
#define TO_PLAYBACK_TLM_PIPE_NAME    "TO_PLAYBACK_PIPE"

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
** Local Function Proto_types
*/

/*==============================================================================
** Name: TO_InitializeRealTimeFrame
**
** Purpose: To Initialize a frame buffer for use
**
** Arguments:
**     TO_FrameBuffer_t  *FrameBuffer - Pointer to_ frame buffer
**     uint16   usLength - Length of buffer
**     boolean   bInitHeader - Flag to_ initialize header or not
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**     TO_AddOverflowToFrame
**
** Called By:
**     TO_InitializeFrames
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
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
void TO_InitializeRealTimeFrame( TO_FrameBuffer_t  *FrameBuffer,
                                            uint16   usLength,
                                            boolean  bInitHeader )
{
    TO_TransferFrameMsg_t    *FrameMsgPtr;
      
    FrameMsgPtr = (TO_TransferFrameMsg_t *) FrameBuffer->MsgData;

    FrameBuffer->PktByteOffset  = 0;
    FrameBuffer->Length         = usLength;
    FrameBuffer->BytesFree      = usLength;
    FrameBuffer->HasData        = FALSE;

    /* Initialize the header */
    if ( bInitHeader == TRUE )
    {  

        FrameMsgPtr->SyncMarker              = TO_SYNC_MARK_VALUE;
        FrameMsgPtr->Header.VirtualChannelID = TO_REAL_TIME_VIRTUAL_CHAN_ID;
        FrameMsgPtr->Header.SpacecraftID     = CFE_SPACECRAFT_ID;
        FrameMsgPtr->Header.VersionNumber    = TO_CCSDS_TLM_VERSION_NUM;
        FrameMsgPtr->Header.FrameCounterMSW  = 0;
        FrameMsgPtr->Header.FrameCounterLSB  = 0;
        FrameMsgPtr->Header.ReplayFlagRT     = 0;
        FrameMsgPtr->Header.Reserved         = 0;
        FrameMsgPtr->PDUHeader.Reserved      = 0;

    }

    /* Set first header pointer in the frame header */
    if ( TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.ReadIndex].HasData == TRUE )
    {
        FrameMsgPtr->PDUHeader.FirstHeaderPointer = 
           TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.ReadIndex].HeaderPointer;

        /* Copy in overflow packets left over  */
        TO_AddOverflowToFrame(FrameBuffer);
    }
    else
    {
        FrameMsgPtr->PDUHeader.FirstHeaderPointer = 0;
    }
    
    FrameMsgPtr->Trailer.CRCValue  = 0;

}

/*==============================================================================
** Name: TO_InitializeOverflowBuffer
**
** Purpose: Initializes the overflow frame buffer for the selected
**          buffer index and initializes the header pointer.
**
** Arguments:
**     uint16 Index - Buffer Index
**     uint16 HeaderPointer - Pointer of data header
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
**     TO_InitializeFrames
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     None
**
** Global Outputs/Writes:
**     TO_AppData.Frame.OverflowFrame[].HasData
**     TO_AppData.Frame.OverflowFrame[].HeaderPointer
**     TO_AppData.Frame.OverflowFrame[].BytesSaved
**     TO_AppData.Frame.OverflowFrame[].BytesFree
**     TO_AppData.Frame.OverflowFrame[].PktByteOffset
**
** Programmer(s): Brian Butcher
**
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
void TO_InitializeOverflowBuffer ( uint16 usIndex , uint16 usHeaderPointer )
{
     TO_AppData.Frame.OverflowFrame[usIndex].HasData         = FALSE;
     TO_AppData.Frame.OverflowFrame[usIndex].HeaderPointer   = usHeaderPointer;
     TO_AppData.Frame.OverflowFrame[usIndex].BytesSaved      = 0;
     TO_AppData.Frame.OverflowFrame[usIndex].BytesFree       = TO_FRAME_DATA_SIZE;
     TO_AppData.Frame.OverflowFrame[usIndex].PktByteOffset   = 0;
}

/*==============================================================================
** Name: TO_InitializeFrames
**
** Purpose: To call the appropriate init routines.
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
**    TO_InitializeRealTimeFrame
**    TO_InitializeOverflowBuffer
**    TO_InitializeFillPkt
**
** Called By:
**    TO_AppInit
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     TO_AppData.Frame.FrameBuffer
**
** Global Outputs/Writes:
**    TO_AppData.Frame.WriteIndex - Current Frame Write Index
**    TO_AppData.Frame.ReadIndex - Current Frame Read Index
**    TO_AppData.Frame.RealTimePktBitsTotal
**    TO_AppData.Frame.PlaybackPktBitsTotal
**    TO_AppData.Frame.FrameBuffer[]
**
** Programmer(s): Brian Butcher
**
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
void TO_InitializeFrames ( void )
{
    uint16  usIndex;

    /****
    **    Init the overflow frames 
    ****/
    TO_AppData.Frame.WriteIndex = 0;
    TO_AppData.Frame.ReadIndex  = 0;
    TO_AppData.Frame.RealTimePktBitsTotal  = 0;
    TO_AppData.Frame.PlaybackPktBitsTotal  = 0;
    
    for (usIndex = 0; usIndex < TO_NUMBER_OF_OVERFLOW_FRAMES; usIndex++)
    {
       TO_InitializeOverflowBuffer (usIndex, 0);
    }
    /*   Init the Real-Time Frame */
    TO_InitializeRealTimeFrame (&TO_AppData.Frame.FrameBuffer[0],
                                 TO_FRAME_DATA_SIZE, /* length of buffer */
                                 TRUE );             /* header_init */
    
    /* Init the general purpose Fill Packet for a frame */
    TO_InitializeFillPkt();

}

/*==============================================================================
** Name:  TO_InitializeFillPkt
**
** Purpose: Performs initialization for Fill Packet global.
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
**     TO_InitializeFrames
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     None
**
** Global Outputs/Writes:
**     TO_AppData.FillPktSeqCount
**     TO_AppData.FillPacket.PriHdr.StreamId
**     TO_AppData.FillPacket.PriHdr.Sequence
**     TO_AppData.FillPacket.PriHdr.Length
**     TO_AppData.FillPacket.Data
**
** Programmer(s): Brian Butcher
**
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
void    TO_InitializeFillPkt ( void )
{
#ifdef TO_DEBUG
printf("TO: Inside TO_InitializeFillPkt\n");
#endif

    uint16  i;
    uint8   ucFillByte;
    uint16  streamId = TO_FILL_PACKET_APPID;
    uint8   *streamIdBytes = (uint8*)&streamId;
    uint16  sequence = TO_AppData.FillPktSeqCount | TO_FILL_PACKET_SEGMENTATION;
    uint8   *sequenceBytes = (uint8*)&sequence;
    uint16  length = TO_IN_FILL_PKT_SIZE - 1;
    uint8   *lengthBytes = (uint8*)&length;

    /* Init Fill Packet */
    TO_AppData.FillPktSeqCount = 0;
    /* TODO:  Genericize this to support big endian as well. */
    TO_AppData.FillPacket.PriHdr.StreamId[0] = streamIdBytes[1];
    TO_AppData.FillPacket.PriHdr.StreamId[1] = streamIdBytes[0];
    TO_AppData.FillPacket.PriHdr.Sequence[0] = sequenceBytes[1];
    TO_AppData.FillPacket.PriHdr.Sequence[1] = sequenceBytes[0];
    TO_AppData.FillPacket.PriHdr.Length[0] = lengthBytes[1];
    TO_AppData.FillPacket.PriHdr.Length[1] = lengthBytes[0];

    /* Starting byte in fill packet */
    ucFillByte = TO_FILL_PACKET_DATA_BYTE;

    for (i=0; i < (TO_FRAME_DATA_SIZE - TO_PKT_HEADER_SIZE); i++)
    {
        TO_AppData.FillPacket.Data[i] = ucFillByte++;
    }

    return;

}

/*==============================================================================
** Name: TO_AddOverflowToFrame
**
** Purpose: To add an overflow frame to_ the current buffer
**
** Arguments:
**     TO_FrameBuffer_t   *FrameBufferPtr - Pointer to_ Frame Buffer
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   We are currently not using overflow buffers and it has not been tested.
**
** Routines Called:
**     TO_InitializeOverflowBuffer
**
** Called By:
**     None
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     TO_AppData.Frame.OverflowFrame[].BytesSaved
**
** Global Outputs/Writes:
**     None
**
** Programmer(s): Brian Butcher
**
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**ToDo: Check into_ the overflow frame buffers
**==============================================================================
*/
void    TO_AddOverflowToFrame ( TO_FrameBuffer_t   *FrameBufferPtr  )
{
    TO_TransferFrameMsg_t *FrameMsgPtr;

    uint16 BytesSaved =
       TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.ReadIndex].BytesSaved;

    FrameMsgPtr = (TO_TransferFrameMsg_t *) FrameBufferPtr->MsgData;

    /* Update overflow buffer and frame buffer */
    /* Currently not using overflow buffers, this need to_ be tested
    memcpy((char *) &(FrameMsgPtr->PacketData [FrameBufferPtr->PktByteOffset] ),
               (char *) &(TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.ReadIndex].Data ),
               BytesSaved );
    */

    FrameBufferPtr->BytesFree     -= BytesSaved;
    FrameBufferPtr->PktByteOffset += BytesSaved;

    TO_InitializeOverflowBuffer(TO_AppData.Frame.ReadIndex, 0);

    if (TO_AppData.Frame.ReadIndex != TO_AppData.Frame.WriteIndex)
    {
        TO_AppData.Frame.ReadIndex =
        	(TO_AppData.Frame.ReadIndex + 1 >= TO_NUMBER_OF_OVERFLOW_FRAMES )?
                                           0 : TO_AppData.Frame.ReadIndex + 1;
    }

    FrameBufferPtr->HasData = TRUE;

}


/*==============================================================================
** Name: TO_BuildRealTimeFrame
**
** Purpose: Fills the frame buffer with telemetry packet data.
**
** Arguments:
**    TO_FrameBuffer_t   *FrameBufferPtr - Frame Buffer Pointer
**    boolean             CompleteFrame  - Flag to_ indicate complete pkt
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**     TO_PacketsAdded
**     TO_CompleteFrame
**
** Called By:
**     TO_ForwardTelemetry
**
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     TO_AppData.TlmPipe
**
** Global Outputs/Writes:
**     None
**
** Programmer(s): Brian Butcher
**
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
void TO_BuildRealTimeFrame ( TO_FrameBuffer_t   *FrameBufferPtr,
                             boolean             CompleteFrame )
{
#ifdef TO_DEBUG
printf("TO: Inside TO_BuildRealTimeFrame\n");
#endif

	boolean PacketsAdded = FALSE;

	/* Read TlmPipe and place messages into frames */
    PacketsAdded =
         TO_PacketsAdded(TO_AppData.TlmPipe, FrameBufferPtr, TRUE);

    if((FrameBufferPtr->BytesFree > 0) && (CompleteFrame))
	{
		/* Complete Frame for Output */
		TO_CompleteFrame (FrameBufferPtr);
	}
}

/*==============================================================================
** Name:  TO_PacketsAdded
**
** Purpose: Adds packets from a specified pipeline to_ the frame buffer.
**
** Arguments:
**      CFE_SB_PipeId_t   SBPipe - SB Pipe to_ read
**      TO_FrameBuffer_t  *FrameBufferPtr - Buffer to_ sto_re frames
**      boolean           ReadTillEmpty - Flag to_ read until pipe empty
**
** Returns:
**     boolean HasPacketBeenAdded - Indicato_r to_ tell if packets have been added
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**     CFE_SB_RcvMsg
**     TO_AddPacketToFrame
**
**
** Called By:
**     TO_BuildRealTimeFrame
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
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
boolean TO_PacketsAdded ( CFE_SB_PipeId_t   SBPipe,
                          TO_FrameBuffer_t  *FrameBufferPtr,
                          boolean           ReadTillEmpty )
{
    uint16             AppID;
    uint16             MsgID;
    CFE_SB_MsgPtr_t    MessagePtr;
    boolean            DoneAdding         = FALSE;
    boolean            HasPacketBeenAdded = FALSE;
    boolean            AddPacket          = TRUE;
    int32              ReceiveStatus      = CFE_SUCCESS;

#ifdef TO_DEBUG
printf("TO: Inside TO_PacketsAdded\n");
#endif

    while  (! DoneAdding )
    {
    	/* Check for message on supplied pipe */
        ReceiveStatus = CFE_SB_RcvMsg (&MessagePtr, SBPipe, CFE_SB_POLL);

        if (ReceiveStatus == CFE_SUCCESS)
        {
            if ( AddPacket == TRUE )
            {
            	/* Add packet to_ frame */
               TO_AddPacketToFrame ( ( TO_PacketArray_t * ) MessagePtr, FrameBufferPtr );

               HasPacketBeenAdded  = TRUE;
            }
        }
        else
        {
            DoneAdding = TRUE;
        }

        if ((ReadTillEmpty == FALSE))
        {
            DoneAdding = TRUE;
        }
    }

    return ( HasPacketBeenAdded );

}


/*==============================================================================
** Name: TO_CompleteFrame
**
** Purpose: Completes a frame buffer with a fill packet if needed.
**
** Arguments:
**     TO_FrameBuffer_t   *FrameBufferPtr  - Pointer to_ the frame buffer
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**     TO_AddPacketToFrame
**
** Called By:
**     TO_ForwardTelemetry
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     TO_AppData.FillPktSeqCount - Fill packet sequence counter
**     TO_AppData.FillPacket - Fill packet buffer
**
** Global Outputs/Writes:
**     TO_AppData.FillPktSeqCount - Fill packet sequence count
**     TO_AppData.FillPacket.PriHdr.Sequence - Current fill packet sequence
**     TO_AppData.FillPacket.PriHdr.Length - Fill Packet Length
**
** Programmer(s): Brian Butcher
**
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
void TO_CompleteFrame ( TO_FrameBuffer_t   *FrameBufferPtr )
{
#ifdef TO_DEBUG
printf("TO: Inside Complete Frame\n");
#endif

    TO_TransferFrameMsg_t      *FrameMsgPtr;

    /* Set Sequnce Value */
    TO_AppData.FillPacket.PriHdr.Sequence =
    		         TO_AppData.FillPktSeqCount | TO_FILL_PACKET_SEGMENTATION;

    /* Swap bytes on little endian machine */
   TO_AppData.FillPacket.PriHdr.Sequence =
    		                     hto_ns(TO_AppData.FillPacket.PriHdr.Sequence);

    TO_AppData.FillPktSeqCount++;

    if(FrameBufferPtr->BytesFree > (TOIN_FILL_PKT_SIZE + TO_PKT_HEADER_SIZE))
    {
        TO_AppData.FillPacket.PriHdr.Length =
        		            FrameBufferPtr->BytesFree - TO_PKT_HEADER_SIZE - 1;

        FrameMsgPtr = (TO_TransferFrameMsg_t *) FrameBufferPtr->MsgData;

        /* Copy fill packet header to_ frame buffer */
        memcpy((char *) &(FrameMsgPtr->PacketData[FrameBufferPtr->PktByteOffset]),
               (char *) &TO_AppData.FillPacket,
               FrameBufferPtr->BytesFree);

        FrameBufferPtr->PktByteOffset += FrameBufferPtr->BytesFree;

        FrameBufferPtr->BytesFree = 0;

    }
    else
    {
        TO_AppData.FillPacket.PriHdr.Length = TOIN_FILL_PKT_SIZE - 1;

        TO_AddPacketToFrame ((TO_PacketArray_t *) &TO_AppData.FillPacket,
        		                                   FrameBufferPtr);
    }
}

/*==============================================================================
** Name: TO_AddPacketToFrame
**
** Purpose: Adds a packet to_ the frame buffer
**
** Arguments:
**     TO_PacketArray_t * TlmPacketPtr - Pointer to_ telem packet
**     TO_FrameBuffer_t * FrameBufferPtr - Pointer to_ frame buffer
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**     TO_SaveToOverflow
**     CFE_EVS_SendEvent
**
** Called By:
**     TO_CompleteFrame
**     TO_PacketsAdded
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     TO_AppData.Frame.PlaybackFrameBuffer
**     TO_AppData.Frame.PlaybackPktBitsTotal
**     TO_AppData.Frame.RealTimePktBitsTotal
**
**
** Global Outputs/Writes:
**     TO_AppData.HkPacket.usPacketLossCount
**
** Programmer(s): Brian Butcher
**
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
void TO_AddPacketToFrame  ( TO_PacketArray_t * TlmPacketPtr,
                            TO_FrameBuffer_t * FrameBufferPtr )
{
    uint16                     NumBytesToCopy;
    uint16                     PacketBytesLeft;
    TO_PacketArray_t          *PacketArrayPtr;
    TO_TransferFrameMsg_t     *FrameMsgPtr;
    CCSDS_TlmPkt_t            *PacketMsgPtr;
    uint16                     usTempShort;
    int i;

#ifdef TO_DEBUG
printf("TO: Inside TO_AddPacketToFrame\n");
#endif

    PacketArrayPtr = (TO_PacketArray_t *) TlmPacketPtr;
    FrameMsgPtr    = (TO_TransferFrameMsg_t *) FrameBufferPtr->MsgData;

    NumBytesToCopy = ((CCSDS_TlmPkt_t *)&TlmPacketPtr->Data[0])->PriHdr.Length
    		                                           + TO_PKT_HEADER_SIZE + 1;

    if ( FrameBufferPtr == &TO_AppData.Frame.PlaybackFrameBuffer )
    {
        TO_AppData.Frame.PlaybackPktBitsTotal += ( NumBytesToCopy * 8 );
    }
    else
    {
        TO_AppData.Frame.RealTimePktBitsTotal += ( NumBytesToCopy * 8 );
    }

    /* if packet fits in buffer */
    if (NumBytesToCopy <= FrameBufferPtr->BytesFree) 
    {
        memcpy((char *)&(FrameMsgPtr->PacketData[FrameBufferPtr->PktByteOffset]),
               (char *)  PacketArrayPtr,
               NumBytesToCopy);

        PacketMsgPtr = (CCSDS_TlmPkt_t*)&(FrameMsgPtr->PacketData[FrameBufferPtr->PktByteOffset]);

        /* Swap bytes if little endian machine after copy has occurred */
        PacketMsgPtr->PriHdr.StreamId = hto_ns(PacketMsgPtr->PriHdr.StreamId);
        PacketMsgPtr->PriHdr.Sequence = hto_ns(PacketMsgPtr->PriHdr.Sequence);
        PacketMsgPtr->PriHdr.Length = hto_ns(PacketMsgPtr->PriHdr.Length);

        PacketBytesLeft                = 0;
        FrameBufferPtr->BytesFree     -= NumBytesToCopy;
        FrameBufferPtr->PktByteOffset += NumBytesToCopy;
        FrameBufferPtr->HasData        = TRUE;

    }
    else /* Packet doesn't fit, copy portion that does and copy rest to_ overflow */
    {
        if (TO_OverflowBytesLeft () >= NumBytesToCopy - FrameBufferPtr->BytesFree)
        {
            /* copy data that fits to_ the frame buffer */
        	/* Not currently using overflow buffer, need to_ test before uncommenting
            memcpy((char *) &(FrameMsgPtr->PacketData [FrameBufferPtr->PktByteOffset] ),
                   (char *) PacketArrayPtr,
                   FrameBufferPtr->BytesFree );
            */

            PacketBytesLeft  = NumBytesToCopy - FrameBufferPtr->BytesFree;

            TO_SaveToOverflow (PacketArrayPtr, FrameBufferPtr->BytesFree,
                               PacketBytesLeft  );

            FrameBufferPtr->PktByteOffset += FrameBufferPtr->BytesFree;
            FrameBufferPtr->BytesFree      = 0;
            FrameBufferPtr->HasData        = TRUE;

        }
        else   /* packet is lost */
        {
            TO_AppData.HkPacket.usPacketLossCount++;

            if (TO_AppData.HkPacket.usPacketLossCount <= TO_PACKET_LOSS_REPORT_LIMIT)
            {
                CFE_EVS_SendEvent( TO_PACKET_LOST_ERR_EID,CFE_EVS_ERROR
                                 , "Telemetry Packet Loss MID = %04X, length = %d bytes"
                                 , ((CCSDS_TlmPkt_t *)&TlmPacketPtr->Data[0])->PriHdr.StreamId
                                 , ((CCSDS_TlmPkt_t *)&TlmPacketPtr->Data[0])->PriHdr.Length );
            }
        }
    }
}

/*==============================================================================
** Name: TO_OverflowBytesLeft
**
** Purpose: Calculates the number of bytes remaining in the OverflowFrame
**          array of buffers.
**
** Arguments:
**     None
**
** Returns:
**     uint16 ByteCount - Number of bytes remaining
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
** Algorithm: psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     TO_AppData.Frame.WriteIndex
**     TO_AppData.Frame.OverflowFrame[].BytesFree
**     TO_AppData.Frame.ReadIndex
**
** Global Outputs/Writes:
**     None
**
** Programmer(s): Brian Butcher
**
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
uint16  TO_OverflowBytesLeft(void)
{
    uint16   ByteCount = 0;
    uint16   WrIndex   = TO_AppData.Frame.WriteIndex;
    
    ByteCount = TO_AppData.Frame.OverflowFrame[WrIndex].BytesFree;

    WrIndex = ( WrIndex + 1 >= TO_NUMBER_OF_OVERFLOW_FRAMES )?  0 : WrIndex + 1;
   
    while (WrIndex != TO_AppData.Frame.ReadIndex)
    {
        ByteCount += TO_AppData.Frame.OverflowFrame[WrIndex].BytesFree;
        
        WrIndex =
              ( WrIndex + 1 >= TO_NUMBER_OF_OVERFLOW_FRAMES )?  0 : WrIndex + 1;
    }

    return ( ByteCount );

}

/*==============================================================================
** Name:   TO_SaveToOverflow
**
** Purpose:  Writes the rest of the packet that failed to_ fit into_ the current
**           frame to_ the overflow buffers.
**
** Arguments:
**     TO_PacketArray_t *PacketArrayPtr - Pointer to_ packet
**     uint16             PktIndex - Index into_ packet pointer
**     uint16             PktBytesLeft - Bytes left in packet to_ save
**
** Returns:
**     None
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   Not currently using overflow becasue our frames are fixed and
**       will not overflow. Keeping legacy LRO code for future use.
**
** Routines Called:
**     TO_CalculateHeaderPointer
**     TO_InitializeOverflowBuffer
**
** Called By:
**     TO_AddPacketToFrame
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
** TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].BytesFree
** TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].HasData
** TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].PktByteOffset
**
** Global Outputs/Writes:
** TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].HeaderPointer
** TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].BytesSaved
** TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].BytesFree
** TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].HasData
** TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].PktByteOffset
** TO_AppData.Frame.WriteIndex
**
** Programmer(s): Brian Butcher
**
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
void    TO_SaveToOverflow    ( TO_PacketArray_t *PacketArrayPtr,
                              uint16             PktIndex,
                              uint16             PktBytesLeft )
{
    uint16      BytesToCopy;
    uint16      HeaderPointer;
    uint16      OverflowBytesFree;
    uint16      PacketIndex     = PktIndex;
    uint16      BytesLeft       = PktBytesLeft;
    uint16      StartByteOffset = 0;

    OverflowBytesFree =
      TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].BytesFree;
    
    BytesToCopy =
    		    (BytesLeft >= OverflowBytesFree)? OverflowBytesFree : BytesLeft;

    if ( TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].HasData == FALSE )
    {
        TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].HeaderPointer =
                               TO_CalculateHeaderPointer (PacketIndex, BytesToCopy);
    }
   
    while ( BytesLeft > 0 )
    {
        StartByteOffset =
           TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].PktByteOffset;
/*
        OS_MemCpy ((char *) &(TO_AppData.Frame.OverflowFrame [TO_AppData.Frame.WriteIndex].Data [StartByteOffset]),
                   (char *) &(PacketArrayPtr->Data [PacketIndex] ),
                   BytesToCopy  );
*/
        BytesLeft    -= BytesToCopy;
        PacketIndex  += BytesToCopy;

        TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].HasData        = TRUE;
        TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].BytesFree     -= BytesToCopy;
        TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].BytesSaved    += BytesToCopy;
        TO_AppData.Frame.OverflowFrame[TO_AppData.Frame.WriteIndex].PktByteOffset += BytesToCopy;
 
        if (BytesLeft > 0)
        {
            BytesToCopy = ( BytesLeft >= TO_FRAME_DATA_SIZE )?  TO_FRAME_DATA_SIZE : BytesLeft;

            TO_AppData.Frame.WriteIndex = ( TO_AppData.Frame.WriteIndex + 1 >= TO_NUMBER_OF_OVERFLOW_FRAMES )?
                                          0 : TO_AppData.Frame.WriteIndex + 1;

            HeaderPointer = TO_CalculateHeaderPointer (PacketIndex, BytesToCopy);

            TO_InitializeOverflowBuffer ( TO_AppData.Frame.WriteIndex , HeaderPointer);

        }
    }
}

/*==============================================================================
** Name: TO_CalculateHeaderPointer
**
** Purpose:  Calculates the header pointer for overflow frames.
**
** Arguments:
**     uint16 Index - Current packet index
**     uint16 BytesToCopy - number of bytes to_ copy
**
** Returns:
**     uint16 HeaderPointer - Pointer to_ packet header
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**     None
**
** Called By:
**     TO_SaveToOverflow
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
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
uint16  TO_CalculateHeaderPointer   ( uint16 Index, uint16 BytesToCopy )
{
    uint16 HeaderPointer;

    if ( Index == 0 ) /* Packet not written yet */
    {
        HeaderPointer = 0;
    }
    else if ( BytesToCopy >=  TO_FRAME_DATA_SIZE)
    {
        HeaderPointer = TO_NO_PACKET_HEADER;
    }
    else
    {
        HeaderPointer = BytesToCopy;
    }

    return (HeaderPointer);
}



/*==============================================================================
** Name:  TO_OutputFrame
**
** Purpose: Outputs a VCDU frame buffer using the send serial or send data
**          functions.
**
** Arguments:
**     TO_FrameBuffer_t   *CurrentBuffer - Pointer to_ current buffer
**
** Returns:
**     Boolean Status - Indicates success or failure
**
** Assumptions, Limitations, External Events, and Notes:
**  1.   None
**
** Routines Called:
**     fCalcCRC
**     TO_SendSocketData
**     TO_SendSerialData
**
** Called By:
**     None
**
** Algorithm: psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     TO_AppData.suppress_sendto_ - Flag to_ suppress socket telem
**     TO_AppData.suppress_serial_sendto_ - Flag to_ suppress serial telem
**
** Global Outputs/Writes:
**     None
**
** Programmer(s): Brian Butcher
**
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
boolean TO_OutputFrame              ( TO_FrameBuffer_t   *CurrentBuffer )
{    
    TO_TransferFrameMsg_t  *FrameMsgPtr;
    uint32                  FrameCounter;
    unsigned short usPrevHdrValues[3];
    int32                   status;
    unsigned short          usCRC;
    int i;

#ifdef TO_DEBUG
printf("TO: Inside TO_OutputFrame\n");
#endif

    FrameMsgPtr    = (TO_TransferFrameMsg_t *) CurrentBuffer->MsgData;
    
    FrameCounter   = ((( uint32 )FrameMsgPtr->Header.FrameCounterMSW) << 8 )
                     + ( uint32 )FrameMsgPtr->Header.FrameCounterLSB;
    FrameCounter++;

    FrameMsgPtr->Header.FrameCounterMSW = ( 0x00FFFF00 & FrameCounter ) >> 8;
    FrameMsgPtr->Header.FrameCounterLSB = ( 0x000000FF & FrameCounter );

    /* Assign the latest Command Link Control Word only for Real-Time frame */
    if(FrameMsgPtr->Header.VirtualChannelID == TO_REAL_TIME_VIRTUAL_CHAN_ID)
    {
        FrameMsgPtr->Trailer.CommandLinkControlWordMSW =
        		          ( 0xFFFF0000 & TO_AppData.Frame.LatestCLCW ) >> 16;
        FrameMsgPtr->Trailer.CommandLinkControlWordLSW =
        		          ( 0x0000FFFF & TO_AppData.Frame.LatestCLCW );
    }

    //Ensure Header Bytes Are Big Endian for transmit
    uint16* usHdrPtr = (uint16*) FrameMsgPtr;
    for(i=TO_FRAME_HDR_OFFSET;i<TO_FRAME_HDR_OFFSET+TO_FRAME_HDR_SIZE;i++)
    {
       usPrevHdrValues[i - TO_FRAME_HDR_OFFSET] = usHdrPtr[i];
	   usHdrPtr[i] = hto_ns(usHdrPtr[i]);
    }

    /* Calulate the CRC for the frame trailer */
    usCRC = fCalcCRC((unsigned char*)FrameMsgPtr,TO_FRAME_SYNC_SIZE,TO_CRC_FRAME_SIZE);

    FrameMsgPtr->Trailer.CRCValue = hto_ns(usCRC);

    if(TO_AppData.suppress_sendto_ == FALSE)
    {
    	status = TO_SendSocketData((unsigned char*) FrameMsgPtr,
    			                     TO_TRANSFER_FRAME_SIZE);
    }

    if(TO_AppData.suppress_serial_sendto_ == FALSE)
    {
    	status = TO_SendSerialData((unsigned char*) FrameMsgPtr);
    }

    if(status < 0)
    {
    	status = FALSE;
    }
    else
    {
    	status = TRUE;
    }

    /* Set Header Bytes Back to_ un-swapped values for local processing.  */
    /* The headers are intialized once so these values need to_ be reset  */
    for(i=TO_FRAME_HDR_OFFSET;i<TO_FRAME_HDR_OFFSET+TO_FRAME_HDR_SIZE;i++)
    {
	   (usHdrPtr[i]) = usPrevHdrValues[i - TO_FRAME_HDR_OFFSET];
    }

    return( status );

}



/*==============================================================================
** Name:  TO_DiscardTelemetry
**
** Purpose:  Read and Discard packets from the telemetry pipes
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
**    CFE_SB_RcvMsg
**
** Called By:
**     None
**
** Algorithm:   psuedo-code or english descrption of basic algorithm
**
** Global Inputs/Reads:
**     TO_AppData.TlmPipe
**
** Global Outputs/Writes:
**     None
**
** Programmer(s): Brian Butcher
**
** Histo_ry:     Date Written  mm-dd-yyyy
**              Unit Tested mm-dd-yyyy
**
**==============================================================================
*/
void    TO_DiscardTelemetry   ( void )
{    
    CFE_SB_MsgPtr_t  LocalMessagePtr;   /*** SB Message discard buffer ***/
    int32            SBReadResult  = CFE_SUCCESS;
    
    while  (SBReadResult == CFE_SUCCESS) 
    {
        SBReadResult = CFE_SB_RcvMsg ( &LocalMessagePtr,
        		                        TO_AppData.TlmPipe,
        		                        CFE_SB_POLL);
    }
    
}

#ifdef __cplusplus
}
#endif

/* ---------- end of file to__frame.c ----------*/

