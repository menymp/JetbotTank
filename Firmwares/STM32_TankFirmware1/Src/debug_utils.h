/*
 * debug_utils.h
 *
 *  Created on: 19/08/2023
 *      Author: menymp
 */

#ifndef DEBUG_UTILS_H_
#define DEBUG_UTILS_H_

/*UNKNOWN COMMAND, TIMEOUT, MISSING TERMINATOR CHAR, NOT IMPLEMENTED*/
#define ERR_UNKNOWN_COMMAND_CODE			10
#define	ERR_UNKNOWN_COMMAND_MESSAGE			"unknown command"
#define ERR_UNKNOWN_COMMAND_MESSAGE_LEN		sizeof(ERR_UNKNOWN_COMMAND_MESSAGE)

#define ERR_SYS_TIMEOUT_CODE				20
#define	ERR_SYS_TIMEOUT_MESSAGE				"sys timeout"
#define ERR_SYS_TIMEOUT_MESSAGE_LEN			sizeof(ERR_SYS_TIMEOUT_MESSAGE)

#define ERR_MISSING_TERMINATOR_CODE			30
#define	ERR_MISSING_TERMINATOR_MESSAGE		"missing terminator ;"
#define ERR_MISSING_TERMINATOR_MESSAGE_LEN	sizeof(ERR_MISSING_TERMINATOR_MESSAGE)

#define ERR_NOT_IMPLEMENTED_CODE			40
#define	ERR_NOT_IMPLEMENTED_MESSAGE			"command not implemented"
#define ERR_NOT_IMPLEMENTED_MESSAGE_LEN		sizeof(ERR_NOT_IMPLEMENTED_MESSAGE)

#endif /* DEBUG_UTILS_H_ */
