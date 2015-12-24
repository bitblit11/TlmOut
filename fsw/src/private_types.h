#ifndef _TO_PRIVATE_TYPES_H_
#define _TO_PRIVATE_TYPES_H_

#define "common_types.h"

/*
** Local Structure Declarations
*/
#if defined(_ix86_)

typedef struct
{
	uint16 spare:2;
	uint16 chann_id:6;
	uint16 cop_in_effect:2;
	uint16 status:3;
	uint16 version:2;
    uint16 word_type:1;

    uint16 value:8;
    uint16 rep_type:1;
    uint16 farm_b_ctr:2;
    uint16 retransmit:1;
    uint16 wait:1;
    uint16 lockout:1;
    uint16 no_bitlock:1;
    uint16 no_rf:1;

} RCM_CLCW_T;

#else

typedef struct
{
     uint16 word_type:1;
     uint16 version:2;
     uint16 status:3;
     uint16 cop_in_effect:2;
     uint16 chann_id:6;
     uint16 spare:2;

     uint16 no_rf:1;
     uint16 no_bitlock:1;
     uint16 lockout:1;
     uint16 wait:1;
     uint16 retransmit:1;
     uint16 farm_b_ctr:2;
     uint16 rep_type:1;
     uint16 value:8;

} RCM_CLCW_T;

#endif

typedef struct
{
     CCSDS_PriHdr_t               hdr;
     CCSDS_CmdSecHdr_t            sec_hdr;
     RCM_CLCW_T                  ClcwData;

} RCM_CLCW_TLM_T;

#define RCM_CLCW_MSG_LENGTH sizeof(RCM_CLCW_TLM_T)

/* Set File Descriptor Cmd Data Structure */
typedef struct
{
     CCSDS_PriHdr_t               hdr;
     CCSDS_CmdSecHdr_t            sec_hdr;
     int                          iSerialFD;

} RCM_SET_FD_T;

#define RCM_SET_FD_MSG_LENGTH sizeof(RCM_SET_FD_T)


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

#endif
