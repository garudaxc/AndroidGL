#include "Texture.h"
#include "MyLog.h"
#include "glUtil.h"
#include "thirdParty/stb/stb_image.h"
#include "FileSystem.h"
#include <vector>

using namespace std;

namespace FancyTech
{

	/*

	KTX Container Format

	KTX is a format for storing textures for OpenGL and OpenGL ES applications.
	It is distinguished by the simplicity of the loader required to instantiate
	a GL texture object from the file contents.

	Byte[12] identifier
	UInt32 endianness
	UInt32 glType
	UInt32 glTypeSize
	UInt32 glFormat
	Uint32 glInternalFormat
	Uint32 glBaseInternalFormat
	UInt32 pixelWidth
	UInt32 pixelHeight
	UInt32 pixelDepth
	UInt32 numberOfArrayElements
	UInt32 numberOfFaces
	UInt32 numberOfMipmapLevels
	UInt32 bytesOfKeyValueData

	for each keyValuePair that fits in bytesOfKeyValueData
	UInt32   keyAndValueByteSize
	Byte     keyAndValue[keyAndValueByteSize]
	Byte     valuePadding[3 - ((keyAndValueByteSize + 3) % 4)]
	end

	for each mipmap_level in numberOfMipmapLevels*
	UInt32 imageSize;
	for each array_element in numberOfArrayElements*
	for each face in numberOfFaces
	for each z_slice in pixelDepth*
	for each row or row_of_blocks in pixelHeight*
	for each pixel or block_of_pixels in pixelWidth
	Byte data[format-specific-number-of-bytes]**
	end
	end
	end
	Byte cubePadding[0-3]
	end
	end
	Byte mipPadding[3 - ((imageSize + 3) % 4)]
	end

	*/

	//
	//
	//#pragma pack(1)
	//struct OVR_KTX_HEADER
	//{
	//	ubyte_t		identifier[12];
	//	uint32_t	endianness;
	//	uint32_t	glType;
	//	uint32_t	glTypeSize;
	//	uint32_t	glFormat;
	//	uint32_t	glInternalFormat;
	//	uint32_t	glBaseInternalFormat;
	//	uint32_t	pixelWidth;
	//	uint32_t	pixelHeight;
	//	uint32_t	pixelDepth;
	//	uint32_t	numberOfArrayElements;
	//	uint32_t	numberOfFaces;
	//	uint32_t	numberOfMipmapLevels;
	//	uint32_t	bytesOfKeyValueData;
	//};
	//#pragma pack()
	//
	//
	//
	//
	//
	//bool LoadTextureKTX(const char * fileName, const unsigned char * buffer, const int bufferLength,
	//	bool useSrgbFormat, bool noMipMaps, int & width, int & height)
	//{
	//
	//	width = 0;
	//	height = 0;
	//
	//	if (bufferLength < (int)(sizeof(OVR_KTX_HEADER))){
	//		GLog.LogError("%s: Invalid KTX file", fileName);
	//		return false;
	//	}
	//
	//	const ubyte_t fileIdentifier[12] = {
	//		0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
	//	};
	//
	//	const OVR_KTX_HEADER & header = *(OVR_KTX_HEADER *)buffer;
	//	if (memcmp(header.identifier, fileIdentifier, sizeof(fileIdentifier)) != 0){
	//		GLog.LogError("%s: Invalid KTX file", fileName);
	//		return false;
	//	}
	//	// only support little endian
	//	if (header.endianness != 0x04030201){
	//		GLog.LogError("%s: KTX file has wrong endianess", fileName);
	//		return false;
	//	}
	//	// only support compressed or unsigned byte
	//	if (header.glType != 0 && header.glType != GL_UNSIGNED_BYTE){
	//		GLog.LogError("%s: KTX file has unsupported glType %d", fileName, header.glType);
	//		return false;
	//	}
	//	// no support for texture arrays
	//	if (header.numberOfArrayElements != 0){
	//		GLog.LogError("%s: KTX file has unsupported number of array elements %d", fileName, header.numberOfArrayElements);
	//		return false;
	//	}
	//	// derive the texture format from the GL format
	//	int format = 0;
	//
	//	const char* codetype = GLCompressedTexName(header.glInternalFormat);
	//
	//
	//	//if (!GlFormatToTextureFormat(format, header.glFormat, header.glInternalFormat))	{
	//	//	GLog.LogError("%s: KTX file has unsupported glFormat %d, glInternalFormat %d", fileName, header.glFormat, header.glInternalFormat);
	//	//	return false;
	//	//}
	//	// skip the key value data
	//	const uintptr_t startTex = sizeof(OVR_KTX_HEADER)+header.bytesOfKeyValueData;
	//	if ((startTex < sizeof(OVR_KTX_HEADER)) || (startTex >= static_cast< size_t >(bufferLength))){
	//		GLog.LogError("%s: Invalid KTX header sizes", fileName);
	//		return false;
	//	}
	//
	//	width = header.pixelWidth;
	//	height = header.pixelHeight;
	//
	//	/*const UInt32 mipCount = (noMipMaps) ? 1 : OVR::Alg::Max(1u, header.numberOfMipmapLevels);
	//
	//	if (header.numberOfFaces == 1)
	//	{
	//	return CreateGlTexture(fileName, format, width, height, buffer + startTex, bufferLength - startTex, mipCount, useSrgbFormat, true);
	//	}
	//	else if (header.numberOfFaces == 6)
	//	{
	//	return CreateGlCubeTexture(fileName, format, width, height, buffer + startTex, bufferLength - startTex, mipCount, useSrgbFormat, true);
	//	}
	//	else
	//	{
	//	GLog.LogError("%s: KTX file has unsupported number of faces %d", fileName, header.numberOfFaces);
	//	}*/
	//
	//	width = 0;
	//	height = 0;
	//	return false;
	//}


	Texture::Texture() :target_(0), texId_(0)
	{
	}

	Texture::~Texture()
	{
		//if (texId_ != 0){
		//	glDeleteTextures(1, &texId_);
		//	texId_ = 0;
		//}
	}

	bool LoadFileToMemory(const char* fileName, ubyte_t*& buffer, uint32_t& size)
	{
		FILE* pf = fopen(fileName, "rb");
		if (pf == NULL){
			GLog.LogError("Model::Load failed! %s", fileName);
			return false;
		}

		fseek(pf, 0, SEEK_END);
		size = ftell(pf);

		buffer = new ubyte_t[size];
		fseek(pf, 0, SEEK_SET);
		int r = fread(buffer, 1, size, pf);

		fclose(pf);

		return true;
	}

	bool StrPostfix(const char* str, const char* fix)
	{
		const char* sub = strstr(str, fix);
		if (sub != NULL && (sub - str) == strlen(str) - strlen(fix)) {
			return true;
		}

		return false;
	}


	bool Texture::Load(const char* fileName)
	{
		target_ = GL_TEXTURE_2D;
		int w, h, comp;

		//ubyte_t* buffer = NULL;
		//uint32_t size = 0;
		//LoadFileToMemory(fileName, buffer, size);

		//LoadTextureKTX(fileName, buffer, size, false, true, w, h);
		//return true;

		File* file = GFileSys->OpenFile(fileName);
		if (file == NULL) {
			return false;
		}

		vector<ubyte_t> buffer(file->Size());
		file->Read(&buffer[0], file->Size());
		file->Close();

		if (StrPostfix(fileName, ".pkm")) {
			GLog.LogInfo("load pkm texture file!");

			texId_ = LoadPKM(&buffer[0], buffer.size());
			if (texId_ != 0) {
				return true;
			}
		}

		//stbi_uc * t = stbi_load(fileName, &w, &h, &comp, 0);
		stbi_uc* t = stbi_load_from_memory(&buffer[0], buffer.size(), &w, &h, &comp, 0);
		if (t == NULL){
			GLog.LogError("Load texture %s failed! : %s", fileName, stbi_failure_reason());
			return false;
		}
		GLog.LogInfo("texture %s info, w %d h %d component %d", fileName, w, h, comp);

		if (comp < 3){
			GLog.LogError("texture %s format not support!", fileName);
			return false;
		}

		int format = 0;
		if (comp == 3){
			format = GL_RGB;
			format_ = RGB8;
		}
		else if (comp == 4){
			format = GL_RGBA;
			format_ = RGBA8;
		}

		texId_ = CreateTextureETC1(t, w, h, comp, comp * w);
		if (texId_ != 0) {
			stbi_image_free(t);
			return true;
		}

		glGenTextures(1, &texId_);
		glBindTexture(target_, texId_);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(target_, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, t);
		checkGlError("glTexImage2D");
		glGenerateMipmap(target_);

		// Draw left quad with repeat wrap mode
		glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(target_, 0);
		stbi_image_free(t);

		return true;
	}


	void Texture::Create(int width, int height, TEXTURE_FORMAT fmt)
	{
		target_ = GL_TEXTURE_2D;
		int format = 0;
		if (fmt == RGB8){
			format = GL_RGB;
		} else if (fmt == RGBA8){
			format = GL_RGBA;
		} else {
			GLog.LogError("unsupport texture format %d", fmt);
			return;
		}

		format_ = fmt;

		glGenTextures(1, &texId_);
		glBindTexture(target_, texId_);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	

		glTexImage2D(target_, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
		
		checkGlError("glTexImage2D");
		//glGenerateMipmap(GL_TEXTURE_2D);

		// Draw left quad with repeat wrap mode
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(target_, 0);
	}

	void Texture::Bind()
	{
		if (texId_ != 0){
			glBindTexture(target_, texId_);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	FrameBuffer::FrameBuffer() :fboId_(0)
	{
	}

	FrameBuffer::~FrameBuffer()
	{
		if (fboId_ != 0) {
			glDeleteRenderbuffers(1, &fboId_);
			fboId_ = 0;
		}
	}

	void FrameBuffer::Create(int width, int height, TEXTURE_FORMAT fmt, int depth)
	{
		tex_.Create(width, height, fmt);

		glGenFramebuffers(1, &fboId_);

		//glGenRenderbuffers(1, &depthRenderbuffer);
		//glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, fboId_);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_.target_, tex_.texId_, 0);

		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

		checkGlError("glFramebufferTexture2D");
		
		GLenum state = state = glCheckFramebufferStatus(GL_FRAMEBUFFER);		
		if (state != GL_FRAMEBUFFER_COMPLETE) {
			GLog.LogError("frame buffer not complete! code = %d", state);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::Bind()
	{
		if (fboId_ != 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, fboId_);
		}
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}