#ifndef __USB_DEF_H
#define __USB_DEF_H

typedef enum _RECIPIENT_TYPE {
	DEVICE_RECIPIENT,
	INTERFACE_RECIPIENT,
	ENDPOINT_RECIPIENT,
	OTHER_RECIPIENT
} RECIPIENT_TYPE;

typedef enum _STANDARD_REQUESTS {
	GET_STATUS = 0,
	CLEAR_FEATURE,
	RESERVED1,
	SET_FEATURE,
	RESERVED2,
	SET_ADDRESS,
	GET_DESCRIPTOR,
	SET_DESCRIPTOR,
	GET_CONFIGURATION,
	SET_CONFIGURATION,
	GET_INTERFACE,
	SET_INTERFACE,
	TOTAL_sREQUEST,  /* Total number of Standard request */
	SYNCH_FRAME = 12
} STANDARD_REQUESTS;

/* Definition of "USBwValue" */
typedef enum _DESCRIPTOR_TYPE {
	DEVICE_DESCRIPTOR = 1,
	CONFIG_DESCRIPTOR,
	STRING_DESCRIPTOR,
	INTERFACE_DESCRIPTOR,
	ENDPOINT_DESCRIPTOR,
	DEVICE_QUALIFIER
} DESCRIPTOR_TYPE;

/* Feature selector of a SET_FEATURE or CLEAR_FEATURE */
typedef enum _FEATURE_SELECTOR {
	ENDPOINT_STALL,
	DEVICE_REMOTE_WAKEUP
} FEATURE_SELECTOR;


#define REQUEST_DIRECTION			0x80
#define REQUEST_TYPE				0x60
#define REQUEST_RECIPIENT			0x1F

#define TO_DEVICE_DIRECTION_TYPE	(0x0 << 7)
#define TO_HOST_DIRECTION_TYPE		(0x1 << 7)

#define STANDARD_REQUEST_TYPE		(0x0 << 5)
#define CLASS_REQUEST_TYPE			(0x1 << 5)
#define VENDOR_REQUEST_TYPE			(0x2 << 5)
#define RESERVED_REQUEST_TYPE		(0x3 << 5)

#define DEVICE_RECIPIENT_TYPE		0x0
#define INTERFACE_RECIPIENT_TYPE	0x1
#define ENDPOINT_RECIPIENT_TYPE		0x2
#define OTHER_RECIPIENT_TYPE		0x3

#endif /* __USB_DEF_H */
