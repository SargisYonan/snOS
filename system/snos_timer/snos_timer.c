#include "snos_timer.h"
#include "snos_alloc.h"
#include "list.h"

static uint8_t __timer_enabled = 0;
static uint8_t __timer_init = 0;
static list_t __timer_list = NULL;

static snOSError _initialize_timers(void) {
	__timer_list = list_create();
	if (__timer_list) {
		return snOS_SUCCESS;
	} else {
		return snOS_SYSTEM_OOM_ERROR;
	}
}

void snos_tick(void) {
	static snOSTimeVector last_time = 0;
	snOSTimeVector this_time = 0;
	snOSTimeVector elapsed_time = 0;
	snOSTimer *_timer = NULL;

	if (__timer_init && __timer_enabled) {
		this_time = snos_get_system_time();
	    // check for timer overflow
	    if (this_time < last_time) {
	        elapsed_time = (SNOS_SYSTEM_TIMER_MAX - last_time) + this_time;
	    } else {
	        elapsed_time = this_time - last_time;
	    }
	}
	list_move_cursor_to_head(__timer_list);
	while (1) {
		_timer = list_get_cursor_data(__timer_list);

		if (_timer->enabled) {
			_timer->elapsed_ms += elapsed_time;
			if (_timer->elapsed_ms >= _timer->duration_ms) {
				_timer->elapsed_time = 0;
				_timer->enabled = 0;
				_timer->timer_callback();
			}
		}

		if (_timer == list_get_tail_data(__timer_list)) {
			break;
		}

		list_move_cursor_right(__timer_list);
	}
}

snOSError snos_enable_system_timer(void) {
	if (__timer_init) {
		__timer_enabled = 1;
		return snOS_SUCCESS;
	} else {
		return snOS_ERROR;
	}
}

snOSError snos_disable_system_timer(void) {
	if (__timer_init) {
		__timer_enabled = 0;
		return snOS_SUCCESS;
	} else {
		return snOS_ERROR;
	}
}

snOSTimer *snos_create_timer(snOSError (*timer_callback)(void), snOSTimeVector duration_ms) {
	snOSTimer *new_timer = NULL;

	if (!__timer_init) {
		if (_initialize_timers() != snOS_SUCCESS) {
			return NULL;
		}
	}

	new_timer = snos_alloc(sizeof(snOSTimer));
	if (new_timer) {
		new_timer->enabled = 0;
		new_timer->duration_ms = duration_ms;
		new_timer->elapsed_ms = 0;
		new_timer->timer_callback = timer_callback;
		
		list_append(__timer_list, new_timer);
	}

	return new_timer;
}

snOSError snos_disable_timer(snOSTimer *timer) {
	if (timer && __timer_init) {
		timer->enabled = 0;
		return snOS_SUCCESS;
	} else {
		snOS_ERROR;
	}
}

snOSError snos_start_timer(snOSTimer *timer) {
	if (timer && __timer_init) {
		timer->enabled = 1;
		timer->elapsed_time = 0;
		return snOS_SUCCESS;
	} else {
		snOS_ERROR;
	}
}

snOSError snos_set_duration(snOSTimer *timer, snOSTimeVector duration_ms) {
	if (timer && __timer_init) {
		timer->duration_ms = 0;
		return snOS_SUCCESS;
	} else {
		snOS_ERROR;
	}
}

snOSError snos_remove_timer(snOSTimer *timer) {
	if (timer) {
		list_move_cursor_to_head(__timer_list);

		while(1) {
			if (timer == list_get_cursor_data(__timer_list)) {
				list_detete_current_item(__timer_list);
				break;
			}

			if (list_get_cursor_data(__timer_list) == list_get_tail_data(__timer_list)) {
				break;
			}

			list_move_cursor_right(__timer_list);
		}

		return snOS_SUCCESS;
	} else {
		snOS_ERROR;
	}
}

snOSError snos_change_callback(snOSTimer *timer, snOSError (*new_timer_callback)(void)) {
	if (timer && __timer_init) {
		timer->timer_callback = new_timer_callback;
		return snOS_SUCCESS;
	} else {
		snOS_ERROR;
	}
}
