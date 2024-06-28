#include "AudioRecordWindows.h"

#include <fstream>
#include <unordered_map>
#ifdef PLATFORM_WINDOWS
#pragma comment(lib,"winmm.lib")
namespace AudioRecordSpace
{
    
    DWORD WaveAPI_Callback(        // WaveAPI回调函数
        HWAVEIN hWaveIn,                            //  输入设备
        UINT uMsg,                                  //  消息
        DWORD_PTR dwInstance,                       //  保留
        DWORD_PTR dwParam1,                         //  刚填充好的缓冲区指针
        DWORD_PTR dwParam2                          //  保留
    )
    {
        return reinterpret_cast<AudioRecordWindows*>(dwInstance)->WaveAPI_Callback(
            hWaveIn, uMsg, dwInstance, dwParam1, dwParam2
        );
    }
    
    AudioRecordWindows::AudioRecordWindows():
        m_AudioDevice(nullptr),
        m_WaveHdrBuffer{},
        m_AudioDataBlock(),
        m_bStopRecord(false),
        m_bPushData(false),
        m_Callback(nullptr),
        m_bCallback(false)
    {
        m_WavHeader =
        {
            {'R', 'I', 'F', 'F'},
            0,
            {'W', 'A', 'V', 'E'},
            {'f', 'm', 't', ' '},
            sizeof(PCMWAVEFORMAT),
            WAVE_FORMAT_PCM,
            1,
            SAMPLE_RATE,
            SAMPLE_RATE * (SAMPLE_BITS / 8),
            SAMPLE_BITS / 8,
            SAMPLE_BITS,
            {'d', 'a', 't', 'a'},
            0
        };
    }

    AudioRecordWindows::~AudioRecordWindows()
    {
    }
    bool AudioRecordWindows::OpenAudioDevice(UINT index)
    {
        WAVEFORMATEX waveFormate;
        InitWaveFormat(&waveFormate, CHANNEL_NUM, SAMPLE_RATE, SAMPLE_BITS);
        waveInOpen(&m_AudioDevice
            , index
            , &waveFormate
            , (DWORD_PTR)::AudioRecordSpace::WaveAPI_Callback
            ,reinterpret_cast<DWORD_PTR>(this)
            , CALLBACK_FUNCTION);
        return true;
    }
    void AudioRecordWindows::CloseAudioDevice()
    {
        if (m_bCallback)
        {
            m_Callback(m_AudioDataBlock, RecordStatus::CloseDevice);
        }
        waveInClose(m_AudioDevice);
    }
    void AudioRecordWindows::InitWaveFormat(LPWAVEFORMATEX WaveFormat, WORD ChannelNum, DWORD SampleRate, WORD BitsPerSample)
    {
        // 配置音频波形参数
        WaveFormat->wFormatTag = WAVE_FORMAT_PCM;
        WaveFormat->nChannels = ChannelNum;
        WaveFormat->nSamplesPerSec = SampleRate;
        WaveFormat->nAvgBytesPerSec = SampleRate * ChannelNum * BitsPerSample / 8;
        WaveFormat->nBlockAlign = ChannelNum * BitsPerSample / 8;
        WaveFormat->wBitsPerSample = BitsPerSample;
        WaveFormat->cbSize = 0;
    }
    void AudioRecordWindows::StartRecord()
    {
        m_bPushData = true;
        m_bStopRecord = false;
        for (unsigned int i = 0; i < BUFFER_NUM; ++i)
        {
            m_WaveHdrBuffer[i].lpData = new char[AUDIO_DATA_BLOCK_SIZE];
            m_WaveHdrBuffer[i].dwBufferLength = AUDIO_DATA_BLOCK_SIZE;
            m_WaveHdrBuffer[i].dwBytesRecorded = 0;
            m_WaveHdrBuffer[i].dwUser = i;
            m_WaveHdrBuffer[i].dwFlags = 0;
            m_WaveHdrBuffer[i].dwLoops = 0;
            m_WaveHdrBuffer[i].lpNext = NULL;
            m_WaveHdrBuffer[i].reserved = 0;
            // 排进缓冲区
            waveInPrepareHeader(m_AudioDevice, &m_WaveHdrBuffer[i], sizeof(WAVEHDR));
            waveInAddBuffer(m_AudioDevice, &m_WaveHdrBuffer[i], sizeof(WAVEHDR));
        }

        // 清除视频缓冲
        m_AudioData.clear();
        m_AudioData.shrink_to_fit();
        m_AudioData.resize(0);

        // 开始录音
        waveInStart(m_AudioDevice);

        if (m_bCallback)
        {
            m_Callback(m_AudioDataBlock, RecordStatus::RecordStart);
        }

    }
    void AudioRecordWindows::StopRecord()
    {
        m_bStopRecord = true;
        // 停止录音设备
        waveInStop(m_AudioDevice);
        waveInReset(m_AudioDevice);


        // 释放缓冲区
        for (unsigned int i = 0; i < BUFFER_NUM; ++i)
        {
            waveInUnprepareHeader(m_AudioDevice, &m_WaveHdrBuffer[i], sizeof(WAVEHDR));
            delete m_WaveHdrBuffer[i].lpData;
            m_WaveHdrBuffer[i].lpData = NULL;
        }
    }
    void AudioRecordWindows::ResetRecord()
    {
        m_AudioData.clear();
        m_AudioData.shrink_to_fit();
        m_bPushData = true;
        m_bStopRecord = false;
        // m_bCallback = false;
        // m_Callback = NULL;
    }
    void AudioRecordWindows::WriteWavFile(const FString& FilePath)
    {
        // 编辑并写入Wave头信息
        std::ofstream out(*FilePath, std::ios::out | std::ios::binary);
        
        m_WavHeader.data_size = AUDIO_DATA_BLOCK_SIZE * m_AudioData.size();

        m_WavHeader.size_8 = m_WavHeader.data_size + (sizeof(WavHeader) - 8);

        out.write((const char*)&m_WavHeader, sizeof(m_WavHeader));

        out.write((const char*)m_AudioData.data(),
            AUDIO_DATA_BLOCK_SIZE * m_AudioData.size());

        out.flush();

        out.close();

        // fwrite(&m_WavHeader, sizeof(m_WavHeader), 1, m_WavFileOpen);
        
        // 追加RawData
        // fwrite(m_AudioData.data(), AUDIO_DATA_BLOCK_SIZE * m_AudioData.size(), 1, m_WavFileOpen);

        // 写入结束
        // fclose(m_WavFileOpen);
    }
    void AudioRecordWindows::WritePcmFile(const char* FilePath)
    {
        std::ofstream out(FilePath, std::ios::out | std::ios::binary);

        out.write((const char*)m_AudioData.data(),
            AUDIO_DATA_BLOCK_SIZE * m_AudioData.size());

        out.flush();

        out.close();
        
        // 追加RawData
        // fwrite(m_AudioData.data(), AUDIO_DATA_BLOCK_SIZE * m_AudioData.size(), 1, m_PcmFileOpen);

        // 写入结束
        // fclose(m_PcmFileOpen);
    }
    void AudioRecordWindows::RegisterCallback(AudioRecordCallback audioCallback)
    {
        m_bCallback = true;
        m_Callback = audioCallback;
    }

    void AudioRecordWindows::GetAllAudioDevice(TArray<FString>& Devices)
    {
        int audioDeviceNum = waveInGetNumDevs();
        checkf(audioDeviceNum > 0, TEXT("Windows没有找到录音设备，请确认Windows找到了录音设备"));
        
        {
            for (int i = 0; i < audioDeviceNum; ++i)
            {
                WAVEINCAPS waveInCaps;
                MMRESULT mmResult = waveInGetDevCaps(i, &waveInCaps, sizeof(WAVEINCAPS));
                
                Devices.Add(waveInCaps.szPname);
            }
        }
    }

    DWORD AudioRecordWindows::WaveAPI_Callback(HWAVEIN hWaveIn, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
    {
        // 消息switch
        switch (uMsg)
        {
        case WIM_OPEN:      // 设备成功已打开
                    
            if (m_bCallback)
            {
                m_Callback(m_AudioDataBlock, RecordStatus::OpenDevice);
            }

            break;

        case WIM_DATA:      // 缓冲区数据填充完毕

            // 停止后会频繁发出WIM_DATA,已经将数据转移所以不必理会后继数据【后继数据在这里来看是是重复的】

            if (m_bPushData)
            {
                // 把缓冲区数据拷贝出来
                memcpy(m_AudioDataBlock.data(), ((LPWAVEHDR)dwParam1)->lpData, AUDIO_DATA_BLOCK_SIZE);

                // 没有录进去的被填充为0xcd,改成0来避免末尾出现爆音【只在结束录音时进行，不影响添加缓存效率】
                if (((LPWAVEHDR)dwParam1)->dwBytesRecorded < AUDIO_DATA_BLOCK_SIZE)
                {
                    for (size_t i = ((LPWAVEHDR)dwParam1)->dwBytesRecorded; i < AUDIO_DATA_BLOCK_SIZE; i++)
                    {
                        m_AudioDataBlock.at(i) = 0;
                    }
                }
                // 添加这一帧
                m_AudioData.push_back(m_AudioDataBlock);

                // 如果你设置了回调函数
                if (m_bCallback)
                {
                    m_Callback(m_AudioDataBlock, RecordStatus::RecordWriteData);
                }
            }


            // 如果需要停止录音则不继续添加缓存
            if (!m_bStopRecord)
            {
                waveInAddBuffer(hWaveIn, (LPWAVEHDR)dwParam1, sizeof(WAVEHDR));//添加到缓冲区
            }
            else
            {
                // 如果已经停止了录制，就不要再写入数据
                m_bPushData = false;
            }

            break;

        case WIM_CLOSE:
            // 操作成功完成
            if (m_bCallback)
            {
                m_Callback(m_AudioDataBlock, RecordStatus::RecordStop);
            }
            break;

        default:
            break;
        }
        return 0;
    }
}
#endif