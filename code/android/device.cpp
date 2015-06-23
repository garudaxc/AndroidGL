#include "Platfrom.h"

#include <sys/ioctl.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/types.h>
#include <sys/stat.h>
#include <string>
#include "MyLog.h"

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
//
//
//
//
//
//
//
//





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
			} else {
				int device = open(devicePath, O_RDWR);
				if (device < 0)			{
					device = open(devicePath, O_RDONLY);
				}

				if (device >= 0) {
					GLog.LogInfo(devicePath);
					close(device);
				}
			}

			//GLog.LogInfo(devicePath);

			entry = readdir(dir);
		}
		closedir(dir);
	}
}



void DumpDevices()
{
	
	DumpDevices("/dev");
	return;

	DIR* dir = opendir("/dev");

	string info;
	
	if (dir) {
		dirent* entry = readdir(dir);
		while (entry){

			char devicePath[32];
			sprintf(devicePath, "/dev/%s", entry->d_name);
			entry = readdir(dir);
			GLog.LogInfo(devicePath);


			continue;

			
			int device = open(devicePath, O_RDWR);
			if (device < 0)			{
				device = open(devicePath, O_RDONLY);
			}

			if (device < 0) {
				continue;
				//GLog.LogInfo("open failed!");
			}
			GLog.LogInfo(devicePath);

			int i, res, desc_size = 0;
			char buf[256];

			struct hidraw_report_descriptor rpt_desc;
			struct hidraw_devinfo info;

			res = ioctl(device, HIDIOCGRAWINFO, &info);
			if (res < 0) {
				GLog.LogInfo("error HIDIOCGRAWINFO");
			}
			else {
				GLog.LogInfo("Raw Info:");
				GLog.LogInfo("\tbustype: %d", info.bustype);
				GLog.LogInfo("\tvendor: 0x%hx", info.vendor);
				GLog.LogInfo("\tproduct: 0x%hx", info.product);
			}
			
			close(device);
			continue;

			/* Get Report Descriptor Size */
			res = ioctl(device, HIDIOCGRDESCSIZE, &desc_size);
			if (res < 0) {
				GLog.LogInfo("error HIDIOCGRDESCSIZE");
				continue;
			}
			else
				GLog.LogInfo("Report Descriptor Size: %d\n", desc_size);

			/* Get Report Descriptor */
			rpt_desc.size = desc_size;
			res = ioctl(device, HIDIOCGRDESC, &rpt_desc);
			if (res < 0) {
				GLog.LogInfo("error HIDIOCGRDESC");
				continue;
			}
			else {
				char buffer[256];
				int pos = 0;
				for (i = 0; i < rpt_desc.size; i++) {
					pos += sprintf(buffer + pos, "%hhx ", rpt_desc.value[i]);
				}

				GLog.LogInfo("Report Descriptor: %s", buffer);
			}

			/* Get Raw Name */
			res = ioctl(device, HIDIOCGRAWNAME(256), buf);
			if (res < 0)
				GLog.LogInfo("error HIDIOCGRAWNAME");
			else
				GLog.LogInfo("Raw Name: %s", buf);
			
			close(device);

		}

		closedir(dir);
	}

	//GLog.LogInfo(info.c_str());

	GLog.LogInfo("device path end");

}