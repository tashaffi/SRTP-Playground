    /*
   This IPv4 example uses the background thread itself to process all packets.
   You can use example one to send data to the session that's created in this
   example.
*/

#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtppacket.h>
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtperrors.h>
#include <jrtplib3/rtpsourcedata.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "stream.h"
using namespace std;

using namespace jrtplib;

#ifdef RTP_SUPPORT_THREAD

//
// This function checks if there was a RTP error. If so, it displays an error
// message and exists.
//

void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		exit(-1);
	}
}

//
// The new class routine
//

class MyRTPSession : public RTPSession
{
public: 
    sf::Int16* data;
    int count;
protected:
	void OnPollThreadStep();
	void ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack);
};

void MyRTPSession::OnPollThreadStep()
{
	BeginDataAccess();
		
	// check incoming packets
	if (GotoFirstSourceWithData())
	{
		do
		{
			RTPPacket *pack;
			RTPSourceData *srcdat;
			
			srcdat = GetCurrentSourceInfo();
			
			while ((pack = GetNextPacket()) != NULL)
			{
				ProcessRTPPacket(*srcdat,*pack);
				DeletePacket(pack);
			}
		} while (GotoNextSourceWithData());
	}
		
	EndDataAccess();
}

void MyRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	// You can inspect the packet and the source's info here
	std::cout << "Got packet " << rtppack.GetExtendedSequenceNumber() << " from SSRC " << srcdat.GetSSRC() << std::endl;
    data[count++] = *((sf::Int16*)rtppack.GetPayloadData());

}

//
// The main routine
// 

int main(void)
{
#ifdef RTP_SOCKETTYPE_WINSOCK
	WSADATA dat;
	WSAStartup(MAKEWORD(2,2),&dat);
#endif // RTP_SOCKETTYPE_WINSOCK
	
	MyRTPSession sess;
    //TODO
    //hardcoded array size, need to check if there are better ways to deal with this
    sess.data = new sf::Int16[1000000000];
    sess.count=0;
	uint16_t portbase;
	std::string ipstr;
	int status,num;

        // First, we'll ask for the necessary information
		
	std::cout << "Enter local portbase:" << std::endl;
	std::cin >> portbase;
	std::cout << std::endl;
	
	std::cout << std::endl;
	std::cout << "Number of seconds you wish to wait:" << std::endl;
	std::cin >> num;
	
	// Now, we'll create a RTP session, set the destination
	// and poll for incoming data.
	
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
	
	// IMPORTANT: The local timestamp unit MUST be set, otherwise
	//            RTCP Sender Report info will be calculated wrong
	// In this case, we'll be just use 8000 samples per second.
	sessparams.SetOwnTimestampUnit(1.0/8000.0);		
	
	transparams.SetPortbase(portbase);
	status = sess.Create(sessparams,&transparams);	
	checkerror(status);
	
	// Wait a number of seconds
	RTPTime::Wait(RTPTime(num,0));
	
	sess.BYEDestroy(RTPTime(10,0),0,0);

    cout << "count is " << sess.count << endl;

    sf::SoundBuffer buffer;
    const sf::Int16* dataReceived = sess.data;
    buffer.loadFromSamples(dataReceived, sess.count, 1, 22050);
    // initialize and play our custom stream
    MyStream stream;
    stream.load(buffer);
    stream.play();
    while(stream.getStatus() == MyStream::Playing)
    {
    }

#ifdef RTP_SOCKETTYPE_WINSOCK
	WSACleanup();
#endif // RTP_SOCKETTYPE_WINSOCK
	return 0;
}

#else

int main(void)
{
	std::cerr << "Thread support is required for this example" << std::endl;
	return 0;
}

#endif // RTP_SUPPORT_THREAD

//TODO
//Code for streaming
//Has bug, works with RTPTime delay(.00000020); from server side but not with other delays. Need to fix.
//Sometimes plays noise

    /*
   This IPv4 example uses the background thread itself to process all packets.
   You can use example one to send data to the session that's created in this
   example.
*/

// #include <jrtplib3/rtpsession.h>
// #include <jrtplib3/rtppacket.h>
// #include <jrtplib3/rtpudpv4transmitter.h>
// #include <jrtplib3/rtpipv4address.h>
// #include <jrtplib3/rtpsessionparams.h>
// #include <jrtplib3/rtperrors.h>
// #include <jrtplib3/rtpsourcedata.h>
// #include <stdlib.h>
// #include <stdio.h> 
// #include <iostream>
// #include <string>
// #include "stream.h"
// using namespace std;

// using namespace jrtplib;

// #ifdef RTP_SUPPORT_THREAD

// #include <SFML/Audio.hpp>
// #include <SFML/Network.hpp>
// #include <iomanip>
// #include <iterator>


// const sf::Uint8 audioData   = 1;
// const sf::Uint8 endOfStream = 2;


// ////////////////////////////////////////////////////////////
// /// Customized sound stream for acquiring audio data
// /// from the network
// ////////////////////////////////////////////////////////////
// class NetworkAudioStream : public sf::SoundStream
// {
// public:

//     void setStream(const sf::Int16* samples, std::size_t sampleCount){
//         {
//             sf::Lock lock(m_mutex);
//             std::copy(samples, samples + sampleCount, std::back_inserter(m_samples));
//         }
//     }

//     void setStreamingFinished(){
//         m_hasFinished = true;
//     }

//     ////////////////////////////////////////////////////////////
//     /// Default constructor
//     ///
//     ////////////////////////////////////////////////////////////
//     NetworkAudioStream() :
//     m_offset     (0),
//     m_hasFinished(false)
//     {
//         // Set the sound parameters
//         initialize(1, 22050);
//     }

//     ////////////////////////////////////////////////////////////
//     /// Run the server, stream audio data from the client
//     ///
//     ////////////////////////////////////////////////////////////
//     void start(unsigned short port)
//     {
//         if (!m_hasFinished)
//         {
//             // Start playback
//             play();
//         }
//         else
//         {
//             // Start playback
//             play();
//         }
//     }

// private:

//     ////////////////////////////////////////////////////////////
//     /// /see SoundStream::OnGetData
//     ///
//     ////////////////////////////////////////////////////////////
//     virtual bool onGetData(sf::SoundStream::Chunk& data)
//     {
//         cout << "is finished? " << m_hasFinished << endl;
//         // We have reached the end of the buffer and all audio data have been played: we can stop playback
//         if ((m_offset >= m_samples.size()) && m_hasFinished)
//             return false;

//         // No new data has arrived since last update: wait until we get some
//         while ((m_offset >= m_samples.size()) && !m_hasFinished)
//             sf::sleep(sf::milliseconds(10));

//         // Copy samples into a local buffer to avoid synchronization problems
//         // (don't forget that we run in two separate threads)
//         {
//             sf::Lock lock(m_mutex);
//             m_tempBuffer.assign(m_samples.begin() + m_offset, m_samples.end());
//         }

//         // Fill audio data to pass to the stream
//         data.samples     = &m_tempBuffer[0];
//         data.sampleCount = m_tempBuffer.size();

//         // Update the playing offset
//         m_offset += m_tempBuffer.size();

//         return true;
//     }

//     ////////////////////////////////////////////////////////////
//     /// /see SoundStream::OnSeek
//     ///
//     ////////////////////////////////////////////////////////////
//     virtual void onSeek(sf::Time timeOffset)
//     {
//         m_offset = timeOffset.asMilliseconds() * getSampleRate() * getChannelCount() / 1000;
//     }


//     ////////////////////////////////////////////////////////////
//     // Member data
//     ////////////////////////////////////////////////////////////
//     sf::TcpListener        m_listener;
//     sf::TcpSocket          m_client;
//     sf::Mutex              m_mutex;
//     std::vector<sf::Int16> m_samples;
//     std::vector<sf::Int16> m_tempBuffer;
//     std::size_t            m_offset;
//     bool                   m_hasFinished;
// };


// //
// // This function checks if there was a RTP error. If so, it displays an error
// // message and exists.
// //

// void checkerror(int rtperr)
// {
// 	if (rtperr < 0)
// 	{
// 		std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
// 		exit(-1);
// 	}
// }

// //
// // The new class routine
// //

// class MyRTPSession : public RTPSession
// {
// public: 
//     NetworkAudioStream stream;
// protected:
// 	void OnPollThreadStep();
// 	void ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack);
// };

// void MyRTPSession::OnPollThreadStep()
// {
// 	BeginDataAccess();
		
// 	// check incoming packets
// 	if (GotoFirstSourceWithData())
// 	{
// 		do
// 		{
// 			RTPPacket *pack;
// 			RTPSourceData *srcdat;
			
// 			srcdat = GetCurrentSourceInfo();
			
// 			while ((pack = GetNextPacket()) != NULL)
// 			{
// 				ProcessRTPPacket(*srcdat,*pack);
// 				DeletePacket(pack);
// 			}
// 		} while (GotoNextSourceWithData());
// 	}
		
// 	EndDataAccess();
// }

// void MyRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
// {
// 	// You can inspect the packet and the source's info here
// 	std::cout << "Got packet " << rtppack.GetExtendedSequenceNumber() << " from SSRC " << srcdat.GetSSRC() << std::endl;
//     const sf::Int16* samples     = (sf::Int16*)rtppack.GetPayloadData();
//     std::size_t      sampleCount = (rtppack.GetPayloadLength()) / sizeof(sf::Int16);
//     cout << "sample : " << *samples << endl;
//     stream.setStream(samples, sampleCount);
//     // load an audio buffer from a sound file

// }

// //
// // The main routine
// // 

// int main(void)
// {
// #ifdef RTP_SOCKETTYPE_WINSOCK
// 	WSADATA dat;
// 	WSAStartup(MAKEWORD(2,2),&dat);
// #endif // RTP_SOCKETTYPE_WINSOCK
	
// 	MyRTPSession sess;
// 	uint16_t portbase;
// 	std::string ipstr;
// 	int status,num;

//         // First, we'll ask for the necessary information
		
// 	std::cout << "Enter local portbase:" << std::endl;
// 	std::cin >> portbase;
// 	std::cout << std::endl;
//     sess.stream.start(portbase);
	
// 	std::cout << std::endl;
// 	std::cout << "Number of seconds you wish to wait:" << std::endl;
// 	std::cin >> num;
	
// 	// Now, we'll create a RTP session, set the destination
// 	// and poll for incoming data.
	
// 	RTPUDPv4TransmissionParams transparams;
// 	RTPSessionParams sessparams;
	
// 	// IMPORTANT: The local timestamp unit MUST be set, otherwise
// 	//            RTCP Sender Report info will be calculated wrong
// 	// In this case, we'll be just use 8000 samples per second.
// 	sessparams.SetOwnTimestampUnit(1.0/8000.0);		
	
// 	transparams.SetPortbase(portbase);
// 	status = sess.Create(sessparams,&transparams);	
// 	checkerror(status);

//     // // Loop until the sound playback is finished
//     // while (audioStream.getStatus() != sf::SoundStream::Stopped)
//     // {
//     //     // Leave some CPU time for other threads
//     //     sf::sleep(sf::milliseconds(100));
//     // }

	
// 	// Wait a number of seconds
// 	RTPTime::Wait(RTPTime(num,0));

//     sess.stream.setStreamingFinished();
// 	sess.BYEDestroy(RTPTime(10,0),0,0);

// #ifdef RTP_SOCKETTYPE_WINSOCK
// 	WSACleanup();
// #endif // RTP_SOCKETTYPE_WINSOCK
// 	return 0;
// }

// #else

// int main(void)
// {
// 	std::cerr << "Thread support is required for this example" << std::endl;
// 	return 0;
// }

// #endif // RTP_SUPPORT_THREAD
