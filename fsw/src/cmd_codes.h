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
