// BOF preprocessor bug prevent - insert me on top of your arduino-code
#if 1
__asm volatile ("nop");
#endif

/*
 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Deviation is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Deviation.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <Arduino.h>
#if defined (__SAM3X8E__) 
	//#include <DueTimer.h>
#elif defined (__AVR_ATmega2560__ )
	//#include <TimerThree.h>
#else
	#include <TimerOne.h>
#endif

#include "a7105.h"
#include "config.h"
#include "model.h"

#ifdef MODULAR
  //Allows the linker to properly relocate
  #define FLYSKY_Cmds PROTO_Cmds
  #pragma long_calls
#endif



#ifdef MODULAR
  #pragma long_calls_off
  extern unsigned _data_loadaddr;
  const unsigned long protocol_type = (unsigned long)&_data_loadaddr;
#endif


//Fewer bind packets in the emulator so we can get right to the important bits
#ifdef EMULATOR
#define BIND_COUNT 3
#else
#define BIND_COUNT 2500
#endif
/*
static const char * const flysky_opts[] = {
  "WLToys V9x9",  _tr_noop("Off"), _tr_noop("On"), NULL,
  NULL
};*/
#define WLTOYS_ON 1
#define WLTOYS_OFF 0
enum {
    PROTOOPTS_WLTOYS = 0,
};
static const u8 A7105_regs[] = {
     -1,  0x42, 0x00, 0x14, 0x00,  -1 ,  -1 , 0x00, 0x00, 0x00, 0x00, 0x01, 0x21, 0x05, 0x00, 0x50,
    0x9e, 0x4b, 0x00, 0x02, 0x16, 0x2b, 0x12, 0x00, 0x62, 0x80, 0x80, 0x00, 0x0a, 0x32, 0xc3, 0x0f,
    0x13, 0xc3, 0x00,  -1,  0x00, 0x00, 0x3b, 0x00, 0x17, 0x47, 0x80, 0x03, 0x01, 0x45, 0x18, 0x00,
    0x01, 0x0f,  -1,
};
static const u8 tx_channels[16][16] = {
  {0x0a, 0x5a, 0x14, 0x64, 0x1e, 0x6e, 0x28, 0x78, 0x32, 0x82, 0x3c, 0x8c, 0x46, 0x96, 0x50, 0xa0},
  {0xa0, 0x50, 0x96, 0x46, 0x8c, 0x3c, 0x82, 0x32, 0x78, 0x28, 0x6e, 0x1e, 0x64, 0x14, 0x5a, 0x0a},
  {0x0a, 0x5a, 0x50, 0xa0, 0x14, 0x64, 0x46, 0x96, 0x1e, 0x6e, 0x3c, 0x8c, 0x28, 0x78, 0x32, 0x82},
  {0x82, 0x32, 0x78, 0x28, 0x8c, 0x3c, 0x6e, 0x1e, 0x96, 0x46, 0x64, 0x14, 0xa0, 0x50, 0x5a, 0x0a},
  {0x28, 0x78, 0x0a, 0x5a, 0x50, 0xa0, 0x14, 0x64, 0x1e, 0x6e, 0x3c, 0x8c, 0x32, 0x82, 0x46, 0x96},
  {0x96, 0x46, 0x82, 0x32, 0x8c, 0x3c, 0x6e, 0x1e, 0x64, 0x14, 0xa0, 0x50, 0x5a, 0x0a, 0x78, 0x28},
  {0x50, 0xa0, 0x28, 0x78, 0x0a, 0x5a, 0x1e, 0x6e, 0x3c, 0x8c, 0x32, 0x82, 0x46, 0x96, 0x14, 0x64},
  {0x64, 0x14, 0x96, 0x46, 0x82, 0x32, 0x8c, 0x3c, 0x6e, 0x1e, 0x5a, 0x0a, 0x78, 0x28, 0xa0, 0x50},
  {0x50, 0xa0, 0x46, 0x96, 0x3c, 0x8c, 0x28, 0x78, 0x0a, 0x5a, 0x32, 0x82, 0x1e, 0x6e, 0x14, 0x64},
  {0x64, 0x14, 0x6e, 0x1e, 0x82, 0x32, 0x5a, 0x0a, 0x78, 0x28, 0x8c, 0x3c, 0x96, 0x46, 0xa0, 0x50},
  {0x46, 0x96, 0x3c, 0x8c, 0x50, 0xa0, 0x28, 0x78, 0x0a, 0x5a, 0x1e, 0x6e, 0x32, 0x82, 0x14, 0x64},
  {0x64, 0x14, 0x82, 0x32, 0x6e, 0x1e, 0x5a, 0x0a, 0x78, 0x28, 0xa0, 0x50, 0x8c, 0x3c, 0x96, 0x46},
  {0x46, 0x96, 0x0a, 0x5a, 0x3c, 0x8c, 0x14, 0x64, 0x50, 0xa0, 0x28, 0x78, 0x1e, 0x6e, 0x32, 0x82},
  {0x82, 0x32, 0x6e, 0x1e, 0x78, 0x28, 0xa0, 0x50, 0x64, 0x14, 0x8c, 0x3c, 0x5a, 0x0a, 0x96, 0x46},
  {0x46, 0x96, 0x0a, 0x5a, 0x50, 0xa0, 0x3c, 0x8c, 0x28, 0x78, 0x1e, 0x6e, 0x32, 0x82, 0x14, 0x64},
  {0x64, 0x14, 0x82, 0x32, 0x6e, 0x1e, 0x78, 0x28, 0x8c, 0x3c, 0xa0, 0x50, 0x5a, 0x0a, 0x96, 0x46},
};
static u32 id;
//static const u8 id[] = { 0x02, 0x00, 0x00, 0x70 };
static u8 chanrow;
static u8 chancol;
static u8 chanoffset;
static u8 packet[21];
static u16 counter;

//The folloiwng code came from: http://notabs.org/winzipcrc/winzipcrc.c
// C99 winzip crc function, by Scott Duplichan
//We could use the internal CRC implementation in the STM32, but this is really small
//and perfomrance isn't really an issue
static u32 Crc(const void *buffer, u32 size)
{
	u32 crc = ~0;
	const u8  *position = (u8  *)buffer;

	while (size--)
	{
		int bit;
		crc ^= *position++;
		for (bit = 0; bit < 8; bit++)
		{
			s32 out = crc & 1;
			crc >>= 1;
			crc ^= -out & 0xEDB88320;
		}
	}
	return ~crc;
}

static u8 proto_state;
static u32 bind_time;
#define PROTO_DEINIT    0x00
#define PROTO_INIT      0x01
#define PROTO_READY     0x02
#define PROTO_BINDING   0x04
#define PROTO_BINDDLG   0x08
#define PROTO_MODULEDLG 0x10

static void PROTOCOL_SetBindState(u32 msec)
{
	if (msec) {
		if (msec == 0xFFFFFFFF)
		bind_time = msec;
		else
		bind_time = millis() + msec;
		proto_state |= PROTO_BINDING;
		//PROTOCOL_SticksMoved(1);  //Initialize Stick position
	} else {
		proto_state &= ~PROTO_BINDING;
	}
}




// Channels should be volatile:
// This array is written from the main event loop
// and read from an interrupt service routine.
// volatile makes sure, each access to the array
// will be an actual access to the memory location
// an element is stored in.
// If it is omitted, the optimizer might create a
// 'short cut', removing seemingly unneccessary memory accesses,
// and thereby preventing the propagation of an update from
// the main loop to the interrupt routine (since the optimizer
// has no clue about interrupts)

volatile s16 Channels[NUM_OUT_CHANNELS];








static int flysky_init()
{
    int i;
    u8 if_calibration1;
    u8 vco_calibration0;
    u8 vco_calibration1;
	u8 vco_read;

    A7105_WriteID(0x5475c52a);
    for (i = 0; i < 0x33; i++)
        if((s8)A7105_regs[i] != -1)
            A7105_WriteReg(i, A7105_regs[i]);

    A7105_Strobe(A7105_STANDBY);
	
//    vco_read = A7105_ReadReg(0x00);
//    printf("%d  vco_read=%d\n", __LINE__, vco_read);

    //IF Filter Bank Calibration
    A7105_WriteReg(0x02, 1);
    u32 ms = millis();
    while(millis()  - ms < 500) {
        if(! A7105_ReadReg(0x02))
            break;
    }
    if (millis() - ms >= 500) {
		printf("flysky_init() - failed! (%d)\n", __LINE__);
        return 0;
	}
    if_calibration1 = A7105_ReadReg(0x22);
    if(if_calibration1 & A7105_MASK_FBCF) {
        //Calibration failed...what do we do?
		printf("flysky_init() - failed! (%d)\n", __LINE__);
        return 0;
    }

    //VCO Current Calibration
    A7105_WriteReg(0x24, 0x13); //Recomended calibration from A7105 Datasheet

    //VCO Bank Calibration
    A7105_WriteReg(0x26, 0x3b); //Recomended limits from A7105 Datasheet

    //VCO Bank Calibrate channel 0?
    //Set Channel
    A7105_WriteReg(0x0f, 0); //Should we choose a different channel?
    //VCO Calibration
    A7105_WriteReg(0x02, 2);
    ms = millis();
    while(millis()  - ms < 500) {
        if(! A7105_ReadReg(0x02))
            break;
    }
    if (millis() - ms >= 500) {
		printf("flysky_init() - failed! (%d)\n", __LINE__);
        return 0;
	}
    vco_calibration0 = A7105_ReadReg(0x25);
    if (vco_calibration0 & A7105_MASK_VBCF) {
        //Calibration failed...what do we do?
		printf("flysky_init() - failed! (%d)\n", __LINE__);
        return 0;
    }

    //Calibrate channel 0xa0?
    //Set Channel
    A7105_WriteReg(0x0f, 0xa0); //Should we choose a different channel?
    //VCO Calibration
    A7105_WriteReg(0x02, 2);
    ms = millis();
    while(millis()  - ms < 500) {
        if(! A7105_ReadReg(A7105_02_CALC))
            break;
    }
    if (millis() - ms >= 500) {
		printf("flysky_init() - failed! (%d)\n", __LINE__);
        return 0;
	}
    vco_calibration1 = A7105_ReadReg(0x25);
    if (vco_calibration1 & A7105_MASK_VBCF) {
        //Calibration failed...what do we do?
		printf("flysky_init() - failed! (%d)\n", __LINE__);
        return 0;
    }

    //Reset VCO Band calibration
    A7105_WriteReg(0x25, 0x08);

    A7105_SetTxRxMode(TX_EN);
    A7105_SetPower(Model.tx_power);

    A7105_Strobe(A7105_STANDBY);
	
	printf("flysky_init() - OK!\n");
    return 1;
}


// 05 AA     5C 29 00 90 or			05 55	5C 29 00 90
// F1 E7							DC 05
// 48 94							F4 05					
// 95 FD							E8 03	
// 98 EB							DC 05	
// BD 19							EA 02	
// FE 0F							FF 0F
// 33 8C							19 00
// EA B2							00 00
// pause
// 0F 01							0F 91



//				Roll   |	Pitch 	|  Throttle|	Yaw    | Data 4 | Data 5 | Data 6 | Data 7
//				Data 0 |	Data 1	|	Data 2 |	Data 3 | Data 4 | Data 5 | Data 6 | Data 7
//	
// Idle			DC 05		F4 05		E8 03		79 05	 E0 02	  36 00		19 00	00 00
// Idle			DC 05		F4 05		E8 03		79 05	 DA 02	  FE 0F		19 00	00 00   (left button 1)
// Idle			DC 05		F4 05		E8 03		47 05	 DB 02	  36 00		19 00	00 00	(left button 2)
// Idle			DC 05		F4 05		E8 03		47 05	 DA 02	  3C 00		19 00	00 00	(right button 1)
// Idle			DC 05		F4 05		E8 03		47 05	 DA 02	  3C 00		19 00	00 00	(right button 2)
// Yaw Left		DC 05		F4 05		E8 03		DC 05	 DD 02	  27 00		19 00	00 00
// Yaw Left		DC 05		F4 05		E8 03		79 05	 E1 02	  3C 00		19 00	00 00
// Yaw Right	DC 05		F4 05		E8 03		DC 05	 E3 02	  3C 00		19 00	00 00
// Pitch Back	DC 05		57 06		E8 03		79 05	 DF 02	  FF 0F		19 00	00 00
// Pitch Forw	DC 05		91 05		E8 03		79 05	 DF 02	  FE 0F		19 00	00 00
// Roll Right	3F 06		F4 05		E8 03		79 05	 DE 02	  FE 0F		19 00	00 00
// Roll Left	79 05		F4 05		E8 03		79 05	 DE 02	  FE 0F		19 00	00 00
	

static void flysky_build_packet(u8 init)
{
    int i;
    //-100% =~ 0x03e8
    //+100% =~ 0x07ca
    //Calculate:
    //Center = 0x5d9
    //1 %    = 5
    packet[0] = init ? 0xaa : 0x55;
    packet[1] = (id >>  0) & 0xff;
    packet[2] = (id >>  8) & 0xff;
    packet[3] = (id >> 16) & 0xff;
    packet[4] = (id >> 24) & 0xff;
    for (i = 0; i < 8; i++) {
        if (i > Model.num_channels) {
            packet[5 + i*2] = 0;
            packet[6 + i*2] = 0;
            continue;
        }
//        s32 value = (s32)Channels[i] * 0x1f1 / CHAN_MAX_VALUE + 0x5d9;

		s32 value = 0;
		switch (i) {
			case 0:
			case 3:
				value = (s32)Channels[i] * 0x1f1 / CHAN_MAX_VALUE + 0x5dc;
				break;
			case 1:
				value = (s32)Channels[i] * 0x1f1 / CHAN_MAX_VALUE + 0x5f4;
				break;
			case 2:
				value = (s32)Channels[i] * 0x1f1 / CHAN_MAX_VALUE + 0x3e8;
		}
        if (value < 0)
            value = 0;
        packet[5 + i*2] = value & 0xff;
        packet[6 + i*2] = (value >> 8) & 0xff;
    }
//    if (Model.proto_opts[PROTOOPTS_WLTOYS] == WLTOYS_ON) {
        if(Channels[4] > 0)
            packet[12] |= 0x20;
        if(Channels[5] > 0)
            packet[10] |= 0x40;
        if(Channels[6] > 0)
            packet[10] |= 0x80;
        if(Channels[7] > 0)
            packet[12] |= 0x10;

    //}
}


//static u16 flysky_cb()
static void flysky_cb()
{
    if (counter) {
        flysky_build_packet(1);
        A7105_WriteData(packet, 21, 1);
        counter--;
        if (! counter)
            PROTOCOL_SetBindState(0);
    } else {
        flysky_build_packet(0);
        A7105_WriteData(packet, 21, tx_channels[chanrow][chancol]-chanoffset);
        chancol = (chancol + 1) % 16;
        if (! chancol) //Keep transmit power updated
            A7105_SetPower(Model.tx_power);
    }
    //return 1460;
}

static void initialize(u8 bind) {

    while(1) {
        //A7105_Reset();
        if (flysky_init())
            break;
    }
    if (Model.fixed_id) {
        id = Model.fixed_id;
    } else {
        //id = (Crc(&Model, sizeof(Model)) + Crc(&Transmitter, sizeof(Transmitter))) % 999999;
		id = rand();
    }
    chanrow = id % 16;
    chancol = 0;
    chanoffset = (id & 0xff) / 16;
    if (bind || ! Model.fixed_id) {
        counter = BIND_COUNT;
        PROTOCOL_SetBindState(2500 * 1460 / 1000); //msec
    } else {
        counter = 0;
    }
    //CLOCK_StartTimer(2400, flysky_cb);
#if defined (__arm__) && defined (__SAM3X8E__) 
	Timer3.attachInterrupt(flysky_cb).start(1250); //2400
#elif defined (__AVR_ATmega328P__)
	Timer1.attachInterrupt(flysky_cb, 1250);
#else
	Timer3.attachInterrupt(flysky_cb, 1250);
#endif

	printf("Init ok session id=0x%x channel=0x%x\n", id, chanrow);
}


const void *FLYSKY_Cmds(enum ProtoCmds cmd)
{
    switch(cmd) {
        case PROTOCMD_INIT:  initialize(0); return 0;
        case PROTOCMD_DEINIT:
        case PROTOCMD_RESET:
            //CLOCK_StopTimer();
#if defined (__AVR_ATmega328P__)
			Timer1.detachInterrupt();
#else
			Timer3.detachInterrupt();
#endif
			
            return (void *)(A7105_Reset() ? 1L : -1L);
        case PROTOCMD_CHECK_AUTOBIND: return Model.fixed_id ? 0 : (void *)1L;
        case PROTOCMD_BIND:  initialize(1); return 0;
        case PROTOCMD_NUMCHAN: return (void *)8L;
        case PROTOCMD_DEFAULT_NUMCHAN: return (void *)8L;
        case PROTOCMD_CURRENT_ID: return (void *)((unsigned long)id);
//        case PROTOCMD_GETOPTIONS:
//            return flysky_opts;
        case PROTOCMD_TELEMETRYSTATE: return (void *)(long)PROTO_TELEM_UNSUPPORTED;
        default: break;
    }
    return 0;
}
