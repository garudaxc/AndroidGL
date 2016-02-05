#include "thirdParty/etc1/etc1.h"
#include "MyLog.h"

GLuint CreateTexture(const uint8_t* data, uint32_t w, uint32_t h,
						uint32_t pixelsize, uint32_t stride)
{
	if (data == NULL || w == 0 || h == 0) {
		GLog.LogError("CreateTexture invalid parameter data %p w %d h %d", data, w, h);
		return 0;
	}

	uint32_t size = etc1_get_encoded_data_size(w, h);
	uint8_t* buffer = (uint8_t*)malloc(size);

	if (buffer == NULL)	{
		GLog.LogError("malloc failed size %u", size);
		return 0;
	}

	if (etc1_encode_image(data, w, h, pixelsize, stride, buffer) != 0) {
		GLog.LogError("etc1_encode_image failed!");
		free(buffer);
		return 0;
	}




	return 0;
}