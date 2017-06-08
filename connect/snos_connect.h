// Connect

typedef enum {
	SOH_STATE = 0x00,
	STX_STATE = 0x01,
	DATA_STATE,
	EOT_STATE = 0x04
} PacketReceiverStates_t;

struct _snos_receiving_machine {
	PacketReceiverStates_t receiver_state;
	uint8_t byte_index;
	uint8_t *rx_packet;
	uint8_t inner_state;
};
typedef struct _snos_receiving_machine snOSReceiveMachine;

struct _snos_transceiver {
	uint8_t (*is_byte_available)(void);
	uint8_t (*packet_byte_receiver)(void);
	void (*packet_byte_transmitter)(uint8_t);
	snOSTask *handler;
	uint64_t max_packet_size;
	uint8_t enabled;
	uint8_t is_data_ready;
	uint64_t received_data_size;
	void *data_received;
	snOSReceiveMachine *receive_machine;
};
typedef struct _snos_transceiver snOSTransceiver;

// puts the transceiver in a queue that will be executed in a stack
snOSTransceiver *snos_connect_initialize_channel(
	snOSTask *handler, 
	uint8_t (*packet_byte_receiver)(void), 
	void (*packet_byte_transmitter)(uint8_t)
	);

snOSError snos_connect_send_packet(
	snOSTransceiver *channel, 
	uint8_t *data_ptr, 
	uint64_t size
	);

snOSError snos_connect_start(snOSTransceiver *channel);
snOSError snos_connect_stop(snOSTransceiver *channel);

// returns number of available bytes
uint64_t snos_conenct_is_packet_available(snOSTransceiver *channel);
snOSError snos_connect_get_packet(snOSTransceiver *channel, void *data)
snOSError snos_connect_post_to_task(snOSTransceiver *channel);
// runs every transceiver's receive function
snOSError snos_receiver(void);