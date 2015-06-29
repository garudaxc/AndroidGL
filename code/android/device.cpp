#include "Platfrom.h"

#include <sys/ioctl.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <string>
#include <vector>
#include "MyLog.h"
#include "Thread.h"


using namespace std;


#define HIDIOCGRDESCSIZE	_IOR('H', 0x01, int)
#define HIDIOCGRDESC		_IOR('H', 0x02, struct hidraw_report_descriptor)
#define HIDIOCGRAWINFO		_IOR('H', 0x03, struct hidraw_devinfo)
#define HIDIOCGRAWNAME(len)     _IOC(_IOC_READ, 'H', 0x04, len)
#define HIDIOCGRAWPHYS(len)     _IOC(_IOC_READ, 'H', 0x05, len)
// The first byte of SFEATURE and GFEATURE is the report number
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)


#define HID_MAX_DESCRIPTOR_SIZE		4096
struct hidraw_report_descriptor {
	__u32 size;
	__u8 value[HID_MAX_DESCRIPTOR_SIZE];
};

struct hidraw_devinfo {
	__u32 bustype;
	__s16 vendor;
	__s16 product;
};




//
//
//
//
///* function type that is called for each filename */
//typedef int Myfunc(const char *, const struct stat *, int);
//static Myfunc myfunc;
//static int myftw(char *, Myfunc *);
//static int dopath(Myfunc *);
//static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;
//
//#define FTW_F 1/*file other than directory */
//#define FTW_D 2/*directory */
//#define FTW_DNR 3 /* directory that can¡¯t be read */
//#define FTW_NS 4/*file that we can¡¯t stat */
//
//static char *fullpath = "/dev";
//static size_t pathlen;
//
//	/*Descend through the hierarchy, starting at "fullpath".
//	* If "fullpath" is anything other than a directory, we lstat() it,
//	*call func(), and return. For a directory, we call ourself
//	*recursively for each name in the directory.
//	*/
//	static int /* we return whatever func() returns */
//		dopath(Myfunc* func)
//	{
//			struct stat statbuf;
//			struct dirent *dirp;
//			DIR  *dp;
//			int  ret, n;
//			if (lstat(fullpath, &statbuf) < 0) /* stat error */
//				return(func(fullpath, &statbuf, FTW_NS));
//			if (S_ISDIR(statbuf.st_mode) == 0) /* not a directory */
//				return(func(fullpath, &statbuf, FTW_F));
//			/*
//			*It¡¯s a directory. First call func() for the directory,
//			*then process each filename in the directory.
//			*/
//			if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
//				return(ret);
//			n = strlen(fullpath);
//			if (n + NAME_MAX + 2 > pathlen) { /* expand path buffer */
//				pathlen *= 2;
//				if ((fullpath = (char*)realloc(fullpath, pathlen)) == NULL)
//					GLog.LogInfo("realloc failed");
//			}
//			fullpath[n++] = '/';
//			fullpath[n] = 0;
//			if ((dp = opendir(fullpath)) == NULL) /* can¡¯t read directory */
//				return(func(fullpath, &statbuf, FTW_DNR));
//			while ((dirp = readdir(dp)) != NULL) {
//				if (strcmp(dirp->d_name, ".") == 0 ||
//					strcmp(dirp->d_name, "..") == 0)
//					continue;  /* ignore dot and dot-dot */
//				strcpy(&fullpath[n], dirp->d_name); /* append name after "/" */
//				if ((ret = dopath(func)) != 0) /* recursive */
//					break;  /* time to leave */
//			}
//			fullpath[n - 1] = 0; /* erase everything from slash onward */
//			if (closedir(dp) < 0)
//				GLog.LogInfo("can't close directory %s", fullpath);
//			return(ret);
//		}
//	static int myfunc(const char *pathname, const struct stat *statptr, int type)
//	{
//		GLog.LogInfo(pathname);
//
//			switch (type) {
//			case FTW_F:
//				switch (statptr->st_mode & S_IFMT) {
//				case S_IFREG: nreg++;  break;
//				case S_IFBLK: nblk++;  break;
//				case S_IFCHR: nchr++;  break;
//				case S_IFIFO: nfifo++;  break;
//				case S_IFLNK: nslink++;  break;
//				case S_IFSOCK: nsock++;  break;
//				case S_IFDIR: /* directories should have type = FTW_D */
//					GLog.LogInfo("for S_IFDIR for %s", pathname);
//				}
//				break;
//			case FTW_D:
//				ndir++;
//				break;
//			case FTW_DNR:
//				GLog.LogInfo("can't read directory %s", pathname);
//				break;
//			case FTW_NS:
//				GLog.LogInfo("stat error for %s", pathname);
//				break;
//			default:
//				GLog.LogInfo("unknown type %d for pathname %s", type, pathname);
//			}
//			return(0);
//		}
//
//
//
//
//
//
//


int oculusDevice = 0;


const char* GetFileType(const struct stat& buf)
{
	const char* ptr = NULL;

	if (S_ISREG(buf.st_mode))
		ptr = "regular";
	else if (S_ISDIR(buf.st_mode))
		ptr = "directory";
	else if (S_ISCHR(buf.st_mode))
		ptr = "character special";
	else if (S_ISBLK(buf.st_mode))
		ptr = "block special";
	else if (S_ISFIFO(buf.st_mode))
		ptr = "fifo";
	else if (S_ISLNK(buf.st_mode))
		ptr = "symbolic link";
	else if (S_ISSOCK(buf.st_mode))
		ptr = "socket";
	else
		ptr = "** unknown mode **";

	return ptr;
}
//
//void Permsion()
//{
//	S_IRUSR
//}


void DumpDevices(const char* sdir)
{
	DIR* dir = opendir(sdir);

	if (dir) {
		dirent* entry = readdir(dir);
		while (entry){

			if (strstr(entry->d_name, ".") || strstr(entry->d_name, "..")) {
				entry = readdir(dir);
				continue;
			}

			char devicePath[128];
			sprintf(devicePath, "%s/%s", sdir, entry->d_name);
			struct stat statbuf;
			if (lstat(devicePath, &statbuf) < 0) {
				GLog.LogError("lstat error!");
				entry = readdir(dir);
				continue;
			}

			if (S_ISDIR(statbuf.st_mode)) {
				DumpDevices(devicePath);
			}
			else {
				int device = open(devicePath, O_RDWR);
				if (device < 0)			{
					device = open(devicePath, O_RDONLY);
				}

				if (device >= 0) {
					GLog.LogInfo("%s %s Permissions 0x%X", devicePath, GetFileType(statbuf), statbuf.st_mode & 0x1FF);

					//if (S_ISBLK(statbuf.st_mode)){
						hidraw_devinfo info;
						memset(&info, 0, sizeof(info));
						int r = ioctl(device, HIDIOCGRAWINFO, &info);

						if (r < 0) {
							GLog.LogInfo("ioctl error");
							close(device);
						}
						else {
							GLog.LogInfo("hid raw info %u vendor=%hd product=%hd", info.bustype, info.vendor, info.product);

							//oculusDevice = device;
						}
					//}

					close(device);
				}

			}

			//GLog.LogInfo(devicePath);

			entry = readdir(dir);
		}
		closedir(dir);
	}
}




void CreateOculusDevices(const char* sdir)
{
	DIR* dir = opendir(sdir);
	
	if (dir) {
		dirent* entry = readdir(dir);
		while (entry){

			if (strstr(entry->d_name, ".") || strstr(entry->d_name, "..")) {
				entry = readdir(dir);
				continue;
			}

			char devicePath[128];
			sprintf(devicePath, "%s/%s", sdir, entry->d_name);
			struct stat statbuf;
			if (lstat(devicePath, &statbuf) < 0) {
				GLog.LogError("lstat error!");
				entry = readdir(dir);
				continue;
			}

			if (S_ISDIR(statbuf.st_mode)) {
				CreateOculusDevices(devicePath);
			} else {
				int device = open(devicePath, O_RDWR);
				if (device < 0)			{
					device = open(devicePath, O_RDONLY);
				}

				if (device >= 0) {

					if (strstr(devicePath, "ovr")){
						hidraw_devinfo info;
						memset(&info, 0, sizeof(info));
						int r = ioctl(device, HIDIOCGRAWINFO, &info);

						if (r < 0) {
							GLog.LogInfo("ioctl error");
							close(device);
						} else {
							GLog.LogInfo("hid raw info %u vendor=%hd product=%hd", info.bustype, info.vendor, info.product);
							
							oculusDevice = device;
						}
					}
					
					//close(device);
				}

				GLog.LogInfo("%s %d", devicePath, device);
			}

			//GLog.LogInfo(devicePath);

			entry = readdir(dir);
		}
		closedir(dir);
	}
}









class DeviceThread : public Thread
{
public:
	DeviceThread();
	~DeviceThread()	{}

	virtual void*		Run();

	vector<struct pollfd> fds_;

private:
	
};


DeviceThread::DeviceThread()
{

}


void* DeviceThread::Run()
{
	while (true){

		int waitMs = 500;
		nfds_t nfds = fds_.size();

		int n = poll(&fds_[0], nfds, waitMs);

		if (n > 0)
		{
			// Iterate backwards through the list so the ordering will not be
			// affected if the called object gets removed during the callback
			// Also, the HID data streams are located toward the back of the list
			// and servicing them first will allow a disconnect to be handled
			// and cleaned directly at the device first instead of the general HID monitor
			for (int i = nfds - 1; i >= 0; i--)
			{
				const short revents = fds_[i].revents;

				// If there was an error or hangup then we continue, the read will fail, and we'll close it.
				if (revents & (POLLIN | POLLERR | POLLHUP))
				{
					if (revents & POLLERR)
					{
						GLog.LogInfo("DeviceManagerThread - poll error event %d ", fds_[i].fd);
					}

					char buffer[256];
					int bytes = read(fds_[i].fd, buffer, 256);
					
					GLog.LogInfo("read %d bytes timestamps %d", bytes, GetTicksMS());
				}
			}
		}


	}

}

static DeviceThread deviceThread;


void DumpDevices()
{
	
	DumpDevices("/dev");

	return;

	if (oculusDevice <= 0){
		GLog.LogError("Create oculus device failed");
		return;
	}

	struct pollfd pfd;
	pfd.fd = oculusDevice;
	pfd.events = POLLIN | POLLHUP | POLLERR;
	pfd.revents = 0;

	deviceThread.fds_.push_back(pfd);


	bool r = deviceThread.Create();
	if (!r) {
		GLog.LogError("deviceThread.Create() failed!");
	}

}

