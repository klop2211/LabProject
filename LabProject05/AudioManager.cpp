#include "stdafx.h"
#include "AudioManager.h"

const std::string CAudioManager::file_path_ = "../Resource/Sound/";

CAudioManager::CAudioManager()
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	XAudio2Create(&ix_audio_);

	ix_audio_->CreateMasteringVoice(&ix_mastering_voice_);
}

CAudioManager::~CAudioManager()
{
	ix_mastering_voice_->DestroyVoice();
	ix_audio_->Release();
}

void CAudioManager::AddTrack(const std::string& track_name)
{
	if (IsTrack(track_name)) return;

	std::string file_name = file_path_ + track_name + ".wav";

	audio_tracks_.emplace(track_name, ix_audio_, file_name);

}

void CAudioManager::PlayTrack(const std::string& track_name)
{
	audio_tracks_[track_name].Play();
}

CAudioTrack::CAudioTrack(IXAudio2* ix_audio, const std::string& file_name)
{
	auto data = ::LoadWavFile(file_name, wave_format_);

	ix_audio->CreateSourceVoice(&ix_source_voice_, &wave_format_);

	buffer_.AudioBytes = data.size();
	buffer_.pAudioData = data.data();
	buffer_.Flags = XAUDIO2_END_OF_STREAM;

	ix_source_voice_->SubmitSourceBuffer(&buffer_);
}

CAudioTrack::~CAudioTrack()
{
	ix_source_voice_->DestroyVoice();
}

void CAudioTrack::Play()
{
	XAUDIO2_VOICE_STATE state;
	ix_source_voice_->GetState(&state);

	// Àç»ı Áß
	if (state.BuffersQueued > 0)
		return;

	ix_source_voice_->Start(0);
}