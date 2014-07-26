/*
 * V9x9protocol.h
 *
 * Created: 18/07/2014 23:03:43
 *  Author: Miro
 */ 


#ifndef V9X9PROTOCOL_H_
#define V9X9PROTOCOL_H_

extern volatile s16 Channels[NUM_OUT_CHANNELS];

void *FLYSKY_Cmds(enum ProtoCmds cmd);


#endif /* V9X9PROTOCOL_H_ */