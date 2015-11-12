#include "Platfrom.h"

namespace FancyTech
{

	class Texture
	{
	public:
		Texture();
		~Texture();

		bool	Load(const char* fileName);

		void	Bind();

	private:

		uint32_t	target_;
		GLuint		texId_;
	};
}
