/*==============================================================================
** File Name: to_frame.h
**
** Title:  TO Frame Header File
**
** $Author: $
** $Revision: $
** $Date:  $
**
** Purpose:   To frame #defines, structs, and prototypes.
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

#ifndef _TO_FRAME_H_
#define _TO_FRAME_H_

/*
** Pragmas
*/

/*
** Include Files
*/

#include "cfe.h"
#include "common_types.h"
#include "cfe_sb.h"
#include "cfe_evs.h"

/*
** Local Defines
*/

#define   TO_NUMBER_OF_BUILD_FRAMES        1  /* Frames to use for telemetry */
#define   TO_NUMBER_OF_OVERFLOW_FRAMES     8  /* Overflow_Frame size         */
#define   TO_PACKET_LOSS_REPORT_LIMIT      5  /* Number of Events before stop*/

/*  Default CLCW values */
#define   TO_VC0_CLCW_DEFAULT              0x01040000

#define   TO_FRAME_DATA_SIZE               462   /* bytes */
#define   TO_CCSDS_TLM_VERSION_NUM         1
#define   TO_REAL_TIME_VIRTUAL_CHAN_ID     0
#define   TO_NO_PACKET_HEADER              0x7FF

#define   TO_IN_FILL_PKT_SIZE             2
#define   TO_FILL_PACKET_APPID             2047
#define   TO_FILL_PACKET_DATA_BYTE         0
#define   TO_FILL_PACKET_SEGMENTATION      0xC000

#define   TO_PKT_HEADER_SIZE            sizeof(CCSDS_PriHdr_t)

/* bytes of packet data not including 12 bytes of */
/* primary and secondary header                   */
#define   TO_AX_TLM_PKT_SIZE  450

#define   TO_APPLICATION_ID_MASK    ( 0x07FF )

/* Offset in shorts into transfer frame where hdr starts */
#define   TO_FRAME_HDR_OFFSET 2

/* Number of shorts in the transfer frame header */
#define   TO_FRAME_HDR_SIZE   3

#define   TO_CLCW_BYTE_SIZE   4
#define   TO_FRAME_SYNC_SIZE  4


/******   CCSDS transfer frame and packet definitions ******/
#define   TO_SYNC_MARK_VALUE         0x1ACFFC1D
#define   M_MIN_TLM_MID              0x0800
#define   M_MAX_TLM_MID              0x08FF


/*
** Local Structure Declarations
*/

typedef  struct
{
     uint16  CommandLinkControlWordMSW;
     uint16  CommandLinkControlWordLSW;
     uint16  CRCValue;
     uint8   ReedSolomon[96];

}  TO_FrameTrailer_t;

// MPDU Header
typedef struct
{
     uint16      Reserved           :5;
     uint16      FirstHeaderPointer :11;

} TO_FrameDataFieldHeader_t;

// VCDU Header
typedef struct
{
     uint16      VersionNumber    :2;
     uint16      SpacecraftID     :8;
     uint16      VirtualChannelID :6;

     uint16      FrameCounterMSW;

     uint16      FrameCounterLSB  :8;
     uint16      ReplayFlagRT     :1;
     uint16      Reserved         :7;

} TO_FramePrimaryHeader_t ;

#define   TO_FRAME_DATA_FIELD_HEADER_SIZE   sizeof (TO_FrameDataFieldHeader_t)



typedef struct
{
    uint32                     SyncMarker;
    TO_FramePrimaryHeader_t    Header ;
    TO_FrameDataFieldHeader_t  PDUHeader ;
    uint8                      PacketData[TO_FRAME_DATA_SIZE];
    TO_FrameTrailer_t          Trailer ;
    
}  TO_TransferFrameMsg_t;

#define   TO_TRANSFER_FRAME_SIZE       sizeof (TO_TransferFrameMsg_t)

#define   TO_TRANSFER_FRAME_BASE_SIZE   (TO_FRAME_DATA_SIZE \
		                                + sizeof(TO_FramePrimaryHeader_t) \
                                        + sizeof (TO_FrameDataFieldHeader_t) \
                                        + sizeof (TO_FrameTrailer_t))

#define   TO_TRANSFER_FRAME_BIT_SIZE    ((sizeof (TO_TransferFrameMsg_t) \
		                                  - (CFE_SB_TLM_HDR_SIZE * 8) ) * 8 )

#define   TO_CRC_FRAME_SIZE (TO_FRAME_DATA_SIZE \
		                     + sizeof (TO_FramePrimaryHeader_t) \
                             + sizeof (TO_FrameDataFieldHeader_t) \
                             + TO_CLCW_BYTE_SIZE)

typedef struct
{ 
     uint8          MsgData[TO_TRANSFER_FRAME_SIZE];
     boolean        HasData;
     uint16         Length;
     uint16         BytesFree;
     uint16         PktByteOffset;
     
} TO_FrameBuffer_t;

typedef struct
{ 
     boolean        HasData;
     uint8          Spare;
     uint16         HeaderPointer;
     uint16         BytesFree;
     uint16         BytesSaved;
     uint16         PktByteOffset;
     uint8          Data [TO_FRAME_DATA_SIZE];
     
} TO_OverflowBuffer_t;


typedef struct
{
     CCSDS_PriHdr_t   PriHdr;
     uint8            Data[ TO_FRAME_DATA_SIZE - TO_PKT_HEADER_SIZE ];
     
}  TO_FillPacket_t;

typedef struct
{
     uint8 Data [TO_AX_TLM_PKT_SIZE + sizeof (CCSDS_TlmPkt_t)];
     
}  TO_PacketArray_t;


typedef struct
{
    uint8            CmdHeader[CFE_SB_CMD_HDR_SIZE];
     
}  TO_PDUAcknowledgeCmd_t;


typedef struct
{
    uint32                  LatestCLCW;
    boolean                 OutputRealTimeVCDU;
    uint16                  ReadIndex;            /* Reading from Overflow */
    uint16                  WriteIndex;           /* Writing to Overflow */ 
    uint32                  RealTimePktBitsTotal; /** Total of bits added in HK cycle **/
    uint32                  PlaybackPktBitsTotal; /** Total of bits added in HK cycle **/
    TO_FrameBuffer_t       FrameBuffer   [TO_NUMBER_OF_BUILD_FRAMES];
    TO_FrameBuffer_t       PlaybackFrameBuffer;
    TO_OverflowBuffer_t    OverflowFrame [TO_NUMBER_OF_OVERFLOW_FRAMES];

} TO_FrameManagement_t;

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

void    TO_InitializeRealTimeFrame  ( TO_FrameBuffer_t  *FrameBuffer,
                                               uint16   Length,
                                              boolean   InitHeader );
void    TO_InitializePlaybackFrame  ( TO_FrameBuffer_t  *FrameBuffer,
                                               uint16   Length,
                                             boolean   InitHeader );
void    TO_InitializeOverflowBuffer ( uint16 Index , uint16 HeaderPointer );
void    TO_InitializeFillPkt        ( void );
void    TO_InitializeFrames         ( void );
void    TO_AddOverflowToFrame       ( TO_FrameBuffer_t   *FrameBufferPtr  );
uint16  TO_OverflowBytesLeft        ( void );
void    TO_SaveToOverflow           ( TO_PacketArray_t *PacketArrayPtr,
                                      uint16             PktIndex,
                                      uint16             PktBytesLeft );
void    TO_BuildRealTimeFrame       ( TO_FrameBuffer_t   *FrameBufferPtr,
                                      boolean             CompleteFrame );
boolean TO_PacketsAdded             ( CFE_SB_PipeId_t   SBPipe,
									   TO_FrameBuffer_t  *FrameBufferPtr,
                                      boolean            ReadTillEmpty );
void    TO_AddPacketToFrame         ( TO_PacketArray_t * TlmPacketPtr,
									   TO_FrameBuffer_t * FrameBufferPtr );
void    TO_CompleteFrame           ( TO_FrameBuffer_t   *FrameBufferPtr );
uint16  TO_CalculateHeaderPointer  ( uint16 Index, uint16 BytesToCopy );
boolean TO_OutputFrame             ( TO_FrameBuffer_t   *CurrentBuffer );
void    TO_DiscardTelemetry        ( void );

#endif /* _TO_FRAME_ */

/* ---------- end of file to_frame.h ----------*/
