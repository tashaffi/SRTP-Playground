
#include<fstream>
#include<iostream>
#include <SFML/Audio.hpp>
#include "audio.h"
using namespace std;

void loadWaveFile(string fname, WavData* ret) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(fname)){
        //TODO
        //catch error and show proper error message
        return;
    }

    ret->setData(buffer.getSamples());
    ret->setSampleCount(buffer.getSampleCount());
    ret->setChannelCount(buffer.getChannelCount());
    ret->setSampleRate(buffer.getSampleRate());
}

//Example, how to call and use this function and test playback
// int main(){
//     WavData song;
//     string audioFilePath = "/Users/tashaffi/Documents/office work data/SRTP project/src/audio.wav";
//     loadWaveFile(audioFilePath, &song);
    
//     sf::SoundBuffer buffer;
//     buffer.loadFromSamples(song.getData(), song.getSampleCount(), song.getChannelCount(), song.getSampleRate());

//     sf::Sound sound(buffer);
//     sound.play();
//     while(sound.getStatus() == sf::Sound::Playing)
//     {
//     }
//     return 0;
// }
