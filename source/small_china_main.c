/**
 * @file	src/main.c
 *
 * @brief	Объявление точки вхождения в C-код.
 *
 * @author	Vasily Yurchenko <vasily.v.yurchenko@yandex.ru>
 */

#include "drivers.h"
#include "config.h"
#include "console.h"
#include "scheduler.h"
#include "spi.h"
#include "communication.h"
#include "max3421e.h"
#include "utils.h"

#include "usb_core.h"

uint8_t		ep_index;
uint16_t	wInterrupt_Mask;
DEVICE_INFO *usb_device_info;
DEVICE_PROP *usb_device_property;
uint16_t	SaveState ;
DEVICE_INFO	device_info;
USER_STANDARD_REQUESTS  *usb_standard_requests;

uint8_t comm_buff[16] = {0,1,2,3,4,5,6,7,8,9,0xA,0xB,0xC,0xD,0xE,0xF};

/** Количество байтов идентификатора SPI Flash. */
#define SPI_ID_BYTES		64
/** Количество байтов в одной странице данных SPI Flash. */
#define SPI_PAGE_SIZE		256

/** Код команды чтения идентификатора SPI Flash. */
#define SPI_CMD_RDID		0x9F
/** Код команды включения записи SPI Flash. */
#define SPI_CMD_WREN		0x06
/** Код команды полного стирания SPI Flash. */
#define SPI_CMD_BE		0x60
/** Код команды чтения статусного регистра SPI Flash. */
#define SPI_CMD_RDSR1		0x05
/** Код команды выключения записи SPI Flash. */
#define SPI_CMD_WRDI		0x04
/** Код команды записи одной страницы данных в SPI Flash. */
#define SPI_CMD_PP		0x02
/** Код команды чтения данных SPI Flash. */
#define SPI_CMD_READ		0x03

#define KEYBOARD_CHANNEL	0x1
#define MOUSE_CHANNEL		0x2

void spi_flash_test(void) {

	uint8_t buffer[SPI_ID_BYTES + 1] = {0,0,0,0};
	size_t i = 0;

	buffer[0] = SPI_CMD_RDID;

	spi_chip_activate();

	for (i = 0; i < sizeof(buffer) / sizeof(buffer[0]); ++i) {

		while (!LL_SPI_IsActiveFlag_TXE(SPI1));
		LL_SPI_TransmitData8(SPI1, buffer[i]);

		while (!LL_SPI_IsActiveFlag_RXNE(SPI1));
		buffer[i] = LL_SPI_ReceiveData8(SPI1);
	}

	spi_chip_deactivate();

	d_print("MC_SPI_FLASH_ID (Hex) ");
	for (i = 1; i < sizeof(buffer) / sizeof(buffer[0]); ++i)
		d_print("%02X ", buffer[i]);
}

void max3421e_chip_activate(uint32_t chip_num)
{
	switch(chip_num) {
		case KEYBOARD_CHANNEL:
			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
//#warning "Implement"
			break;

		case MOUSE_CHANNEL:
//			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
//#warning "Implement"
			break;
	};
}

void max3421e_chip_deactivate(uint32_t chip_num)
{
	switch(chip_num) {
		case KEYBOARD_CHANNEL:
			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
//#warning "Implement"

			break;

		case MOUSE_CHANNEL:
//			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
//#warning "Implement"
			break;
	};
}

void max3421e_rev_print(uint32_t chip_num)
{
	max3421e_chip_activate(chip_num);
	/* TODO: Implement device presence checking based on revision reading */
	d_print("REVISION 0x%02X\r\n", max3421e_rev_read());
	max3421e_chip_deactivate(chip_num);
}

void kb_ms_power_on(void)
{
	max3421e_chip_activate(KEYBOARD_CHANNEL);
	uint8_t iopins1 = max3421e_iopins1_read();
	max3421e_chip_deactivate(KEYBOARD_CHANNEL);

	max3421e_chip_activate(KEYBOARD_CHANNEL);
	max3421e_iopins1_write(iopins1 | IOPINS1_GPOUT1);
	max3421e_chip_deactivate(KEYBOARD_CHANNEL);
}

void usb_std_req_print(const struct std_request *std_req)
{
	d_print("bm_request_type: 0x%02X\r\n", std_req->bm_request_type);
	d_print("b_request: 0x%02X\r\n", std_req->b_request);
	d_print("w_value: 0x%04X\r\n", std_req->w_value);
	d_print("w_index: 0x%04X\r\n", std_req->w_index);
	d_print("w_length: 0x%04X\r\n", std_req->w_length);
}

void usb_dev_descr_print(const struct device_descriptor *dev_descr)
{
	d_print("b_length: 0x%02X\r\n", dev_descr->b_length);
	d_print("b_descriptor_type: 0x%02X\r\n", dev_descr->b_descriptor_type);
	d_print("bcd_usb: 0x%04X\r\n", dev_descr->bcd_usb);
	d_print("b_device_class: 0x%02X\r\n", dev_descr->b_device_class);
	d_print("b_device_sub_class: 0x%02X\r\n", dev_descr->b_device_sub_class);
	d_print("b_device_protocol: 0x%02X\r\n", dev_descr->b_device_protocol);
	d_print("b_max_packet_size: 0x%02X\r\n", dev_descr->b_max_packet_size);
	d_print("id_vendor: 0x%04X\r\n", dev_descr->id_vendor);
	d_print("id_product: 0x%04X\r\n", dev_descr->id_product);
	d_print("bcd_device: 0x%04X\r\n", dev_descr->bcd_device);
	d_print("i_manufacturer: 0x%02X\r\n", dev_descr->i_manufacturer);
	d_print("i_product: 0x%02X\r\n", dev_descr->i_product);
	d_print("i_serial_number: 0x%02X\r\n", dev_descr->i_serial_number);
	d_print("b_num_configurations: 0x%02X\r\n", dev_descr->b_num_configurations);
}

void usb_conf_descr_print(const struct configuration_descriptor *conf_descr)
{
	d_print("b_length: 0x%02X\r\n", conf_descr->b_length);
	d_print("b_descriptor_type: 0x%02X\r\n", conf_descr->b_descriptor_type);
	d_print("w_total_length: 0x%04X\r\n", conf_descr->w_total_length);
	d_print("b_num_interfaces: 0x%02X\r\n", conf_descr->b_num_interfaces);
	d_print("b_configuration_value: 0x%02X\r\n", conf_descr->b_configuration_value);
	d_print("i_configuration: 0x%02X\r\n", conf_descr->i_configuration);
	d_print("bm_attributes: 0x%02X\r\n", conf_descr->bm_attributes);
	d_print("b_max_power: 0x%02X\r\n", conf_descr->b_max_power);
}

void usb_iface_descr_print(const struct interface_descriptor *iface_descr)
{
	d_print("b_length: 0x%02X\r\n", iface_descr->b_length);
	d_print("b_descriptor_type: 0x%02X\r\n", iface_descr->b_descriptor_type);
	d_print("b_interface_number: 0x%02X\r\n", iface_descr->b_interface_number);
	d_print("b_alternate_setting: 0x%02X\r\n", iface_descr->b_alternate_setting);
	d_print("b_num_endpoints: 0x%02X\r\n", iface_descr->b_num_endpoints);
	d_print("b_interface_class: 0x%02X\r\n", iface_descr->b_interface_class);
	d_print("b_interface_subclass: 0x%02X\r\n", iface_descr->b_interface_subclass);
	d_print("b_interface_protocol: 0x%02X\r\n", iface_descr->b_interface_protocol);
	d_print("i_interface: 0x%02X\r\n", iface_descr->i_interface);
}

void usb_endp_descr_print(const struct endpoint_descriptor *endp_descr)
{
	d_print("b_length: 0x%02X\r\n", endp_descr->b_length);
	d_print("b_descriptor_type: 0x%02X\r\n", endp_descr->b_descriptor_type);
	d_print("b_endpoint_address: 0x%02X\r\n", endp_descr->b_endpoint_address);
	d_print("bm_attributes: 0x%02X\r\n", endp_descr->bm_attributes);
	d_print("w_max_packet_size: 0x%02X\r\n", endp_descr->w_max_packet_size);
	d_print("b_interval: 0x%02X\r\n", endp_descr->b_interval);
}

int32_t usb_device_get_dev_descr(uint32_t usb_channel, uint8_t dev_addr, struct device_descriptor *dev_descr)
{
	struct std_request get_dev_descr = {0x80, 0x06, 0x0100, 0x0000, sizeof(struct device_descriptor)};

	int16_t hrslt = max3421e_usb_setup_send(usb_channel, dev_addr, &get_dev_descr);

	if (hrslt < 0) {
		d_print("%s(): SETUP transmission error. HRSLT: 0x%01X\r\n", __func__, -1 * hrslt);
		return -1;
	}

	/* It needs time to process request */
	mdelay(20);

	max3421e_usb_recv_tog_set(usb_channel, 1);

	hrslt = max3421e_usb_bulk_receive(usb_channel, dev_addr, 0, dev_descr, sizeof(struct device_descriptor));

	max3421e_usb_status_out_send(usb_channel, dev_addr);

	if (hrslt < 0) {
		d_print("%s(): BULK-IN transmission error. HRSLT: 0x%01X\r\n", __func__, -1 * hrslt);
		return -2;
	}
	return 0;
}

int32_t usb_device_get_conf_descr(uint32_t usb_channel, uint8_t dev_addr, struct configuration_descriptor *conf_descr)
{
	struct std_request get_conf_descr = {0x80, 0x06, 0x0200, 0x0000, sizeof(struct configuration_descriptor)};

	int16_t hrslt = max3421e_usb_setup_send(usb_channel, dev_addr, &get_conf_descr);

	if (hrslt < 0) {
		d_print("%s() SETUP transmission error. HRSLT: 0x%01X\r\n", __func__, -1 * hrslt);
		return -1;
	}

	/* It needs time to process request */
	mdelay(20);

	max3421e_usb_recv_tog_set(usb_channel, 1);

	hrslt = max3421e_usb_bulk_receive(usb_channel, dev_addr, 0, conf_descr, sizeof(struct configuration_descriptor));

	max3421e_usb_status_out_send(usb_channel, dev_addr);

	if (hrslt < 0) {
		d_print("%s(): BULK-IN transmission error. HRSLT: 0x%01X\r\n", __func__, -1 * hrslt);
		return -2;
	}

	return 0;
}

void usb_device_full_conf_print(uint8_t *full_conf, size_t full_conf_size)
{
	size_t remain_bytes = full_conf_size;
	size_t base = 0;
	struct configuration_descriptor *p_conf_descr;
	struct interface_descriptor *p_iface_descr;
	struct endpoint_descriptor *p_endp_descr;

	while (remain_bytes > 1) {

		switch (full_conf[base + 1]) {

		case 0x2: /* CONFIGURATION_DESCRIPTOR_TYPE */

			p_conf_descr = (struct configuration_descriptor *)(full_conf + base);
			base += sizeof(struct configuration_descriptor);
			remain_bytes -= sizeof(struct configuration_descriptor);

			d_print("Configuration descriptor\r\n");
			usb_conf_descr_print(p_conf_descr);
			mdelay(20);
			break;

		case 0x4: /* INTERFACE_DESCRIPTOR_TYPE */

			p_iface_descr = (struct interface_descriptor *)(full_conf + base);
			base += sizeof(struct interface_descriptor);
			remain_bytes -= sizeof(struct interface_descriptor);

			d_print("Interface descriptor\r\n");
			usb_iface_descr_print(p_iface_descr);
			mdelay(20);
			break;

		case 0x5: /* ENDPOINT_DESCRIPTOR_TYPE */

			p_endp_descr = (struct endpoint_descriptor *)(full_conf + base);
			base += sizeof(struct endpoint_descriptor);
			remain_bytes -= sizeof(struct endpoint_descriptor);

			d_print("Endpoint descriptor\r\n");
			usb_endp_descr_print(p_endp_descr);
			mdelay(20);
			break;

		default:
			base++;
			remain_bytes--;
		};
	}
}

int32_t usb_device_get_full_conf(uint32_t usb_channel, uint8_t dev_addr, uint8_t *full_conf, size_t full_conf_size)
{
	struct std_request get_full_conf = {0x80, 0x06, 0x0200, 0x0000, full_conf_size};

	int16_t ret_val = max3421e_usb_setup_send(usb_channel, dev_addr, &get_full_conf);

	if (ret_val < 0) {
		d_print("%s(): SETUP transmission error. HRSLT: 0x%01X\r\n", __func__, -1 * ret_val);
		return -1;
	}

	/* It needs time to process request */
	mdelay(20);

	max3421e_usb_recv_tog_set(usb_channel, 1);

	ret_val = max3421e_usb_bulk_receive(usb_channel, dev_addr, 0, full_conf, full_conf_size);

	max3421e_usb_status_out_send(usb_channel, dev_addr);

	if (ret_val < 0) {
		d_print("%s(): BULK-IN transmission error. HRSLT: 0x%01X\r\n", __func__, -1 * ret_val);
		return -2;
	}
	return 0;
}

int16_t usb_device_get_conf(uint32_t usb_channel, uint8_t dev_addr)
{
	uint8_t configuration;
	struct std_request get_ep_status = {0x80, 0x08, 0x0000, 0x0000, 1};

	int16_t hrslt = max3421e_usb_setup_send(usb_channel, dev_addr, &get_ep_status);

	if (hrslt < 0) {
		d_print("%s(): SETUP transmission error. HRSLT: 0x%01X\r\n", __func__, -1 * hrslt);
		return -1;
	}

	/* It needs time to process request */
	mdelay(20);

	max3421e_usb_recv_tog_set(usb_channel, 1);

	hrslt = max3421e_usb_bulk_receive(usb_channel, dev_addr, 0, &configuration, get_ep_status.w_length);

	max3421e_usb_status_out_send(usb_channel, dev_addr);

	if (hrslt < 0) {
		d_print("%s(): BULK-IN transmission error. HRSLT: 0x%01X\r\n", __func__, -1 * hrslt);
		return -2;
	}
	return configuration;
}

int32_t usb_device_set_conf(uint32_t usb_channel, uint8_t dev_addr, uint8_t conf)
{
	struct std_request set_conf_req = {0x00, 0x09, conf, 0x0000, 0x0000};

	int16_t hrslt = max3421e_usb_setup_send(usb_channel, dev_addr, &set_conf_req);

	max3421e_usb_status_in_send(usb_channel, dev_addr);

	if (hrslt < 0) {
		d_print("%s(): SETUP transmission error. HRSLT: 0x%01X\r\n", __func__, -1 * hrslt);
		return -1;
	}
	return 0;
}

int32_t usb_device_get_ep_status(uint32_t usb_channel, uint8_t dev_addr, uint8_t ep_addr, uint8_t *status)
{
	struct std_request get_ep_status = {0x82, 0x00, 0x0000, ep_addr, 2};

	int16_t hrslt = max3421e_usb_setup_send(usb_channel, dev_addr, &get_ep_status);

	if (hrslt < 0) {
		d_print("%s(): SETUP transmission error. HRSLT: 0x%01X\r\n", __func__, -1 * hrslt);
		return -1;
	}

	/* It needs time to process request */
	mdelay(20);

	max3421e_usb_recv_tog_set(usb_channel, 1);

	hrslt = max3421e_usb_bulk_receive(usb_channel, dev_addr, 0, status, get_ep_status.w_length);

	max3421e_usb_status_out_send(usb_channel, dev_addr);

	if (hrslt < 0) {
		d_print("%s(): BULK-IN transmission error. HRSLT: 0x%01X\r\n", __func__,  -1 * hrslt);
		return -2;
	}

	return 0;
}

void ms_usb_data_read(uint8_t dev_addr, uint8_t ep_addr)
{
	uint8_t data[4];
	int8_t ret_val;

	do {
		mdelay(100);
		ret_val = max3421e_usb_bulk_receive(KEYBOARD_CHANNEL, dev_addr, ep_addr, data, sizeof(data));
		if ((-1 * ret_val) == 0x04)
			continue;

		if ((-1 * ret_val) == 0x06){
			max3421e_usb_recv_tog_set(KEYBOARD_CHANNEL, 0);
			continue;
		}

		if (ret_val < 0) {
//			d_print("BULK-IN transmission error. HRSLT: 0x%01X\r\n",  -1 * ret_val);
			return;
		}

		/* TODO: Call a function to process the received data */

		size_t idx = 0;
		for (; idx < sizeof(data); ++idx)
			d_print("0x%02X ", data[idx]);
		d_print("\r\n");

	} while(1);
}

void kb_usb_data_read(uint8_t dev_addr, uint8_t ep_addr)
{
	uint8_t data[8];
	int8_t ret_val;

	do {
		mdelay(100);
		ret_val = max3421e_usb_bulk_receive(KEYBOARD_CHANNEL, dev_addr, ep_addr, data, sizeof(data));
		if ((-1 * ret_val) == 0x04)
			continue;

		if ((-1 * ret_val) == 0x06){
			max3421e_usb_recv_tog_set(KEYBOARD_CHANNEL, 0);
			continue;
		}

		if (ret_val < 0) {
//			d_print("BULK-IN transmission error. HRSLT: 0x%01X\r\n",  -1 * ret_val);
			return;
		}

		/* TODO: Call a function to process the received data */

		size_t idx = 0;
		for (; idx < sizeof(data); ++idx)
			d_print("0x%02X ", data[idx]);
		d_print("\r\n");

	} while(1);
}

int32_t kb_detect_init()
{
	kb_ms_power_on();

	max3421e_master_mode_set(KEYBOARD_CHANNEL);

	mdelay(4000);

	/* TODO: Check return value */
	if (max3421e_usb_device_detect(KEYBOARD_CHANNEL) == 0)
		return -1;

	max3421e_usb_sof_start(KEYBOARD_CHANNEL);

	max3421e_usb_bus_reset(KEYBOARD_CHANNEL);

	uint8_t kb_usb_addr = 0x34;

	max3421e_usb_device_set_address(KEYBOARD_CHANNEL, kb_usb_addr);

	mdelay(50);

	/* TODO: Pass pointer to descriptor via argument */
	struct configuration_descriptor conf_descr;
	if (usb_device_get_conf_descr(KEYBOARD_CHANNEL, kb_usb_addr, &conf_descr) != 0)
		return -2;
	usb_conf_descr_print(&conf_descr);

	struct device_descriptor dev_descr;
	if (usb_device_get_dev_descr(KEYBOARD_CHANNEL, kb_usb_addr, &dev_descr) != 0)
		return -3;
	usb_dev_descr_print(&dev_descr);

	/* TODO: Check VID and PID dev_descr */

	mdelay(50);

	uint8_t full_conf[512];
	/* TODO: Pass pointer to descriptor via argument */
	if (usb_device_get_full_conf(KEYBOARD_CHANNEL, kb_usb_addr, full_conf, conf_descr.w_total_length) < 0)
		return -4;
	/* TODO: Print the full configuration for debugging  */
	usb_device_full_conf_print(full_conf, conf_descr.w_total_length);

	mdelay(50);

	/* TODO: Pass pointer to descriptor via argument */
	int16_t conf = usb_device_get_conf(KEYBOARD_CHANNEL, kb_usb_addr);
	if (conf < 0)
		return -5;
	/* TODO: Print the descriptor for debugging  */
	d_print("configuration: 0x%02X\r\n", conf);

	mdelay(50);

	if (usb_device_set_conf(KEYBOARD_CHANNEL, kb_usb_addr, 0x1) != 0)
		return -6;

	mdelay(50);

	/* TODO: Pass pointer to descriptor via argument */
	conf = usb_device_get_conf(KEYBOARD_CHANNEL, kb_usb_addr);
	if (conf < 0)
		return -7;
	/* TODO: Print the descriptor for debugging  */
	d_print("configuration: 0x%02X\r\n", conf);

	mdelay(50);

	/* TODO: Pass pointer to status via argument */
	uint8_t status[2];
	if (usb_device_get_ep_status(KEYBOARD_CHANNEL, kb_usb_addr, 0x01, status) != 0)
		return -8;
	d_print("status[0]: 0x%02X\r\n", status[0]);
	d_print("status[1]: 0x%02X\r\n", status[1]);

	mdelay(50);
	return 0;
}

uint32_t ms_detect_init()
{
	return 0;
}

void spi_usb_test(void)
{
	max3421e_fullduplex_spi_set(KEYBOARD_CHANNEL);

	max3421e_rev_print(KEYBOARD_CHANNEL);

	max3421e_chip_reset(KEYBOARD_CHANNEL);

	int32_t ret_val = kb_detect_init();
	if (ret_val != 0) {
		d_print("kb_detect_init(): %ld\r\n", ret_val);
		return;
	}

	/* TODO: Call the functions in a cycle */
//	ms_usb_data_read(0x34, 0x1);
	/* TODO: Call a function to process the received data */
	kb_usb_data_read(0x34, 0x1);
	/* TODO: Call a function to process the received data */

}

void usb_init(void)
{

	NVIC_SetPriorityGrouping(2);

	NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 2);
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

	NVIC_SetPriority(USBWakeUp_IRQn, 1);
	NVIC_EnableIRQ(USBWakeUp_IRQn);

	usb_device_info = &device_info;
	usb_device_info->control_state = IN_DATA;
	usb_device_property = &property;
	usb_standard_requests = &standard_requests;

//	  EXTI_ClearITPendingBit(EXTI_Line18);
//	  EXTI_InitStructure.EXTI_Line = EXTI_Line18;
//	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	  EXTI_Init(&EXTI_InitStructure);

	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_18);
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_18);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_18);

	usb_device_property->init();
}
/**
 * @brief	C-code entry point.
 *
 * @retval	0 in case of success, otherwise error code
 */
int main(void)
{
	soc_init();

	board_init();

	console_init();

//	scheduler_init();

	spi_init();
//	spi_flash_test();
	spi_usb_test();


//	comm_init(&comm_buff, sizeof(comm_buff));
//	comm_start();

//	usb_init();

//	uint32_t i = 0;
	while (1) {
		LL_mDelay(DELAY_500_MS * 2);
//		print("Main thread iteration %ld\r\n", i++);
		/*ina3221_print_voltage_current();*/
//		LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_0);
		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
//		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_14);
//		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_15);
	}
}
