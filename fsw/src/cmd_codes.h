#ifndef _TO_CMD_CODES_H_
#define _TO_CMD_CODES_H_

#define TO_NOP_CC                	0       /*  no-op command     */
#define TO_RESET_STATUS_CC       	1       /*  reset status      */
#define TO_ADD_PKT_CC            	2       /*  add packet        */
#define TO_SEND_DATA_TYPES_CC    	3       /*  send data types   */
#define TO_REMOVE_PKT_CC         	4       /*  remove packet     */
#define TO_REMOVE_ALL_PKT_CC		5       /*  remove all packet */
#define TO_OUTPUT_ENABLE_CC      	6       /*  output enable     */
#define TO_UPDATE_CLCW_CC        	7
#define TO_OUTPUT_SER_ENABLE_CC		8       /*  output serial enable */
#define TO_SET_TLM_SERIAL_FD_CC    	9
#define TO_SET_CMD_SERIAL_FD_CC    	10
#define TO_SET_SERIAL_DEV_CC       	11
#define TO_OUTPUT_SER_DISABLE_CC	12       /*  output serial disable */

#endif
