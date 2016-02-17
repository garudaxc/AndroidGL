#include <jni.h>
#include "libusb.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "MyLog.h"
#include <jni.h>
#include "Thread.h"

#include "AurMath.h"
#include "Platfrom.h"
#include "SensorDevice.h"
#include "Calibration.h"

using namespace FancyTech;


#if !defined(bool)
#define bool int
#endif
#if !defined(true)
#define true (1 == 1)
#endif
#if !defined(false)
#define false (!true)
#endif

enum 
{
	DEVICE_M3D = 0,
	DEVICE_DK1
};

// Future versions of libusb will use usb_interface instead of interface
// in libusb_config_descriptor => catter for that
#define usb_interface interface

// Global variables
static bool extra_info = false;

static int devicefd = -1;
static libusb_device_handle * devHandle = NULL;
static int devEndPoint = -1;
static int deviceType = DEVICE_M3D;

static int perr(char const *format, ...)
{
	return 1;

	//va_list args;
	//int r;

	//va_start (args, format);
	//r = LOGE(stderr, format, args);
	//va_end(args);

	//return r;
}

#define ERR_EXIT(errcode) do { perr("   %s\n", libusb_strerror((enum libusb_error)errcode)); return -1; } while (0)
#define CALL_CHECK(fcall) do { r=fcall; if (r < 0) ERR_EXIT(r); } while (0);



int PrintDeviceInfo(libusb_device_handle *handle)
{

	libusb_device * dev;
	uint8_t bus, port_path[8];
	struct libusb_bos_descriptor *bos_desc;
	struct libusb_config_descriptor *conf_desc;
	const struct libusb_endpoint_descriptor *endpoint;
	int i, j, k, r;
	int iface, nb_ifaces, first_iface = -1;
	struct libusb_device_descriptor dev_desc;
	const char* speed_name[5] = { "Unknown", "1.5 Mbit/s (USB LowSpeed)", "12 Mbit/s (USB FullSpeed)",
		"480 Mbit/s (USB HighSpeed)", "5000 Mbit/s (USB SuperSpeed)"};
	char string[128];
	uint8_t string_index[3];	// indexes of the string descriptors
	uint8_t endpoint_in = 0, endpoint_out = 0;	// default IN and OUT endpoints

	//GLog.LogInfo("Opening device %04X:%04X...\n", vid, pid);
	//handle = libusb_open_device_with_vid_pid(NULL, vid, pid);

	if (handle == NULL) {
		perr("  Failed.\n");
		return -1;
	}

	dev = libusb_get_device(handle);
	bus = libusb_get_bus_number(dev);
	if (extra_info) {
		r = libusb_get_port_numbers(dev, port_path, sizeof(port_path));
		if (r > 0) {
			GLog.LogInfo("\nDevice properties:");
			GLog.LogInfo("        bus number: %d", bus);
			GLog.LogInfo("         port path: %d", port_path[0]);
			for (i=1; i<r; i++) {
				GLog.LogInfo("->%d", port_path[i]);
			}
			GLog.LogInfo(" (from root hub)");
		}
		r = libusb_get_device_speed(dev);
		if ((r<0) || (r>4)) r=0;
		GLog.LogInfo("             speed: %s", speed_name[r]);
	}

	GLog.LogInfo("\nReading device descriptor:");
	CALL_CHECK(libusb_get_device_descriptor(dev, &dev_desc));
	GLog.LogInfo("            length: %d", dev_desc.bLength);
	GLog.LogInfo("      device class: %d", dev_desc.bDeviceClass);
	GLog.LogInfo("               S/N: %d", dev_desc.iSerialNumber);
	GLog.LogInfo("           VID:PID: %04X:%04X", dev_desc.idVendor, dev_desc.idProduct);
	GLog.LogInfo("         bcdDevice: %04X", dev_desc.bcdDevice);
	GLog.LogInfo("   iMan:iProd:iSer: %d:%d:%d", dev_desc.iManufacturer, dev_desc.iProduct, dev_desc.iSerialNumber);
	GLog.LogInfo("          nb confs: %d", dev_desc.bNumConfigurations);
	// Copy the string descriptors for easier parsing
	string_index[0] = dev_desc.iManufacturer;
	string_index[1] = dev_desc.iProduct;
	string_index[2] = dev_desc.iSerialNumber;

	GLog.LogInfo("\nReading string descriptors:");
	for (i=0; i<3; i++) {
		if (string_index[i] == 0) {
			continue;
		}
		if (libusb_get_string_descriptor_ascii(handle, string_index[i], (unsigned char*)string, 128) >= 0) {
			GLog.LogInfo("   String (0x%02X): \"%s\"", string_index[i], string);
		}
	}	

	GLog.LogInfo("\nReading first configuration descriptor:");
	CALL_CHECK(libusb_get_config_descriptor(dev, 0, &conf_desc));
	nb_ifaces = conf_desc->bNumInterfaces;
	GLog.LogInfo("             nb interfaces: %d", nb_ifaces);
	if (nb_ifaces > 0)
		first_iface = conf_desc->usb_interface[0].altsetting[0].bInterfaceNumber;
	for (i=0; i<nb_ifaces; i++) {
		GLog.LogInfo("              interface[%d]: id = %d", i,
			conf_desc->usb_interface[i].altsetting[0].bInterfaceNumber);
		for (j=0; j<conf_desc->usb_interface[i].num_altsetting; j++) {
			GLog.LogInfo("interface[%d].altsetting[%d]: num endpoints = %d",
				i, j, conf_desc->usb_interface[i].altsetting[j].bNumEndpoints);
			GLog.LogInfo("   Class.SubClass.Protocol: %02X.%02X.%02X",
				conf_desc->usb_interface[i].altsetting[j].bInterfaceClass,
				conf_desc->usb_interface[i].altsetting[j].bInterfaceSubClass,
				conf_desc->usb_interface[i].altsetting[j].bInterfaceProtocol);

			for (k=0; k<conf_desc->usb_interface[i].altsetting[j].bNumEndpoints; k++) {
				struct libusb_ss_endpoint_companion_descriptor *ep_comp = NULL;
				endpoint = &conf_desc->usb_interface[i].altsetting[j].endpoint[k];
				
				GLog.LogInfo("       endpoint[%d].address: %02X, direction : %s", k, endpoint->bEndpointAddress,
					endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN ? "in" : "out");

				static const char* transferType[] = {
					"Control endpoint",
					"Isochronous endpoint",
					"Bulk endpoint",
					"Interrupt endpoint",
					"Stream endpoint",
				};
				
				GLog.LogInfo("      transfer type : %s", transferType[endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK]);


				//// Use the first interrupt or bulk IN/OUT endpoints as default for testing
				//if ((endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) & (LIBUSB_TRANSFER_TYPE_BULK | LIBUSB_TRANSFER_TYPE_INTERRUPT)) {
				//	if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN) {
				//		if (!endpoint_in)
				//			endpoint_in = endpoint->bEndpointAddress;
				//	} else {
				//		if (!endpoint_out)
				//			endpoint_out = endpoint->bEndpointAddress;
				//	}
				//}
				GLog.LogInfo("           max packet size: %04X", endpoint->wMaxPacketSize);
				GLog.LogInfo("          polling interval: %02X", endpoint->bInterval);
				libusb_get_ss_endpoint_companion_descriptor(NULL, endpoint, &ep_comp);
				if (ep_comp) {
					GLog.LogInfo("                 max burst: %02X   (USB 3.0)", ep_comp->bMaxBurst);
					GLog.LogInfo("        bytes per interval: %04X (USB 3.0)", ep_comp->wBytesPerInterval);
					libusb_free_ss_endpoint_companion_descriptor(ep_comp);
				}
			}
		}
	}
	libusb_free_config_descriptor(conf_desc);

	endpoint_in = 0x81;
	GLog.LogInfo("first interface %d endporint in 0x%x", first_iface, (uint32_t)endpoint_in);

	if (libusb_kernel_driver_active(handle, first_iface) == 1) {

		GLog.LogInfo("driver is active");

		if (!libusb_detach_kernel_driver(handle, first_iface)) {
			GLog.LogInfo("Detached kernel driver");
		} else {
			GLog.LogInfo("Detaching kernel driver failed!");
			return -1;
		}
	}
	
	r = libusb_claim_interface(handle, first_iface);
	if (r != LIBUSB_SUCCESS) {
		GLog.LogInfo(" libusb_claim_interface Failed. %d", r);
		return -1;
	}

	devHandle = handle;
	devEndPoint = endpoint_in;
	
	return 1;
}


int libUsbTest(int fd, const char* path)
{
	libusb_context* context = NULL;
	int status = libusb_init2(&context, path);
	if (status != LIBUSB_SUCCESS){
		GLog.LogError("libusb_init2 failed!");
		return status;
	} else if (context == NULL) {
		GLog.LogError("context error NULL");
		return LIBUSB_ERROR_OTHER;
	}

	libusb_device* device = NULL;
	libusb_device **list;
	libusb_device_handle *handle;
	libusb_device_descriptor dd;
	int cnt = libusb_get_device_list(context, &list);
	GLog.LogInfo("device count %d", cnt);

	int deviceFound = 0;
	int r = 0;

	for (int i = 0; i < cnt; i++) {
		r = libusb_get_device_descriptor(list[i], &dd);
		if (r < 0) {
			GLog.LogError("libusb_get_device_descriptor failed %d", r);
			continue;
		}

		if ((dd.idVendor == 10291 && dd.idProduct == 1)) {
			deviceType = DEVICE_DK1;
			deviceFound++;
			device = list[i];
		} else if ((dd.idVendor == 1155 && dd.idProduct == 22336) ||
			(dd.idVendor == 949 && dd.idProduct == 1)) {
			deviceType = DEVICE_M3D;
			deviceFound++;
			device = list[i];
		}
	}

	if (device == NULL) {
		GLog.LogError("can not find device !");
		return -1;
	}

	r = libusb_open2(device, &handle, fd);
	if (r < 0) {
		libusb_free_device_list(list, 1);

		GLog.LogError("open device failed %d", r);
		return -1;
	}

	libusb_free_device_list(list, 1);

	PrintDeviceInfo(handle);

	return 0;
}


class UsbDeviceThread : public Thread
{
public:
	UsbDeviceThread();
	~UsbDeviceThread()	{}

	virtual void*		Run();

private:

};


UsbDeviceThread::UsbDeviceThread() :Thread("UsbDeviceThread")
{
}

struct SensorDeviceSample
{
	uint16_t	timestamp;
	int16_t		temperature;

	struct gyroSample
	{
		uint16_t	accel[3];
		uint16_t	gyro[3];
	}	sample[2];

	uint16_t	magnet[3];
};



bool init_ = true;
static Quaternionf gyroRot_;
static float EPSILON = 0.0001f;
void UpdateGyroScope(const Vector3f& v, int64_t timestamp)
{
	//intigrate GyroValue
	static int64_t lastTime = 0;
	if (init_ || lastTime == 0)
	{
		lastTime = timestamp;
		gyroRot_ = Quaternionf::RotationAxis(Vector3f::UNIT_Z, Mathf::HALF_PI);
		init_ = false;
		return;
	}
	float t = (timestamp - lastTime) * 0.000000001f;
	lastTime = timestamp;

	// 注意这里直接取逆了
	Vector3f eyroSample = -v;

	float omegaMagnitude = eyroSample.Length();
	if (omegaMagnitude < EPSILON) {
		return;
	}

	eyroSample.Normalize();

	float thetaOverTwo = omegaMagnitude * t * 0.5f;
	float sinThetaOverTwo = Mathf::Sin(thetaOverTwo);
	float cosThetaOverTwo = Mathf::Cos(thetaOverTwo);

	eyroSample *= sinThetaOverTwo;
	Quaternionf qRot(cosThetaOverTwo, eyroSample.x, eyroSample.y, eyroSample.z);

	gyroRot_ = qRot * gyroRot_;
}

Matrix4f _GetDeviceRotationMatrix()
{
	Matrix4f view = Matrix4f::Transform(gyroRot_, Vector3f::ZERO);

	//// transform from phone coordinate to camera coordinate
	//// phone handled in reverseLandscape
	//view *= Matrix4f(0.f, -1.f, 0.f, 0.f,
	//	1.f, 0.f, 0.f, 0.f,
	//	0.f, 0.f, 1.f, 0.f,
	//	0.f, 0.f, 0.f, 1.f);

	return view;
}

static int sampleCount = 0;
static int lastSampleCount = 0;
static uint64_t lastTimeStamp = 0;


int readlen = 0;

//////////////////////////////////////////////////////////////////////////

float DECOM(const uint8_t * raw, float scale)
{
	short v = raw[0] << 8 | raw[1];
	float f = ((float)v / (short)0x7fff) * scale;
	return f;
}

Vector3f DeomposeData(const uint8_t * data, float scale)
{
	Vector3f v;

	v.x = DECOM(data, scale);
	v.y = DECOM(data + 2, scale);
	v.z = DECOM(data + 4, scale);

	return v;
}

template<class T>
T DecodeData(const uint8_t* data)
{
	return (T(data[0]) << 8) | T(data[1]);
}

//////////////////////////////////////////////////////////////////////////
typedef int8_t          SByte;
typedef uint8_t         UByte;
typedef int16_t         SInt16;
typedef uint16_t        UInt16;
typedef int32_t         SInt32;
typedef uint32_t        UInt32;
typedef int64_t         SInt64;
typedef uint64_t        UInt64;


// Reported data is little-endian now
static UInt16 DecodeUInt16(const UByte* buffer)
{
	return (UInt16(buffer[1]) << 8) | UInt16(buffer[0]);
}

static SInt16 DecodeSInt16(const UByte* buffer)
{
	return (SInt16(buffer[1]) << 8) | SInt16(buffer[0]);
}

static UInt32 DecodeUInt32(const UByte* buffer)
{
	return (buffer[0]) | UInt32(buffer[1] << 8) | UInt32(buffer[2] << 16) | UInt32(buffer[3] << 24);
}

//static void EncodeUInt16(UByte* buffer, UInt16 val)
//{
//	*(UInt16*)buffer = Alg::ByteUtil::SystemToLE(val);
//}

static float DecodeFloat(const UByte* buffer)
{
	union {
		UInt32 U;
		float  F;
	};

	U = DecodeUInt32(buffer);
	return F;
}


static void UnpackSensor(const UByte* buffer, SInt32* x, SInt32* y, SInt32* z)
{
	// Sign extending trick
	// from http://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
	struct { SInt32 x : 21; } s;

	*x = s.x = (buffer[0] << 13) | (buffer[1] << 5) | ((buffer[2] & 0xF8) >> 3);
	*y = s.x = ((buffer[2] & 0x07) << 18) | (buffer[3] << 10) | (buffer[4] << 2) |
		((buffer[5] & 0xC0) >> 6);
	*z = s.x = ((buffer[5] & 0x3F) << 15) | (buffer[6] << 7) | (buffer[7] >> 1);
}



struct DK1TrackerSample
{
	SInt32 AccelX, AccelY, AccelZ;
	SInt32 GyroX, GyroY, GyroZ;
};


struct TrackerSensors
{
	UByte	SampleCount;
	UInt16	Timestamp;
	UInt16	LastCommandID;
	SInt16	Temperature;

	DK1TrackerSample Samples[3];

	SInt16	MagX, MagY, MagZ;

	int Decode(const UByte* buffer, int size)
	{
		if (size < 62)
			return -1;

		SampleCount = buffer[1];
		Timestamp = DecodeUInt16(buffer + 2);
		LastCommandID = DecodeUInt16(buffer + 4);
		Temperature = DecodeSInt16(buffer + 6);

		//if (SampleCount > 2)        
		//    OVR_DEBUG_LOG_TEXT(("TackerSensor::Decode SampleCount=%d\n", SampleCount));        

		// Only unpack as many samples as there actually are
		UByte iterationCount = (SampleCount > 2) ? 3 : SampleCount;

		for (UByte i = 0; i < iterationCount; i++)
		{
			UnpackSensor(buffer + 8 + 16 * i, &Samples[i].AccelX, &Samples[i].AccelY, &Samples[i].AccelZ);
			UnpackSensor(buffer + 16 + 16 * i, &Samples[i].GyroX, &Samples[i].GyroY, &Samples[i].GyroZ);
		}

		MagX = DecodeSInt16(buffer + 56);
		MagY = DecodeSInt16(buffer + 58);
		MagZ = DecodeSInt16(buffer + 60);

		return 0;
	}
};



struct SensorConfigImpl
{
	enum  { PacketSize = 7 };
	UByte   Buffer[PacketSize];

	// Flag values for Flags.
	enum {
		Flag_RawMode = 0x01,
		Flag_CalibrationTest = 0x02, // Internal test mode
		Flag_UseCalibration = 0x04,
		Flag_AutoCalibration = 0x08,
		Flag_MotionKeepAlive = 0x10,
		Flag_CommandKeepAlive = 0x20,
		Flag_SensorCoordinates = 0x40
	};

	UInt16  CommandId;
	UByte   Flags;
	UInt16  PacketInterval;
	UInt16  KeepAliveIntervalMs;

	SensorConfigImpl() : CommandId(0), Flags(0), PacketInterval(0), KeepAliveIntervalMs(0)
	{
		memset(Buffer, 0, PacketSize);
		Buffer[0] = 2;
	}

	void    SetSensorCoordinates(bool sensorCoordinates)
	{
		Flags = (Flags & ~Flag_SensorCoordinates) | (sensorCoordinates ? Flag_SensorCoordinates : 0);
	}
	bool    IsUsingSensorCoordinates() const
	{
		return (Flags & Flag_SensorCoordinates) != 0;
	}

	void Pack()
	{
		Buffer[0] = 2;
		Buffer[1] = UByte(CommandId & 0xFF);
		Buffer[2] = UByte(CommandId >> 8);
		Buffer[3] = Flags;
		Buffer[4] = UByte(PacketInterval);
		Buffer[5] = UByte(KeepAliveIntervalMs & 0xFF);
		Buffer[6] = UByte(KeepAliveIntervalMs >> 8);
	}

	void Unpack()
	{
		CommandId = Buffer[1] | (UInt16(Buffer[2]) << 8);
		Flags = Buffer[3];
		PacketInterval = Buffer[4];
		KeepAliveIntervalMs = Buffer[5] | (UInt16(Buffer[6]) << 8);
	}

};

struct SensorKeepAliveImpl
{
	enum  { PacketSize = 5 };
	UByte   Buffer[PacketSize];

	UInt16  CommandId;
	UInt16  KeepAliveIntervalMs;

	SensorKeepAliveImpl(UInt16 interval = 0, UInt16 commandId = 0)
		: CommandId(commandId), KeepAliveIntervalMs(interval)
	{
		Pack();
	}

	void  Pack()
	{
		Buffer[0] = 8;
		Buffer[1] = UByte(CommandId & 0xFF);
		Buffer[2] = UByte(CommandId >> 8);
		Buffer[3] = UByte(KeepAliveIntervalMs & 0xFF);
		Buffer[4] = UByte(KeepAliveIntervalMs >> 8);
	}

	void Unpack()
	{
		CommandId = Buffer[1] | (UInt16(Buffer[2]) << 8);
		KeepAliveIntervalMs = Buffer[3] | (UInt16(Buffer[4]) << 8);
	}
};

//////////////////////////////////////////////////////////////////////////

void DK1Setup() {

	static int configed = 0;
	if (configed == 1) {
		return;
	}

	int r = 0;
	SensorConfigImpl config;

	//ctrl.bmRequestType = 0xa1;		// device to host / type : class / Recipient : interface
	//ctrl.bRequest = 0x01;			// get report
	//ctrl.wValue = 0x300 | 2;		// HID_FEATURE | id
	//ctrl.wIndex = 0;
	//ctrl.wLength = size;
	//ctrl.data = data;
	//ctrl.timeout = 0;

	r = libusb_control_transfer(devHandle, 0xa1, 0x01, 0x300 | 2, 0, config.Buffer, config.PacketSize, 0);

	GLog.LogInfo("libusb_control_transfer %d", r);

	if (r >= 0) {
		config.Unpack();
		config.Flags &= ~SensorConfigImpl::Flag_MotionKeepAlive;
		config.Pack();

		r = libusb_control_transfer(devHandle, 0x21, 0x09, 0x300 | 2, 0, config.Buffer, config.PacketSize, 0);

		GLog.LogInfo("libusb_control_transfer %d", r);

		configed = 1;
	}
}

void Dk1KeekAlive()
{
	SensorKeepAliveImpl keepAlive;
	keepAlive.KeepAliveIntervalMs = 3000;
	keepAlive.CommandId = 0;
	keepAlive.Pack();
	
	int r = libusb_control_transfer(devHandle, 0x21, 0x09, 0x300 | keepAlive.Buffer[0], 0, keepAlive.Buffer, keepAlive.PacketSize, 0);
	GLog.LogInfo("libusb_control_transfer %d", r);
}


void Fancy3DKeepAlive()
{
	uint8_t buffer = 0x31;
	int reallen = 0;
	int r = libusb_bulk_transfer(devHandle, 0x03, &buffer, 1, &readlen, 0);
	//GLog.LogInfo("KeepAlive %d reallen %d", r, readlen);
}


void* UsbDeviceThread::Run()
{	
	uint8_t buffer[128];
	//int readLen = 0;
	int r = 0;

	Dk1KeekAlive();
	GLog.LogInfo("UsbDeviceThread::Run()");

	while (r == 0 && readlen >= 0 && Check()){

		r = libusb_bulk_transfer(devHandle, devEndPoint, buffer, 128, &readlen, 10);

		//GLog.LogInfo("libusb_bulk_transfer %d", readlen);

		if (r < 0) {
			GLog.LogError("libusb_bulk_transfer failed! %d", r);
			break;
		}

		if (deviceType == DEVICE_DK1) {
			assert(readlen == 62);

			TrackerSample sample;

			TrackerSensors s;
			s.Decode(buffer, readlen);
			sample.accelerate.x = (float)s.Samples[0].AccelX;
			sample.accelerate.y = (float)s.Samples[0].AccelY;
			sample.accelerate.z = (float)s.Samples[0].AccelZ;
			sample.accelerate *= 0.0001f;
			
			sample.gyro.x = (float)s.Samples[0].GyroX;
			sample.gyro.y = (float)s.Samples[0].GyroY;
			sample.gyro.z = (float)s.Samples[0].GyroZ;
			sample.gyro *= 0.0001f;

			sample.magnet.x = (float)s.MagX;
			sample.magnet.y = (float)s.MagY;
			sample.magnet.z = (float)s.MagZ;
			sample.magnet *= 0.0001f;

			sample.timestamp = s.Timestamp;
			sample.temperature = s.Temperature;

			uint64_t timeStamp = GetTicksNanos();
			UpdateGyroScope(sample.gyro, timeStamp);

			sampleCount++;
			if (timeStamp - lastTimeStamp > 1000000000)	{
				float freq = ((sampleCount - lastSampleCount) * 1000000000.f) / (timeStamp - lastTimeStamp);
				lastSampleCount = sampleCount;
				lastTimeStamp = timeStamp;

				GLog.LogInfo("freq %.2f", freq);
				Dk1KeekAlive();
			}

		} else if (deviceType == DEVICE_M3D) {
			assert(readlen == sizeof(SensorDeviceSample));

			//SensorDeviceSample sample;

			uint8_t* data = buffer;

			uint16_t timestamp = DecodeData<uint16_t>(data);
			data += 2;
			
			float temperature = DecodeData<int16_t>(data) / 340.f + 36.53f;
			data += 2;
			
			TrackerSample sample[2];
			sample[0].timestamp = timestamp;
			sample[0].temperature = temperature;

			// 量程+/-4G
			sample[0].accelerate = DeomposeData(data, 4.0f);
			// 量程 +/- 1000度/s
			sample[0].gyro = DeomposeData(data + 6, 1000.f * Mathf::DEG_TO_RAD);
			data += 12;

			sample[1].timestamp = timestamp + 1;
			sample[1].temperature = temperature;
			// 量程+/-4G
			sample[1].accelerate = DeomposeData(data, 4.0f);
			// 量程 +/- 1000度/s
			sample[1].gyro = DeomposeData(data + 6, 1000.f * Mathf::DEG_TO_RAD);
			data += 12;

			// 量程 +/- 0.88 Ga
			Vector3f magValue = DeomposeData(data, 32767.f * 0.00073);
			float y = magValue.z;
			magValue.z = magValue.y;
			magValue.y = y;

			sample[0].magnet = sample[1].magnet = magValue;

			GCalibration.Apply(sample[0]);
			GCalibration.Apply(sample[1]);


			uint64_t timeStamp = GetTicksNanos();
			UpdateGyroScope(sample[0].gyro, timeStamp);


			sampleCount++;
			if (timeStamp - lastTimeStamp > 1000000000)	{
				float freq = ((sampleCount - lastSampleCount) * 1000000000.f) / (timeStamp - lastTimeStamp);
				lastSampleCount = sampleCount;
				lastTimeStamp = timeStamp;

				GLog.LogInfo("freq %.2f", freq);

				Fancy3DKeepAlive();
			}

		} else {
			GLog.LogError("device type error %d", deviceType);
			break;
		}

	}
	//GLog.LogInfo("read r = %d length %d", r, readLen);
}

static UsbDeviceThread deviceThread;



#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
struct usbfs_ctrltransfer {
	/* keep in sync with usbdevice_fs.h:usbdevfs_ctrltransfer */
	uint8_t  bmRequestType;
	uint8_t  bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;

	uint32_t timeout;	/* in milliseconds */

	/* pointer to data */
	void *data;
};

struct usbfs_bulktransfer {
	/* keep in sync with usbdevice_fs.h:usbdevfs_bulktransfer */
	unsigned int ep;
	unsigned int len;
	unsigned int timeout;	/* in milliseconds */

	/* pointer to data */
	void *data;
};



#define IOCTL_USBFS_CONTROL	_IOWR('U', 0, struct usbfs_ctrltransfer)
#define IOCTL_USBFS_BULK		_IOWR('U', 2, struct usbfs_bulktransfer)

void LogSerialString()
{
	unsigned char buffer[255];

	const uint8_t DT_STRING = 0x03;
	
	usbfs_ctrltransfer ctrl;
	ctrl.bmRequestType	= 0x80;
	ctrl.bRequest		= 0x06;		// request get descriptor
	ctrl.wValue =		DT_STRING << 8 | 0;	// string descriptor | id
	ctrl.wIndex			= 0;			// language id
	ctrl.data			= buffer;
	ctrl.wLength		= sizeof(buffer);
	ctrl.timeout		= 1000;

	int r = ioctl(devicefd, IOCTL_USBFS_CONTROL, &ctrl);
	if (r < 0) {
		GLog.LogError("ioctl get language id error r = %d errno %d", r, errno);
		return;
	}

	uint16_t langid = buffer[2] | (buffer[3] << 8);
	GLog.LogInfo("langid = %hd", langid);

	ctrl.wValue = DT_STRING << 8 | 0x03;	// serial string
	ctrl.wIndex = langid;

	r = ioctl(devicefd, IOCTL_USBFS_CONTROL, &ctrl);
	if (r < 0) {
		GLog.LogError("ioctl get serial string error r = %d errno %d", r, errno);
		return;
	}

	GLog.LogInfo("read serial string length r = %d len = %d", r, (int)buffer[0]);

	if (buffer[0] > r || buffer[0] > ctrl.wLength) {
		GLog.LogError("ioctl get serial string size error %d", (int)buffer[0]);
		return;
	}

	if (buffer[1] != DT_STRING) {
		GLog.LogError("ioctl get serial string error (buffer[1] != DT_STRING) buffer[1] = %d", (int)buffer[1]);
		return;
	}
	
	// to ascii code
	char data[128];
	int di = 0, si = 2;
	for (; si < buffer[0]; si += 2) {
		if ((buffer[si] & 0x80) || (buffer[si + 1])) /* non-ASCII */
			data[di++] = '?';
		else
			data[di++] = buffer[si];
	}
	data[di] = 0;

	GLog.LogInfo("serial string %s", data);
}




extern "C"
{

	JNIEXPORT void JNICALL Java_com_xvr_aurora_GL2JNILib_Testlibusb(JNIEnv* env, jobject thiz, jint fd, jstring subPath)
	{
		int strlen = env->GetStringUTFLength(subPath);

		const char * uspfs_path = (subPath == NULL) ? "" : env->GetStringUTFChars(subPath, 0);

		devicefd = fd;

		LogSerialString();

		GLog.LogInfo("fd %d len %d path %s", fd, strlen, uspfs_path);

		libUsbTest(fd, uspfs_path);

		if (devHandle != NULL && devEndPoint != -1)	{
			deviceThread.Create();
		}

	}

}


void SuspendSensorThread()
{
	deviceThread.Suspend();
}

void ResumeSensorThread()
{
	deviceThread.Resume();
}

void StopSensorThread()
{
	deviceThread.Stop();
}

