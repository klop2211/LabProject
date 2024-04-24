#pragma once

class CVoiceCallback : public IXAudio2VoiceCallback
{
	bool is_play_ = false;

public:
	CVoiceCallback() {}
	~CVoiceCallback() {}

	//Called when the voice has just finished playing a contiguous audio stream.
	void OnStreamEnd() {}

	//Unused methods are stubs
	void OnVoiceProcessingPassEnd() { }
	void OnVoiceProcessingPassStart(UINT32 SamplesRequired) { }
	void OnBufferEnd(void* pBufferContext) { is_play_ = false; }
	void OnBufferStart(void* pBufferContext) { is_play_ = true; }
	void OnLoopEnd(void* pBufferContext) {    }
	void OnVoiceError(void* pBufferContext, HRESULT Error) { }

	bool IsPlay() { return is_play_; }
};

class CAudioTrack
{
	static CVoiceCallback voice_callback_;

	WAVEFORMATEXTENSIBLE wave_format_;

	IXAudio2SourceVoice* ix_source_voice_;


	XAUDIO2_BUFFER buffer_;


public:
	CAudioTrack() {}
	CAudioTrack(IXAudio2* ix_audio, const std::string& file_name);
	~CAudioTrack();

	void Play();
	void Pause() { ix_source_voice_->Stop(); }
	void Stop() { ix_source_voice_->Stop(); ix_source_voice_->FlushSourceBuffers(); }
};

class CAudioManager
{
	static const std::string file_path_;

	IXAudio2* ix_audio_;
	IXAudio2MasteringVoice* ix_mastering_voice_;

	std::unordered_map<std::string, CAudioTrack> audio_tracks_;

public:
	CAudioManager();
	~CAudioManager();

	bool IsTrack(const std::string& track_name) 
	{ 
		if (audio_tracks_.empty())
			return false;
		return audio_tracks_.count(track_name); 
	}

	void AddTrack(const std::string& track_name);

	void PlayTrack(const std::string& track_name);

};

