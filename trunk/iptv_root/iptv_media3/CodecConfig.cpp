#include "compat.h"
#include <sstream>
#include "CodecConfig.h"
#include "global_error.h"
#include "semaphore.h"
#include "log.h"
#include "media_utilities.h"

unsigned CodecConfig::uInstances        = 0;
bool     CodecConfig::luaCfgInitialized = false;

static   DsCScriptEngine LuaCfg;
static   _SEMAPHORE Semaph;

CodecConfig::CodecConfig()
{
    Semaph.Wait();
    
    uInstances++;

    if (!luaCfgInitialized)
    {
        if (LuaCfg.Execute("mc.cfg") == DsS_OK)
            luaCfgInitialized = true;
    }

    Semaph.Signal();
}

CodecConfig::~CodecConfig()
{
    Semaph.Wait();

    uInstances--;

    Semaph.Signal();
}


 unsigned CodecConfig::GetTableParam(std::string _transmission, 
                                     std::string _network, 
                                     std::string _bitrate,
                                     std::string _type,
                                     std::string _codec, 
                                     DsCScriptValue & _tableParam)
{
    unsigned ret = RET_OK;

    Semaph.Wait();

    if (luaCfgInitialized)
    {
        std::ostringstream oss;
        
        if (_type == "audio")
        {   
            // avconf_stream_voice_<codec>
            oss << "avconf_stream_voice_"<< _codec;

           _tableParam = LuaCfg.GetGlobalValue(oss.str().c_str());
        }
        else  
        {
            // profile_av_<transmission>_<network>_<codec>_<bitrate>
            oss <<"profile_av_"<<_transmission<<"_"<<_network<<"_"<<_codec<< "_"<< _bitrate;
        
            DsCScriptValue videoParam = LuaCfg.GetGlobalValue(oss.str().c_str());

            const int TYPE_VIDEO = 1;
            _tableParam = videoParam.GetTableValue("encoder").GetTableValue("streams").GetTableValue(TYPE_VIDEO);
        }
    }
    else
        ret = RET_ERROR;

    Semaph.Signal();

    return ret;
}

unsigned CodecConfig::GetTableParam(unsigned char _type, DsCScriptValue & _tableParam)
{
    unsigned ret = RET_OK;

    Semaph.Wait();

    if (luaCfgInitialized)
        _tableParam = LuaCfg.GetGlobalValue("profile_av_streams_types").GetTableValue(_type);
    else
        ret = RET_ERROR;

    Semaph.Signal();

    return ret;
}

unsigned CodecConfig::GetTableParam(std::string _index, DsCScriptValue & _tableParam)
{
    unsigned ret = RET_OK;

    Semaph.Wait();

    if (luaCfgInitialized)
        _tableParam = LuaCfg.GetGlobalValue(_index.c_str());
    else
        ret = RET_ERROR;

    Semaph.Signal();

    return ret;
}

unsigned MediaCodecsList::GetClientCodecs(DsCScriptValue & _tableParam)
{
    CHECK_ERROR(GetTableParam(MEDIA_CONF_PANEL, _tableParam), RET_OK)

    return RET_OK;
}

unsigned MediaCodecsList::GetAudioCodecs(std::deque<std::string> & _audioCodecs)
{
    DsCScriptValue tableParam;
    CHECK_ERROR(GetClientCodecs(tableParam), RET_OK)

    unsigned ret = RET_OK;

    const int TYPE_AUDIO = 0;

    std::string audioStr = tableParam.GetTableValue(DsINT32(TYPE_AUDIO)).GetStr().GetStr();
    
    if (audioStr.size())
        ret = SepareStrings(audioStr, _audioCodecs, m_ListToken);
    else
        ret = RET_ERROR;

    return ret;
}

unsigned MediaCodecsList::GetVideoCodecs(std::deque<std::string> & _videoCodecs, std::deque<std::string> & _videoBitrates)
{
    DsCScriptValue tableParam;
    CHECK_ERROR(GetClientCodecs(tableParam), RET_OK)

    unsigned ret = RET_OK;

    const int TYPE_VIDEO = 1;

    std::string videoStr        = tableParam.GetTableValue(DsINT32(TYPE_VIDEO)).GetTableValue("codecs").GetStr().GetStr();
    std::string videoBitrateStr = tableParam.GetTableValue(DsINT32(TYPE_VIDEO)).GetTableValue("bitrates").GetStr().GetStr();

    if (videoStr.size())
        ret = SepareStrings(videoStr, _videoCodecs, m_ListToken);
    else
        ret = RET_ERROR;

    if (ret == RET_OK)
    {
        if (videoBitrateStr.size())    
            ret = SepareStrings(videoBitrateStr, _videoBitrates, m_ListToken);
        else
            ret = RET_ERROR;
    }

    return ret;
}

unsigned MediaNetworkList::GetClientNetwork(DsCScriptValue & _tableParam)
{
    CHECK_ERROR(GetTableParam(MEDIA_CONF_PANEL, _tableParam), RET_OK)
    
    return RET_OK;
}

unsigned MediaNetworkList::GetAvailableNetworks(std::deque<std::string> & _networks)
{
    DsCScriptValue tableParam;
    CHECK_ERROR(GetClientNetwork(tableParam), RET_OK)

    unsigned ret = RET_OK;

    const int TYPE_NETWORK = 3;

    std::string networkStr = tableParam.GetTableValue(DsINT32(TYPE_NETWORK)).GetStr().GetStr();
    
    if (networkStr.size())
        ret = SepareStrings(networkStr, _networks, m_ListToken);
    else
        ret = RET_ERROR;

    return ret;
}

