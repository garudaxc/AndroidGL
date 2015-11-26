#include "Platfrom.h"
#include "FileSystem.h"
#include "MyLog.h"
#include "zip/unzip.h"
#include <vector>
#include <dirent.h>

namespace FancyTech
{

	class FileLocal : public File
	{
	public:
		FileLocal(FILE* pf);
		~FileLocal();

		virtual size_t	Size() const;
		virtual bool	Read(void* data, int size);
		virtual void	Seek(size_t pos);
		virtual void	Close();

	private:
		FILE*	pf_;
		size_t	size_;
	};

	FileLocal::FileLocal(FILE* pf) : pf_(pf)
	{
		fseek(pf, 0, SEEK_END);
		size_ = ftell(pf);
		fseek(pf, 0, SEEK_SET);
	}

	FileLocal::~FileLocal()
	{
		if (pf_ != NULL){
			fclose(pf_);
			pf_ = NULL;
		}
	}

	size_t	FileLocal::Size() const
	{
		return size_;
	}

	bool FileLocal::Read(void* data, int size)
	{
		if (pf_ == NULL){
			return false;
		}

		if (fread(data, size, 1, pf_) != 1){
			return false;
		}

		return true;
	}

	void FileLocal::Seek(size_t pos)
	{
		fseek(pf_, pos, SEEK_SET);
	}

	void FileLocal::Close()
	{
		delete this;
	}

	

	class FileInMemory : public File
	{
	public:
		FileInMemory(size_t size);
		~FileInMemory();

		virtual size_t	Size() const
		{
			return size_;
		}

		virtual bool	Read(void* data, int size);
		virtual void	Seek(size_t pos);
		virtual void	Close();

		void*		GetBuffer()
		{
			return buffer_;
		}

	private:
		void*	buffer_;
		size_t	size_;
		size_t	currPos_;
	};

	FileInMemory::FileInMemory(size_t size) :size_(size), currPos_(0)
	{
		buffer_ = malloc(size);
	}

	FileInMemory::~FileInMemory()
	{
		if (buffer_ != NULL) {
			free(buffer_);
			buffer_ = NULL;
		}
	}

	bool FileInMemory::Read(void* data, int size)
	{
		if (currPos_ >= size_) {
			return false;
		}

		if (currPos_ + size > size_) {
			memcpy(data, (char*)buffer_ + currPos_, size_ - currPos_);
			currPos_ = size_;
			return false;
		}
		else {
			memcpy(data, (char*)buffer_ + currPos_, size);
			currPos_ += size;
			return true;
		}
	}
	void FileInMemory::Seek(size_t pos)
	{
		if (pos < size_) {
			currPos_ = pos;
		}
	}

	void FileInMemory::Close()
	{
		delete this;
	}



	class FileSystemLocal : public FileSystem
	{
	public:
		FileSystemLocal();
		~FileSystemLocal();

		virtual void SetRootPath(const string& path, bool addPak);

		virtual bool AddPakFile(const string& pathname);

		virtual File* OpenFile(const string& pathname);

		virtual bool FileExist(const string& pathname);

	private:
		void	NormalizeName(string& fullname, string& name, const string& _rawPath);

		string	rootPath_;
		vector<unzFile>	paks_;
	};

	FileSystemLocal::FileSystemLocal()
	{
	}

	FileSystemLocal::~FileSystemLocal()
	{
		for (auto it = paks_.begin(); it != paks_.end(); ++it) {
			if (unzClose(*it) != UNZ_OK) {
				GLog.LogError("close zip file failed, may files not closed");
			}
		}
	}

	void FileSystemLocal::SetRootPath(const string& path, bool addPak)
	{
		rootPath_ = path;
		for (size_t i = 0; i < rootPath_.size(); i++) {
			if (rootPath_[i] == '\\') {
				rootPath_[i] = '/';
			}
		}

		if (!rootPath_.empty() && rootPath_[rootPath_.size() - 1] != '/') {
			rootPath_ += '/';
		}

		if (addPak) {
			DIR* dir = opendir(path.c_str());
			if (dir == NULL) {
				GLog.LogError("FileSystem open dir failed! %s", path.c_str());
				return;
			}

			while (true) {
				dirent* entry = readdir(dir);
				if (entry == NULL) {
					break;
				}

				if (strstr(entry->d_name, ".zip") != NULL ||
					strstr(entry->d_name, ".pak") != NULL) {
					AddPakFile(entry->d_name);
				}
			}

			closedir(dir);
		}
	}

	bool FileSystemLocal::AddPakFile(const string& pathname)
	{
		unzFile pakfile = unzOpen(pathname.c_str());

		if (pakfile == NULL) {
			string name = rootPath_ + pathname;
			pakfile = unzOpen(name.c_str());
		}

		if (pakfile == NULL) {
			GLog.LogError("Open pak file %s failed!", pathname.c_str());
			return false;
		}

		paks_.push_back(pakfile);
		GLog.LogInfo("FileSystem add pak %s", pathname.c_str());

		return true;
	}

	void FileSystemLocal::NormalizeName(string& fullname, string& name, const string& _rawPath)
	{
		string rawPath = _rawPath;
		for (size_t i = 0; i < rawPath.size(); i++) {
			if (rawPath[i] == '\\') {
				rawPath[i] = '/';
			}
		}

		if (rawPath.find(rootPath_) == 0) {
			fullname = rawPath;
			name = fullname.substr(rootPath_.length());
		}
		else {
			fullname = rootPath_ + rawPath;
			name = rawPath;
		}
	}

	File* FileSystemLocal::OpenFile(const string& pathname)
	{
		string name, fullname;
		NormalizeName(fullname, name, pathname);

		FILE* pf = fopen(fullname.c_str(), "rb");
		if (pf != NULL){
			File* file = new FileLocal(pf);
			return file;
		}

		// search pak
		unzFile pakFile = NULL;
		for (auto it : paks_) {
			if (unzLocateFile(it, name.c_str(), 0) == UNZ_OK) {
				pakFile = it;
				break;
			}
		}

		if (pakFile != NULL) {
			unz_file_info fileInfo;
			char filename[256];
			if (unzGetCurrentFileInfo(pakFile, &fileInfo,
				filename, sizeof(filename), NULL, 0, NULL, 0) != UNZ_OK) {
				GLog.LogError("get %s file info in pak error!", pathname.c_str());
				return NULL;
			}

			if (unzOpenCurrentFile(pakFile) != UNZ_OK) {
				GLog.LogError("open file %s in pak error!", pathname.c_str());
				return NULL;
			}

			FileInMemory* file = new FileInMemory(fileInfo.uncompressed_size);
			int r = unzReadCurrentFile(pakFile, file->GetBuffer(), fileInfo.uncompressed_size);
			if (r != fileInfo.uncompressed_size) {
				GLog.LogError("read file %s in pak error!", pathname.c_str());
			}

			unzCloseCurrentFile(pakFile);

			return file;
		}

		GLog.LogError("FileSystem::OpenFile failed! %s", pathname.c_str());

		return NULL;
	}

	bool FileSystemLocal::FileExist(const string& pathname)
	{
		string name, fullname;
		NormalizeName(fullname, name, pathname);

		if (access(fullname.c_str(), 0) != -1) {
			return true;
		}

		for (auto it : paks_) {
			if (unzLocateFile(it, name.c_str(), 0) == UNZ_OK) {
				return true;
			}
		}

		return false;
	}


	static FileSystemLocal fileSysLocal;
	FileSystem* GFileSys = &fileSysLocal;

}