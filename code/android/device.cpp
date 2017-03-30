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
#include <errno.h>
#include <jni.h>

using namespace std;
using namespace FancyTech;


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
							GLog.LogInfo("ioctl error %d", errno);
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
	return;
	
	DumpDevices("/dev/bus/usb");

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

