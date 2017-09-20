#ifndef _SNOS_CONNECT_H_
#define _SNOS_CONNECT_H_

#include "snos_task_manager.h"
#include <stdint.h>
// Connect
#define SOH (0x00)
#define STX (0x01)
#define EOT (0x04)
typedef enum {
	SOH_STATE,
	STX_STATE,
	DATA_STATE,
	EOT_STATE
} PacketReceiverStates_t;

struct _snos_receiving_machine {
	PacketReceiverStates_t receiver_state;
	uint8_t byte_index;
	uint8_t *rx_packet;
	uint8_t inner_state;
};
typedef struct _snos_receiving_machine snOSReceiveMachine;

struct snos_transceiver_s {
	uint8_t (*is_byte_available)(void);
	uint8_t (*packet_byte_receiver)(void);
	void (*packet_byte_transmitter)(uint8_t);
	struct snos_system_task_s *handler;
	uint64_t max_packet_size;
	uint8_t enabled;
	uint8_t is_data_ready;
	uint64_t received_data_size;
	uint8_t *data_received;
	snOSReceiveMachine *receive_machine;
};
typedef struct snos_transceiver_s snOSTransceiver;

// puts the transceiver in a queue that will be executed in a stack
snOSTransceiver *snos_connect_initialize_channel(struct snos_system_task_s *handler, uint8_t (*is_byte_available)(void),uint8_t (*packet_byte_receiver)(void), void (*packet_byte_transmitter)(uint8_t), uint8_t max_packet_size);
snOSError snos_connect_send_packet(snOSTransceiver *channel, uint8_t *data_ptr, uint64_t size);

snOSError snos_connect_start(snOSTransceiver *channel);
snOSError snos_connect_stop(snOSTransceiver *channel);

// returns number of available bytes
uint64_t snos_conenct_is_packet_available(snOSTransceiver *channel);
snOSError snos_connect_get_packet(snOSTransceiver *channel, uint8_t *data, uint64_t n);
snOSError snos_connect_post_to_task(snOSTransceiver *channel);
// runs every transceiver's receive function
snOSError snos_receiver(void);

#endif // _SNOS_CONNECT_H_
