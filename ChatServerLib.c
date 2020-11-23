/*
/*	Author:		Edwin Cadiz
/*	Major:		Computer Science
/*	Due Date:	November 
/*	Course:		CSC328
/*	Professor:	Dr. Frye
/*	Assignment:	Chat Server Application Library
/*	Filename:	ChatServerLib.c
/*	Purpose:	Library for a client and server
/*  Compile:    make
*/

#include "ChatServerLib.h"


//return -1 if error
//return bytes send
int sendMessage(int sockfd, Proto pro, char* nameStr, char* messageStr, int nameSize, int messageSize){

	int tempSize = 2;
	char *tempBuf = NULL;
	//printf("\nnameStr: %s", nameStr);
	//printf("\nmessageStr: %s", messageStr);
	int nameRealSize = 0;
	int messageRealSize = 0;
	if(nameStr){
		char *tempName = nameStr;
		while(*tempName != '\0'){
			nameRealSize++;
			tempName ++;
		
		}
	}
	
	if(messageStr){
		char *tempMessage = messageStr;
		while(*tempMessage != '\0'){
			tempMessage++;
			messageRealSize++;
		}
	}
	
	//if the nameSize given is less than the real size of the name than an error is printed
	if(nameRealSize > nameSize){
		printf("Name is longer than allowed");
		return -1;
	}
	//if the messageSize is less than the real size of the message than an error is printed
	if(messageRealSize > messageSize){
		printf("message is longer than allowed");
		return -1;
	}
	
	//printf("\nnameRealSize: %i", nameRealSize);
	//printf("\nmessageRealSize: %i", messageRealSize);
	
	switch (pro){
		case 0://HELLO
		case 1://BYE
		case 3://READY
		case 4://RETRY
			//only the protocol number and a null terminator are sent
			tempBuf = (char*)malloc(tempSize);
			sprintf(tempBuf, "%i", pro);
			tempBuf[tempSize- 1] = '\0';
			break;
		case 5://CHAT
			//protocol number, name(if it is not null), " ", and the message(if it is not null)
			tempSize = 1 + nameRealSize + 1;	
			if(messageStr)
				tempSize += messageRealSize + 1;
			
			tempBuf = (char*)malloc(tempSize);
			stpcpy(tempBuf, "5");
			if(nameStr)
				strcat(tempBuf, nameStr);
			strcat(tempBuf, " ");
			if(messageStr)
				strcat(tempBuf, messageStr);
			tempBuf[tempSize-1] = '\0';
			
			break;
		case 2://NICK
			tempSize = 1 + nameRealSize + 1;
			tempBuf = (char*)malloc(tempSize);
			strcpy(tempBuf,  "2");
			strcat(tempBuf, nameStr);
			tempBuf[tempSize-1] = '\0';
			break;
		default:
			printf("what?");
			return -1;
			break;
	};
	
	
	printf("\nTEMPBUF: [%s]",tempBuf);
	printf("\ntempSize: %i\n",tempSize);
	

	int bytesSent = send(sockfd, tempBuf, tempSize, 0);
	//int bytesSent = writeMessage(sockfd, tempBuf, tempSize);
	if(bytesSent < 0){
		perror("ERROR SENDING");
		free(tempBuf);
		return -1;
	}
	free(tempBuf);
	return bytesSent;
}

int receiveMessage(int sockfd, void* buf, int size){	
	int bytesRead = recv(sockfd, buf, size, 0);
	//int bytesRead = readMessage(sockfd, buf, size);
	if(bytesRead < 0){
		perror("ERROR RECEVING");
		return -1;
	}
	return bytesRead;
}

  
//name: msg(eof)
//-1 on error
//0 on success
int getInfo(struct messageInfo* msgStruct, char* buffer){

	//printf("\nbuffer: %s\n", buffer);

	int i = 0, err = 0;
	int bufI;
	//get protocol from buffer
	msgStruct->protocol = atoi(buffer);
	buffer++;
	bufI++;
	//if the character after protocol is null set all other member vars to 0 or null
	if(*buffer == '\0'){
		msgStruct->name[0] = '\0';
		msgStruct->msg[0] = '\0';
		msgStruct->msgSize = 0;
		msgStruct->nameSize = 0;
		return 0;
	}
	//take name
	while(*buffer != '\0'){

		if(*buffer == ' '){
			bufI++;
			buffer++;
			break;
		}
		msgStruct->name[i] = *buffer;
		i++;
		bufI++;
		buffer++;
	}
	msgStruct->name[i] = '\0';
	msgStruct->nameSize = i;
	
	//if next char is a null return should be nick
	if(*buffer == '\0'){
		msgStruct->msg[i] = '\0';
		msgStruct->msgSize = 0;
		return 0;
	}
	i = 0;
	
	//take message for chat
	while(*buffer != '\0'){
		msgStruct->msg[i] = *buffer;
		i++;
		bufI++;
		buffer++;
	}
	msgStruct->msg[i] = '\0';
	msgStruct->msgSize = i;
	return err;
}

/*
*	Function name:	readMessage
*	Description:	read from file descriptor complete message
*	Parameters:		int sockfd, char* buffer, int size
*	Return:			int - bytes read, 0 if complete
*                        -1 on fail
*/
int readMessage(int sockfd, char *buffer, int size){

    int bytesLeft = size, bytesRead;
    while(bytesLeft > 0){
        //how many bytes are read
        bytesRead = read(sockfd, buffer, size);
        if(bytesRead < 0){
            //error reading
            //perror("error reading");
            return -1;
        }else if(bytesRead == 0){
            //No more bytes left to read
            return 0;
        }
        bytesLeft -= bytesRead;
        buffer += bytesRead;
    }
    return size - bytesLeft;

}

/*
*	Function name:	writeMessage
*	Description:	write to file descriptor a complete message
*	Parameters:		int sockfd, char *buffer, int size
*	Return:			int - bytes read, 0 if complete
*                        -1 on fail
*/
int writeMessage(int sockfd, char *buffer, int size){
    int bytesLeft = size, bytesWritten;
    while(bytesLeft > 0){
        bytesWritten = write(sockfd, buffer, size);
        if(bytesWritten < 0){
            //perrror("error writing");
            return -1;
        }else if(bytesWritten == 0){
            return 0;
        }
        bytesLeft -= bytesWritten;
        buffer += bytesLeft;
    }
    return size - bytesLeft;
}

