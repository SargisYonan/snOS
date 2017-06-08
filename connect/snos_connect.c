// Connect
#include <list.h>
#include <snos_alloc.h>

static list_t *transceiver_list = NULL;

// puts the transceiver in a queue that will be executed in a stack
snOSTransceiver *snos_connect_initialize_channel(
	snOSTask *handler, 
	uint8_t (*is_byte_available)(void),
	uint8_t (*packet_byte_receiver)(void), 
	void (*packet_byte_transmitter)(uint8_t),
	uint8_t max_packet_size
	) {
	snOSTransceiver *new_t = NULL;

	if (!transceiver_list) {
		transceiver_list = list_create();
	}

	new_t = snos_alloc(sizeof(snOSTransceiver));
	if (new_t) {
		new_t->is_byte_available = is_byte_available
		new_t->packet_byte_receiver = packet_byte_receiver
		new_t->packet_byte_transmitter = packet_byte_transmitter
		new_t->handler = handler;
		new_t->enabled = 1;
		new_t->is_data_ready = 0;
		new_t->received_data_size = 0;
		new_t->data_received = NULL;
		new_t->max_packet_size = max_packet_size;
		new_t->receive_machine = snos_alloc(sizeof(snOSReceiveMachine));
		if (new_t->receive_machine) { 
			new_t->receive_machine->receiver_state = SOH_STATE;
			new_t->receive_machine->byte_index = 0;
			new_t->receive_machine->inner_state = 0;
			new_t->receive_machine->rx_packet = snos_alloc(max_packet_size);
		}
		list_append(transceiver_list, new_t);
	}
	return snOSTransceiver;
}

snOSError snos_connect_send_packet(
	snOSTransceiver *channel, 
	uint8_t *data_ptr, 
	uint64_t size
	) {
	uint64_t itor = 0;
	snOSError ret = snOS_SUCCESS;
	if (channel) {
		if (!(channel->enabled)) {
			return snOS_CHANNEL_DISABLED;
		}
		for(itor = 0; itor < size; itor++) {
			channel->packet_byte_transmitter(data_ptr[itor]);
		}
	}
	return ret;
}

snOSError snos_connect_start(snOSTransceiver *channel) {
	snOSError ret = snOS_ERROR;
	if (channel) {
		channel->enabled = 1;
		ret = snOS_SUCCESS;
	}
	return ret;
}

snOSError snos_connect_stop(snOSTransceiver *channel) {
	snOSError ret = SNOS_ERROR;
	if (channel) {
		channel->enabled = 0;
		ret = SNOS_SUCCESS;
	}
	return ret;
}

// returns number of available bytes
uint64_t snos_conenct_is_packet_available(snOSTransceiver *channel) {
	if (channel) {
		if (channel->is_data_ready) {
			return channel->received_data_size;
		}
	}
	return 0;
}

snOSError snos_connect_get_packet(snOSTransceiver *channel, void *data) {
	if (channel) {
		data = channel->data_received;
	}
}

snOSError snos_connect_post_to_task(snOSTransceiver *channel) {
	return snos_task_set_request(channel->handler);
}

// runs every transceiver's receive function in the list
// in an FSM
snOSError snos_receiver(void) {
	snOSTransceiver *this_rx = NULL;

	list_move_cursor_to_head(transceiver_list);

	while(1) {
		list_get_cursor_data(transceiver_list, this_rx);

		if (this_rx->is_byte_available() && this_rx->enabled) {
			
			uint8_t byte_in = this_rx->packet_byte_receiver();

			switch (this_rx->receive_machine->receiver_state) {
				case SOH_STATE:
					data_size = 0;
					if (byte_in == (uint8_t)SOH_STATE) {
						this_rx->receive_machine->receiver_state = STX_STATE;
					}
					break;

				case STX_STATE:
					if (byte_in == (uint8_t)STX_STATE) {
						this_rx->receive_machine->receiver_state = DATA_STATE;
					} else {
						this_rx->receive_machine->receiver_state = SOH_STATE;
					}

					this_rx->receive_machine->inner_state = 0x00;
					break;

				case DATA_STATE:

					this_rx->is_data_ready = 0;
					this_rx->data_received = NULL;
					this_rx->received_data_size = 0;

					if (this_rx->receive_machine->byte_index < (this_rx->max_packet_size)) {

						switch(this_rx->receive_machine->inner_state) {
							case 0xAA:
								if (byte_in == 0xBB) {
									this_rx->receive_machine->inner_state = 0xBB;
								} else {
									this_rx->receive_machine->rx_packet[this_rx->receive_machine->byte_index++] = 0xAA;
									if (this_rx->receive_machine->byte_index < (this_rx->max_packet_size)) {
										this_rx->receive_machine->rx_packet[this_rx->receive_machine->byte_index++] = byte_in;
									}
									this_rx->receive_machine->inner_state = 0x00;
								}
								break;

							case 0xBB:
								if (byte_in == 0xCC) {
									this_rx->receive_machine->inner_state = 0xCC;
								} else {
									if (this_rx->receive_machine->byte_index < (this_rx->max_packet_size)) {
										this_rx->receive_machine->rx_packet[this_rx->receive_machine->byte_index++] = 0xAA;
									}
									if (this_rx->receive_machine->byte_index < (this_rx->max_packet_size)) {
										this_rx->receive_machine->rx_packet[this_rx->receive_machine->byte_index++] = 0xBB;
									}
									if (this_rx->receive_machine->byte_index < (this_rx->max_packet_size)) {
										this_rx->receive_machine->rx_packet[this_rx->receive_machine->byte_index++] = byte_in;
									}
									this_rx->receive_machine->inner_state = 0x00;
								}
								break;

							case 0xCC:
								if (byte_in == (uint8_t)EOT_STATE) {
									this_rx->receive_machine->receiver_state = EOT_STATE;
								} else {
									if (this_rx->receive_machine->byte_index < (this_rx->max_packet_size)) {
										this_rx->receive_machine->rx_packet[this_rx->receive_machine->byte_index++] = 0xAA;
									}
									if (this_rx->receive_machine->byte_index < (this_rx->max_packet_size)) {
										this_rx->receive_machine->rx_packet[this_rx->receive_machine->byte_index++] = 0xBB;
									}
									this_rx->receive_machine->rx_packet[this_rx->receive_machine->byte_index++] = 0xCC;
									if (this_rx->receive_machine->byte_index < (this_rx->max_packet_size)) {
										this_rx->receive_machine->rx_packet[this_rx->receive_machine->byte_index++] = byte_in;
									}
									this_rx->receive_machine->inner_state = 0x00;
								}
								break;

							default:
								if (byte_in == 0xAA) {
									this_rx->receive_machine->inner_state = 0xAA;
								} else {
									this_rx->receive_machine->rx_packet[this_rx->receive_machine->byte_index++] = byte_in;
								}
								break;
						}

						
					} else {
						if (byte_in == (uint8_t)EOT_STATE) {
							this_rx->receive_machine->receiver_state = EOT_STATE;
						}
					}
					break;

				case EOT_STATE:
						this_rx->data_received = (void*)(this_rx->receiver_machine->rx_buffer);
						this_rx->is_data_ready = 1;
						this_rx->received_data_size = this_rx->receive_machine->byte_index;
						snos_connect_post_to_task(this_rx);

						// reset buffer index
						this_rx->receive_machine->byte_index = 0;
						this_rx->receive_machine->inner_state = 0x00;
						this_rx->receive_machine->receiver_state = SOH_STATE;
					break;

				default:
					this_rx->receive_machine->receiver_state = SOH_STATE;
					break;
			} 
		}

		if (this_rx == list_get_tail_data(transceiver_list)) {
			break;
		} else {
			list_move_cursor_right(transceiver_list);
			list_get_cursor_data(transceiver_list, this_rx);
		}
	}
}
