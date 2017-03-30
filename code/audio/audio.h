#pragma once



class AudioSystem
{
public:
	bool		Init(const char* pathName);
	
	void		Play();
	void		Pause();
	void		Stop();

	uint32_t	GetPosition();
	uint32_t	GetDuration();
};


extern AudioSystem		GAudioSystem;