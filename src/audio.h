#include <string>
#include <SFML/Config.hpp>

class WavData {
    public:
        WavData(const sf::Int16* d, sf::Uint64 sc, unsigned int cc, unsigned int sr){
            data = d;
            sampleCount = sc;
            channelCount = cc;
            sampleRate = sr;
        }

        WavData(){}

        const sf::Int16* getData() { return data; }
        sf::Uint64 getSampleCount() { return sampleCount; }
        unsigned int getChannelCount() { return channelCount; }
        unsigned int getSampleRate() { return sampleRate; }

        void setData(const sf::Int16* d) { data = d; }
        void setSampleCount(sf::Uint64 sc) { sampleCount = sc; }
        void setChannelCount(unsigned int cc) { channelCount = cc; }
        void setSampleRate(unsigned int sr) { sampleRate = sr; }

    private:
        const sf::Int16* data;
        sf::Uint64 sampleCount;
        unsigned int channelCount;
        unsigned int sampleRate;

};

void loadWaveFile(std::string fname, WavData* ret);