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

#ifndef _TO_CMDS_H_
#define _TO_CMDS_H_

/*
** Pragmas
*/

/*
** Include Files
*/
#include "cfe.h"
#include "common_types.h"
#include "msg.h"



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


void TO_CmdExec(CFE_SB_MsgPtr_t msg);
void TO_ResetStatus(void);
void TO_OutputDataTypesPkt(void);
void TO_EnableDownlink( TO_OUTPUT_ENABLE_PKT_t * pCmd );
void TO_AddPkt(TO_ADD_PKT_t* cmd);
void TO_RemovePkt(TO_REMOVE_PKT_t * cmd);
void TO_RemoveAllPkt(void);


#endif  /* _TO_APP_H_ */

/* ---------- end of file to_app.h ----------*/
