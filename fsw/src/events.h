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

#ifndef _TO_EVENTS_H_
#define _TO_EVENTS_H_

/*
** Pragmas
*/

/*
** Include Files
*/

/*
** Local Defines
*/

/* Event message ID's */
#define TO_EVM_RESERVED            0

#define TO_INIT_INF_EID            1
#define TO_CTODPIPE_ERR_EID       2
#define TO_TLMOUTENA_INF_EID       3
#define TO_SUBSCRIBE_ERR_EID      	4
#define TO_TLMPIPE_ERR_EID        	5
#define TO_TLMOUTSOCKET_ERR_EID   	6
#define TO_TLMOUTSTOP_ERR_EID     	7
#define TO_MSGID_ERR_EID          	8
#define TO_FNCODE_ERR_EID      	9
#define TO_ADDPKT_ERR_EID         	10
#define TO_REMOVEPKT_ERR_EID      	11
#define TO_REMOVEALLPTKS_ERR_EID  	12
#define TO_REMOVECMDTO_ERR_EID    	13
#define TO_REMOVEHKTO_ERR_EID     	14
#define TO_ADDPKT_INF_EID         	15
#define TO_REMOVEPKT_INF_EID      	16
#define TO_REMOVEALLPKTS_INF_EID  	17
#define TO_NOOP_INF_EID           	18
#define TO_UPDATE_CLCW_ERR_EID		19
#define TO_PACKET_LOST_ERR_EID    	20
#define TO_SET_FD_EID             	21
#define TO_CONFIG_REG_ERR_EID      	22
#define TO_CONFIG_LD_ERR_EID       	23
#define TO_CONFIG_MNG_ERR_EID      	24
#define TO_CONFIG_GADR_ERR_EID     	25
#define TO_CONFIG_INF_EID          	26

#define TO_CMDINSOCKET_ERR_EID		27
#define TO_CMDSER_ERR_EID			28
#define TO_CMD_INGEST_ERR_EID		29
#define TO_CMD_TRANSFER_FRAME_FAILED_EID	30
#define TO_SEQ_NOT_FOUND_EID		31
#define TO_ERR_BAD_CODE_BLOCK_EID	32
#define TO_ERR_TF_NEGATIVE_SLIDING_EDGE_EID 33
#define TO_ERR_TF_POSITIVE_SLIDING_EDGE_EID 34
#define TO_INVALID_TRANSFER_FRAME_LENGTH_EID 35
#define TO_ERR_TF_LOCKOUT_MODE_EN_EID 36
#define TO_TF_CMD_REJECTED_LOCKOUT_MODE_EN_EID 37
#define TO_ERR_PKT_CONSTRUCT_FAILURE_EID 38
#define TO_ERR_PKT_RECONSTRUCT_EID 39
#define TO_ERR_CHECKSUM_EID 40
#define TO_TRANSFER_FRAME_DISREGARDED_EID 41
#define TO_TRANSFER_LAYER_UNLOCK_EID 42
#define TO_TRANSFER_FRAME_CNT_RESET_EID 43
#define TO_RESYNC_CMD_IGNORE_EID 44
#define TO_INV_BYPASS_CMD_EID 45
#define TO_TLMTASK_CREATE_ERR_EID 46


/*
** Local Structure Declarations
*/

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

#endif  /* _TO_EVENTS_H_ */

/* ---------- end of file to_events.h ----------*/
