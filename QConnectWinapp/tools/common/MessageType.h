/*
 * MessageType.h
 *
 *  Created on: Jul 29, 2023
 *      Author: cuongnht
 */

#ifndef MESSAGETYPE_H_
#define MESSAGETYPE_H_

enum class MessageType {
    INITILIZED_REQUEST_MSG=1,
    WINDOW_INFOR_MSG,
    WINDOW_ID_REQUEST_MSG,
    WINDOW_ID_RESPONSE_MSG,
    FIND_MATCH_REQUEST_MSG,
    FIND_MATCH_RESPONSE_MSG,
    ACKNOWLEDGE_MSG
};

const int MAX_MSG_SIZE = 8192;
struct Message {
    long mType; // Message type (must be a positive integer)
    char mText[MAX_MSG_SIZE]; // Message content
};


#endif /* MESSAGETYPE_H_ */
