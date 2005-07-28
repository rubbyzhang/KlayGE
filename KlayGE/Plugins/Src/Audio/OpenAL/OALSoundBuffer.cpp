// OALSoundBuffer.cpp
// KlayGE OpenAL声音缓冲区类 实现文件
// Ver 2.0.0
// 版权所有(C) 龚敏敏, 2003
// Homepage: http://www.enginedev.com
//
// 2.0.0
// 初次建立 (2003.7.7)
//
// 修改记录
/////////////////////////////////////////////////////////////////////////////////

#include <KlayGE/KlayGE.hpp>
#include <KlayGE/AudioDataSource.hpp>

#include <boost/assert.hpp>
#pragma warning(disable: 4127 4512)
#include <boost/random.hpp>
#include <boost/bind.hpp>

#include <KlayGE/OpenAL/OALAudio.hpp>

namespace
{
	// 检查一个音频缓冲区是否空闲
	/////////////////////////////////////////////////////////////////////////////////
	bool IsSourceFree(ALuint source)
	{
		ALint value;
		alGetSourcei(source, AL_SOURCE_STATE, &value);

		return (AL_PLAYING != (value & AL_PLAYING));
	}
}

namespace KlayGE
{
	// 构造函数
	/////////////////////////////////////////////////////////////////////////////////
	OALSoundBuffer::OALSoundBuffer(AudioDataSourcePtr const & dataSource, uint32_t numSource, float volume)
						: SoundBuffer(dataSource),
							sources_(numSource)
	{
		alGenBuffers(1, &buffer_);

		// 用整个waveFile填充缓冲区
		std::vector<uint8_t> data(dataSource_->Size());
		dataSource_->Read(&data[0], data.size());

		alBufferData(buffer_, Convert(format_), &data[0], static_cast<ALsizei>(data.size()), freq_);

		alGenSources(static_cast<ALsizei>(sources_.size()), &sources_[0]);

		for (SourcesIter iter = sources_.begin(); iter != sources_.end(); ++ iter)
		{
			alSourcef(*iter, AL_PITCH, 1);
			alSourcef(*iter, AL_GAIN, volume);
			alSourcei(*iter, AL_BUFFER, buffer_);
		}

		this->Position(Vector3(0, 0, 0.1f));
		this->Velocity(Vector3(0, 0, 0));
		this->Direction(Vector3(0, 0, 0));

		this->Reset();
	}

	// 析构函数
	/////////////////////////////////////////////////////////////////////////////////
	OALSoundBuffer::~OALSoundBuffer()
	{
		this->Stop();

		alDeleteBuffers(1, &buffer_);
		alDeleteSources(static_cast<ALsizei>(sources_.size()), &sources_[0]);
	}

	// 返回空闲的缓冲区
	/////////////////////////////////////////////////////////////////////////////////
	OALSoundBuffer::SourcesIter OALSoundBuffer::FreeSource()
	{
		BOOST_ASSERT(!sources_.empty());

		SourcesIter iter(std::find_if(sources_.begin(), sources_.end(), IsSourceFree));

		if (iter == sources_.end())
		{
			iter = sources_.begin();
			std::advance(iter,
				boost::variate_generator<boost::lagged_fibonacci607, boost::uniform_int<> >(boost::lagged_fibonacci607(),
					boost::uniform_int<>(0, static_cast<int>(sources_.size())))());
		}

		return iter;
	}

	// 播放音源
	/////////////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::Play(bool loop)
	{
		ALuint& source(*this->FreeSource());

		alSourcefv(source, AL_POSITION, &pos_[0]);
		alSourcefv(source, AL_VELOCITY, &vel_[0]);
		alSourcefv(source, AL_DIRECTION, &dir_[0]);
		alSourcei(source, AL_LOOPING, loop);

		alSourcePlay(source);
	}

	// 停止播放
	/////////////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::Stop()
	{
		alSourceStopv(static_cast<ALsizei>(sources_.size()), &sources_[0]);
	}

	// 声音缓冲区复位
	/////////////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::DoReset()
	{
		alSourceRewindv(static_cast<ALsizei>(sources_.size()), &sources_[0]);
	}

	// 检查缓冲区是否在播放
	/////////////////////////////////////////////////////////////////////////////////
	bool OALSoundBuffer::IsPlaying() const
	{
		return (std::find_if(sources_.begin(), sources_.end(),
			std::not1(std::ptr_fun(IsSourceFree))) != sources_.end());
	}

	// 设置音量
	/////////////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::Volume(float vol)
	{
		std::for_each(sources_.begin(), sources_.end(),
			boost::bind(alSourcef, _1, AL_GAIN, vol));
	}

	// 获取声源位置
	/////////////////////////////////////////////////////////////////////////////////
	Vector3 OALSoundBuffer::Position() const
	{
		return ALVecToVec(pos_);
	}

	// 设置声源位置
	/////////////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::Position(Vector3 const & v)
	{
		pos_ = VecToALVec(v);
	}

	// 获取声源速度
	/////////////////////////////////////////////////////////////////////////////////
	Vector3 OALSoundBuffer::Velocity() const
	{
		return ALVecToVec(vel_);
	}

	// 设置声源速度
	/////////////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::Velocity(Vector3 const & v)
	{
		vel_ = VecToALVec(v);
	}

	// 获取声源方向
	/////////////////////////////////////////////////////////////////////////////////
	Vector3 OALSoundBuffer::Direction() const
	{
		return ALVecToVec(dir_);
	}

	// 设置声源方向
	/////////////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::Direction(Vector3 const & v)
	{
		dir_ = VecToALVec(v);
	}
}
