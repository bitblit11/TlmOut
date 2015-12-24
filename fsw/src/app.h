/*==============================================================================
** File Name: (Name of the C source file)
**
** Title:      TO application header file
**
** $Author: $
** $Revision: $
** $Date:  $
**
** Purpose:   To provide #defines, structs, and prototypes
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

#ifndef _TO_APP_H_
#define _TO_APP_H_

/*
** Pragmas
*/

/*
** Include Files
*/

#include "common_types.h"
#include "cfe_time.h"
#include "cfe_evs.h"
#include "cfe_es.h"
#include "cfe_sb.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "osapi.h"
#include "cmds.h"
#include "frame.h"
#include "msg.h"
#include "tbl.h"

/*
** Local Defines
*/

//#define TO_DEBUG
#define TO_UNUSED              0
#define TO_EVT_COUNT           8
#define TO_MAX_SER_STRING_SIZE 11
#define TO_CMD_PIPE_DEPTH      20
#define TO_SCH_PIPE_DEPTH      10

#define TO_MAX_CMD_INGEST      256
#define TO_CMD_PORT            5010


/*
** Local Structure Declarations
*/
typedef struct
{
    uint8   Octet1;
    uint16  PDataLen;
    uint8   Octet4;
    uint16  SrcEntityId;
    uint32  TransSeqNum;
    uint16  DstEntityId;

}OS_PACK CF_PDU_Hdr_t;

typedef struct
{
   uint8       TlmHeader[CFE_SB_TLM_HDR_SIZE];
   uint16      CmdCount;
   uint16      CmdErrCount;
   uint32      IngestMsgCount;
   uint32      IngestErrorCount;
   uint8       FDPdusDropped;
   uint8       EOFPdusDropped;
   uint8       FINPdusDropped;
   uint8       ACKPdusDropped;
   uint8       MDPdusDropped;
   uint8       NAKPdusDropped;
   uint8       Spare[2];
   uint32      PDUsCaptured;
   uint32      PktSubCount;
} TO_HkPacket_T;

#define TO_HK_TLM_LNGTH      sizeof(TO_HkPacket_T)

typedef struct
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint16             synch;

    uint16             bit1:1;
    uint16             bit2:1;
    uint16             bit34:2;
    uint16             bit56:2;
    uint16             bit78:2;
    uint16             nibble1:4;
    uint16             nibble2:4;

    uint8              bl1, bl2;       /* boolean */
    int8               b1, b2, b3, b4;
    int16              w1,w2;
    
    int32              dw1, dw2;
    float              f1, f2;
    double             df1, df2;
    char               str[10];
}TO_Data_Types_Fmt_T;

#define TO_DATA_TYPES_LNGTH   sizeof (TO_Data_Types_Fmt_T)

/* Type definition (TO app global data)...  */
typedef struct
{
   TO_HkPacket_T           HkPacket;
   TO_Data_Types_Fmt_T     DataTypesPacket;

   CFE_SB_MsgPtr_t          MsgPtr;
   CFE_SB_PipeId_t          CmdPipe;
   CFE_SB_PipeId_t          SchPipe;

   uint32                   RunStatus;

   char                     CmdPipeName[16];
   uint16                   CmdPipeDepth;
   char                     SchPipeName[16];
   uint16                   SchPipeDepth;

   CFE_EVS_BinFilter_t      EventFilters[TO_EVT_COUNT];

   CFE_TBL_Handle_t         ConfigHandle;
   TO_Table_t*             Config;

   boolean                  DropFileData;
   int32                    DropFileDataCnt;
   boolean                  DropEOF;
   int32                    DropEOFCnt;
   boolean                  DropFIN;
   int32                    DropFINCnt;
   boolean                  DropACK;
   int32                    DropACKCnt;
   boolean                  DropMetaData;
   int32                    DropMetaDataCnt;
   boolean                  DropNAK;
   int32                    DropNAKCnt;
   boolean                  CorruptChecksum;
   boolean                  AdjustFileSize;
   int32                    PDUFileSizeAdjustment;

   /* Telemetry Framing Variables */
   uint16                	FillPktSeqCount;
   TO_FillPacket_t       	FillPacket;
   uint32                	uiTotalVCDUOutputCount;
   TO_FrameManagement_t  	Frame;
   uint32					ChildTaskMutex;
} TO_AppData_T;

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

void  TO_OpenTlmSock(void);
int32 TO_AppInit(void);
void  TO_Cleanup(void);
void  TO_ProcessCmdMsg(void);
void  TO_ProcessSchMsg(void);
void  TO_OutputStatus(void);
void  TO_ForwardTelemetry(uint32 PrioMask);
void  TO_AppMain(void);
int32 TO_TableInit (void);
int32 TO_ValidateTable (TO_Table_t*);
int32 TO_RecvMsg( int32 timeout);


#endif  /* _TO_APP_H_ */

/* ---------- end of file to_app.h ----------*/
