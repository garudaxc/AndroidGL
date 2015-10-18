#include <sys/ioctl.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <jni.h>
#include <string>
#include <vector>
#include "Platfrom.h"
#include "MyLog.h"
#include "Thread.h"
#include <unistd.h>
#include <errno.h>


using namespace std;
/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */

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
void DumpDevices();

JavaVM *jvm = NULL;
jclass activeClass = NULL;
jobject activeObject = NULL;
jmethodID readUsbMethod = NULL;

extern "C"
{

JNIEXPORT jstring JNICALL Java_com_example_mytest_MainActivity_stringFromJNI
  (JNIEnv *, jobject);

jstring
Java_com_example_mytest_MainActivity_stringFromJNI( JNIEnv* env,
                                                  jobject thiz )
{

    return env->NewStringUTF("xxxxxxxxxxxxx");
}




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

jbyteArray jBuffer = NULL;


JNIEXPORT void JNICALL Java_com_example_mytest_MainActivity_setupUsbDevice(JNIEnv * env, jobject thiz, jint fd)
{
	//env->GetJavaVM(&jvm);
	//activeObject = thiz;

	//activeClass = env->FindClass("com/example/mytest/MainActivity");
	//if (activeClass == NULL) {
	//	GLog.LogError("can not find java class");
	//}
	//	
	//
	//readUsbMethod = env->GetMethodID(activeClass, "ReadData", "([BII)I");
	//if (readUsbMethod == NULL) {
	//	GLog.LogError("can not find java method");
	//}

	//jBuffer = env->NewByteArray(128);


	//int size = jvm->CallIntMethod(activeObject, readUsbMethod, jBuffer, 128, 500);
	//jbyte buffer[128];
	//jvm->GetByteArrayRegion(jBuffer, 0, size, buffer);
	//GLog.LogInfo("read %d %c %c", size, buffer[0], buffer[1]);

	oculusDevice = fd;

	GLog.LogInfo("fd is %d", oculusDevice);

	GLog.LogInfo("pid %d tid %d uid %d", getpid(), gettid(), getuid());

	DumpDevices();

	return;

	//char buffer[256];
	//int size = read(fd, buffer, 256);
	//GLog.LogInfo("read size %d", size);
	

	char buf[1024] = { 0 };
	char file_path[PATH_MAX] = { 0 }; // PATH_MAX in limits.h
	snprintf(buf, sizeof (buf), "/proc/self/fd/%d", fd);
	if (readlink(buf, file_path, sizeof(file_path)-1) != -1) {
		GLog.LogInfo("file path : %s", file_path);
	}
	
	struct stat statbuf;
	if (fstat(fd, &statbuf) < 0) {
		GLog.LogError("lstat error!");
	}	

	GLog.LogInfo("%d %s Permissions 0x%X", fd, GetFileType(statbuf), statbuf.st_mode & 0x1FF);

	hidraw_devinfo info;
	memset(&info, 0, sizeof(info));
	int r = ioctl(fd, HIDIOCGRAWINFO, &info);

	if (r < 0) {
		GLog.LogInfo("ioctl error %d", errno);
		//close(fd);
	}
	else {
		GLog.LogInfo("hid raw info %u vendor=%hd product=%hd", info.bustype, info.vendor, info.product);
	}
	
	int ftl = fcntl(fd, F_GETFL, 0);
	if (ftl < 0){
		GLog.LogInfo("fcntl error %d %d", ftl, errno);
	} else {
		GLog.LogInfo("fcntl GETFL 0x%X", ftl);
	}
	
}


JNIEXPORT void JNICALL Java_com_example_mytest_MainActivity_read(JNIEnv * env, jobject thiz)
{
	if (oculusDevice == 0){
		return;
	}

	char buffer[256];

	int size = read(oculusDevice, buffer, 256);
	GLog.LogInfo("read size %d", size);

}


JNIEXPORT void JNICALL Java_com_example_mytest_MainActivity_PushData
  (JNIEnv * env, jobject obj, jbyteArray data, jint size)
{

	GLog.LogInfo("push data %d", size);

	if (size < 0) {
		exit(0);
	}

	//jbyte buffer[128];
	//env->GetByteArrayRegion(data, 0, size, buffer);
	//GLog.LogInfo("read %d %c %c", size, buffer[0], buffer[1]);
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
	int x = 10;
	while (x--){

		GLog.LogInfo("DeviceThread::Run()");
		int waitMs = 50;
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
						GLog.LogInfo("DeviceManagerThread - poll error POLLERR %d ", fds_[i].fd);
					}

					if (revents & POLLHUP)
					{
						GLog.LogInfo("DeviceManagerThread - poll error POLLNVAL %d ", fds_[i].fd);
					}
					if (revents & POLLNVAL)
					{
						GLog.LogInfo("DeviceManagerThread - poll error POLLNVAL %d ", fds_[i].fd);
					}


					char buffer[256];
					int bytes = read(fds_[i].fd, buffer, 256);

					GLog.LogInfo("read %d bytes timestamps %d", bytes, GetTicksMS());
				}
			}
		} else if (n == 0) {
			GLog.LogInfo("time out");
		} else {
			GLog.LogInfo("error");
		}
	}

}




//void* DeviceThread::Run()
//{
//	GLog.LogInfo("DeviceThread::Run()");
//
//
//	JNIEnv *env;
//
//	jvm->AttachCurrentThread(&env, NULL);
//
//	while (true){
//
//		int waitMs = 500;
//
//		int size = env->CallIntMethod(activeObject, readUsbMethod, jBuffer, 128, waitMs);
//		jbyte buffer[128];
//		env->GetByteArrayRegion(jBuffer, 0, size, buffer);
//		GLog.LogInfo("read %d %c %c", size, buffer[0], buffer[1]);
//		
//	}
//
//	jvm->DetachCurrentThread();
//}




static DeviceThread deviceThread;




void DumpDevices()
{	
	if (oculusDevice <= 0){
		GLog.LogError("Create oculus device failed");
		return;
	}

	int CommandFd[2];
	int result = pipe(CommandFd);

	GLog.LogInfo("pipe %d %d", CommandFd[0], CommandFd[1]);


	struct pollfd pfd;
	pfd.fd = CommandFd[0];
	//pfd.events = POLLIN | POLLHUP | POLLERR;
	pfd.events = POLLIN;
	pfd.revents = 0;

	deviceThread.fds_.push_back(pfd);


	pfd.fd = oculusDevice;
	//pfd.events = POLLIN | POLLHUP | POLLERR;
	pfd.events = POLLIN;
	pfd.revents = 0;

	deviceThread.fds_.push_back(pfd);

	//bool r = deviceThread.Create();
	//if (!r) {
	//	GLog.LogError("deviceThread.Create() failed!");
	//}
	
	deviceThread.Run();

}
