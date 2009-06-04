#ifndef CODEC_CONFIG_H
#define CODEC_CONFIG_H

#include "dscscript.h"
#include <string>
#include <deque>

#define MEDIA_CONF_PANEL "profile_av_media_conf_panel"

class _SEMAPHORE;
class CodecConfig
{
private:

    static unsigned uInstances;
    static bool     luaCfgInitialized;

protected:

    CodecConfig();
    virtual ~CodecConfig()= 0;

    unsigned GetTableParam(std::string _transmission, 
                           std::string _network, 
                           std::string _bitrate,
                           std::string _type,
                           std::string _codec, 
                           DsCScriptValue & _tableParam);

    unsigned GetTableParam(unsigned char _type, DsCScriptValue & _tableParam);
    unsigned GetTableParam(std::string _index, DsCScriptValue & _tableParam);

    bool luaCfgOk() {return luaCfgInitialized; }
};


class MediaList
{
protected:

    unsigned char m_ListToken;

    MediaList()
    {
        m_ListToken = ' ';
    }
};

class MediaCodecsList : MediaList, private CodecConfig
{
private:

    unsigned GetClientCodecs(DsCScriptValue & _tableParam);

public:

    unsigned GetAudioCodecs(std::deque<std::string> & _audioCodecs);
    unsigned GetVideoCodecs(std::deque<std::string> & _videoCodecs, std::deque<std::string> & _bitrates);
};

class MediaNetworkList : MediaList, private CodecConfig
{
private:

    unsigned GetClientNetwork(DsCScriptValue & _tableParam);

public:

    unsigned GetAvailableNetworks(std::deque<std::string> & _networks);
};



#endif
