#include <jni.h>
#include "libusb.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "MyLog.h"
#include <jni.h>


#if !defined(bool)
#define bool int
#endif
#if !defined(true)
#define true (1 == 1)
#endif
#if !defined(false)
#define false (!true)
#endif

// Future versions of libusb will use usb_interface instead of interface
// in libusb_config_descriptor => catter for that
#define usb_interface interface

// Global variables
static bool binary_dump = false;
static bool extra_info = false;
static bool force_device_request = false;	// For WCID descriptor queries
static const char* binary_name = NULL;

int devicefd = -1;

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
			if ( (conf_desc->usb_interface[i].altsetting[j].bInterfaceClass == LIBUSB_CLASS_MASS_STORAGE)
				&& ( (conf_desc->usb_interface[i].altsetting[j].bInterfaceSubClass == 0x01)
				|| (conf_desc->usb_interface[i].altsetting[j].bInterfaceSubClass == 0x06) )
				&& (conf_desc->usb_interface[i].altsetting[j].bInterfaceProtocol == 0x50) ) {
					// Mass storage devices that can use basic SCSI commands
					//test_mode = USE_SCSI;
			}
			for (k=0; k<conf_desc->usb_interface[i].altsetting[j].bNumEndpoints; k++) {
				struct libusb_ss_endpoint_companion_descriptor *ep_comp = NULL;
				endpoint = &conf_desc->usb_interface[i].altsetting[j].endpoint[k];
				GLog.LogInfo("       endpoint[%d].address: %02X", k, endpoint->bEndpointAddress);
				// Use the first interrupt or bulk IN/OUT endpoints as default for testing
				if ((endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) & (LIBUSB_TRANSFER_TYPE_BULK | LIBUSB_TRANSFER_TYPE_INTERRUPT)) {
					if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN) {
						if (!endpoint_in)
							endpoint_in = endpoint->bEndpointAddress;
					} else {
						if (!endpoint_out)
							endpoint_out = endpoint->bEndpointAddress;
					}
				}
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
	
	uint8_t buffer[128];
	int readLen = 0;
	r = libusb_bulk_transfer(handle, endpoint_in, buffer, 128, &readLen, 0);
	GLog.LogInfo("read r = %d length %d", r, readLen);

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

		if (dd.idVendor == 10291 && dd.idProduct == 1) {
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

extern "C"
{

	JNIEXPORT void JNICALL Java_com_xvr_aurora_GL2JNILib_Testlibusb(JNIEnv* env, jobject thiz, jint fd, jstring subPath)
	{
		int strlen = env->GetStringUTFLength(subPath);

		const char * uspfs_path = (subPath == NULL) ? "" : env->GetStringUTFChars(subPath, 0);

		devicefd = fd;

		GLog.LogInfo("fd %d len %d path %s", fd, strlen, uspfs_path);

		libUsbTest(fd, uspfs_path);
	}

}

