#ifndef _TESTS_H
#define _TESTS_H



void TestReadByte ();
void TestWriteByte ();

void TestReadWord ();
void TestWriteWord ();

void TestReadDM9000_ID ();

void TestGPIO ();

void TestBdelayMilli ();
void TestBdelayMicro ();
void TestPWMBuzzer ();
void TestTimerIsr();

void SetIP();
void TestPing();
void TestPingServer();

/*----game functions----*/
void receive_packet();
void start_eth_device();
/*----------------------*/


#endif
