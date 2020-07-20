#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtptimeutilities.h>
#include <jrtplib3/rtppacket.h>
#include <stdlib.h>
#include <iostream>
#include "audio.h"

using namespace jrtplib;

int main(void)
{
#ifdef RTP_SOCKETTYPE_WINSOCK
	WSADATA dat;
	WSAStartup(MAKEWORD(2,2),&dat);
#endif // RTP_SOCKETTYPE_WINSOCK
		
	RTPSession session;
	
	RTPSessionParams sessionparams;
	sessionparams.SetOwnTimestampUnit(1.0/8000.0);
			
	RTPUDPv4TransmissionParams transparams;
	transparams.SetPortbase(8000);
			
	int status = session.Create(sessionparams,&transparams);
	if (status < 0)
	{
		std::cerr << RTPGetErrorString(status) << std::endl;
		exit(-1);
	}
	
	uint8_t localip[]={127,0,0,1};
	RTPIPv4Address addr(localip,9000);
	
	status = session.AddDestination(addr);
	if (status < 0)
	{
		std::cerr << RTPGetErrorString(status) << std::endl;
		exit(-1);
	}
	
	session.SetDefaultPayloadType(96);
	session.SetDefaultMark(false);
	session.SetDefaultTimestampIncrement(160);
	
    //Deadcode, keeping it for debugging purposes, will remove later
	uint8_t silencebuffer[160];
	for (int i = 0 ; i < 160 ; i++)
		silencebuffer[i] = 128;

    WavData song;
    //TODO
    //Bad practice, hardcoded filename, will fix later
    loadWaveFile("/Users/tashaffi/Documents/office work data/SRTP project/src/audio.wav", &song);

	RTPTime delay(0.020);
    
	RTPTime starttime = RTPTime::CurrentTime();

    int count = 0;
	while (count <= song.getSampleCount())
	{
        //TODO
        //sends one interger at a time, but packets can hold more, send more data with one packet
		status = session.SendPacket(&song.getData()[count], sizeof(song.getData()[count]));
		if (status < 0)
		{
			std::cerr << RTPGetErrorString(status) << std::endl;
			exit(-1);
		}
		
		session.BeginDataAccess();
		if (session.GotoFirstSource())
		{
			do
			{
				RTPPacket *packet;

				while ((packet = session.GetNextPacket()) != 0)
				{
					std::cout << "Got packet with " 
					          << "extended sequence number " 
					          << packet->GetExtendedSequenceNumber() 
					          << " from SSRC " << packet->GetSSRC() 
					          << std::endl;
					session.DeletePacket(packet);
				}
			} while (session.GotoNextSource());
		}
		session.EndDataAccess();
			
		RTPTime::Wait(delay);
    
		count ++;
	}
	
	delay = RTPTime(10.0);
	session.BYEDestroy(delay,"Time's up",9);
	
#ifdef RTP_SOCKETTYPE_WINSOCK
	WSACleanup();
#endif // RTP_SOCKETTYPE_WINSOCK
	return 0;
}