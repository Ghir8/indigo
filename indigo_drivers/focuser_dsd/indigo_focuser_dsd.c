// Copyright (C) 2019 Rumen G. Bogdanovski
// All rights reserved.
//
// You can use this software under the terms of 'INDIGO Astronomy
// open-source license' (see LICENSE.md).
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHORS 'AS IS' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// version history
// 2.0 by Rumen G. Bogdanovski

/** INDIGO DSD focuser driver
 \file indigo_focuser_dsd.c
 */

#define DRIVER_VERSION 0x0001
#define DRIVER_NAME "indigo_focuser_dsd"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>

#if defined(INDIGO_FREEBSD)
#include <libusb.h>
#else
#include <libusb-1.0/libusb.h>
#endif

#include "indigo_driver_xml.h"
#include "indigo_focuser_dsd.h"

#define dsd_VENDOR_ID                   0x03c3

#define PRIVATE_DATA                    ((dsd_private_data *)device->private_data)

#define EAF_BEEP_PROPERTY               (PRIVATE_DATA->beep_property)
#define EAF_BEEP_ON_ITEM                (EAF_BEEP_PROPERTY->items+0)
#define EAF_BEEP_OFF_ITEM               (EAF_BEEP_PROPERTY->items+1)
#define EAF_BEEP_PROPERTY_NAME          "EAF_BEEP_ON_MOVE"
#define EAF_BEEP_ON_ITEM_NAME           "ON"
#define EAF_BEEP_OFF_ITEM_NAME          "OFF"


// gp_bits is used as boolean
#define is_connected                    gp_bits

typedef struct {
	int dev_id;
	//EAF_INFO info;
	int current_position, target_position, max_position, backlash;
	double prev_temp;
	indigo_timer *focuser_timer, *temperature_timer;
	pthread_mutex_t serial_mutex;
	indigo_property *beep_property;
} dsd_private_data;

static void compensate_focus(indigo_device *device, double new_temp);

// -------------------------------------------------------------------------------- INDIGO focuser device implementation
static void focuser_timer_callback(indigo_device *device) {
	bool moving, moving_HC;
	pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
	/*
	int res = EAFGetPosition(PRIVATE_DATA->dev_id, &(PRIVATE_DATA->current_position));
	if (res != EAF_SUCCESS) {
		INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetPosition(%d) = %d", PRIVATE_DATA->dev_id, res);
		FOCUSER_POSITION_PROPERTY->state = INDIGO_ALERT_STATE;
		FOCUSER_STEPS_PROPERTY->state = INDIGO_ALERT_STATE;
	}
	INDIGO_DRIVER_DEBUG(DRIVER_NAME, "EAFGetPosition(%d, -> %d) = %d", PRIVATE_DATA->dev_id, PRIVATE_DATA->current_position, res);

	res = EAFIsMoving(PRIVATE_DATA->dev_id, &moving, &moving_HC);
	if (res != EAF_SUCCESS) {
		INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFIsMoving(%d) = %d", PRIVATE_DATA->dev_id, res);
		FOCUSER_POSITION_PROPERTY->state = INDIGO_ALERT_STATE;
		FOCUSER_STEPS_PROPERTY->state = INDIGO_ALERT_STATE;
	}
	*/
	pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
	FOCUSER_POSITION_ITEM->number.value = PRIVATE_DATA->current_position;
	if ((!moving) || (PRIVATE_DATA->current_position == PRIVATE_DATA->target_position)) {
		FOCUSER_POSITION_PROPERTY->state = INDIGO_OK_STATE;
		FOCUSER_STEPS_PROPERTY->state = INDIGO_OK_STATE;
	} else {
		indigo_reschedule_timer(device, 0.5, &(PRIVATE_DATA->focuser_timer));
	}
	indigo_update_property(device, FOCUSER_STEPS_PROPERTY, NULL);
	indigo_update_property(device, FOCUSER_POSITION_PROPERTY, NULL);
}


static void temperature_timer_callback(indigo_device *device) {
	float temp;
	static bool has_sensor = true;
	static bool first_call = true;
	bool has_handcontrol;
	bool moving = false, moving_HC = false;

	/*
	pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
	int res = EAFIsHandControl(PRIVATE_DATA->dev_id, &has_handcontrol);
	pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
	if (res != EAF_SUCCESS) {
		INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFIsHandControl(%d) = %d", PRIVATE_DATA->dev_id, res);
	} else {
		INDIGO_DRIVER_DEBUG(DRIVER_NAME, "EAFIsHandControl(%d, <- %d) = %d", PRIVATE_DATA->dev_id, has_handcontrol, res);
	}

	// Update temerature at first call even if HC is connected.
	//   We want alert sate and T=-273 if the sensor is not connected.
	if (first_call) has_handcontrol = false;

	if (has_handcontrol) {
		pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
		res = EAFGetPosition(PRIVATE_DATA->dev_id, &(PRIVATE_DATA->current_position));
		//fprintf(stderr, "pos=%d\n", PRIVATE_DATA->current_position);
		pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
		if (res != EAF_SUCCESS) {
			INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetPosition(%d, -> %d) = %d", PRIVATE_DATA->dev_id, PRIVATE_DATA->current_position, res);
		} else if (FOCUSER_POSITION_ITEM->number.value != PRIVATE_DATA->current_position) {
			INDIGO_DRIVER_LOG(DRIVER_NAME, "EAFGetPosition(%d, -> %d) = %d", PRIVATE_DATA->dev_id, PRIVATE_DATA->current_position, res);
			FOCUSER_POSITION_PROPERTY->state = INDIGO_OK_STATE;
			pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
			EAFIsMoving(PRIVATE_DATA->dev_id, &moving, &moving_HC);
			pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
			if (moving) {
				FOCUSER_POSITION_PROPERTY->state = INDIGO_BUSY_STATE;
			}
			FOCUSER_POSITION_ITEM->number.value = (double)PRIVATE_DATA->current_position;
			indigo_update_property(device, FOCUSER_POSITION_PROPERTY, NULL);
		}
	} else { // No Hand control, this does not guarantee that we have temperature sensor
		first_call = false;
		FOCUSER_TEMPERATURE_PROPERTY->state = INDIGO_OK_STATE;
		pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
		res = EAFGetTemp(PRIVATE_DATA->dev_id, &temp);
		FOCUSER_TEMPERATURE_ITEM->number.value = (double)temp;
		pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
		if (res != EAF_SUCCESS) {
			INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetTemp(%d, -> %f) = %d", PRIVATE_DATA->dev_id, FOCUSER_TEMPERATURE_ITEM->number.value, res);
			FOCUSER_TEMPERATURE_PROPERTY->state = INDIGO_ALERT_STATE;
		} else {
			INDIGO_DRIVER_DEBUG(DRIVER_NAME, "EAFGetTemp(%d, -> %f) = %d", PRIVATE_DATA->dev_id, FOCUSER_TEMPERATURE_ITEM->number.value, res);
		}
		// static double ctemp = 0;
		// FOCUSER_TEMPERATURE_ITEM->number.value = ctemp;
		// temp = ctemp;
		// ctemp += 0.12;
		if (FOCUSER_TEMPERATURE_ITEM->number.value < -270.0) {
			FOCUSER_TEMPERATURE_PROPERTY->state = INDIGO_ALERT_STATE;
			if (has_sensor) {
				INDIGO_DRIVER_LOG(DRIVER_NAME, "The temperature sensor is not connected.");
				indigo_update_property(device, FOCUSER_TEMPERATURE_PROPERTY, "The temperature sensor is not connected.");
				has_sensor = false;
			}
		} else {
			has_sensor = true;
			indigo_update_property(device, FOCUSER_TEMPERATURE_PROPERTY, NULL);
		}
		if (FOCUSER_MODE_AUTOMATIC_ITEM->sw.value) {
			compensate_focus(device, temp);
		} else {
			PRIVATE_DATA->prev_temp = -273;
		}
	}
	*/
	indigo_reschedule_timer(device, 2, &(PRIVATE_DATA->temperature_timer));
}


static void compensate_focus(indigo_device *device, double new_temp) {
	int compensation;
	double temp_difference = new_temp - PRIVATE_DATA->prev_temp;

	/* we do not have previous temperature reading */
	if (PRIVATE_DATA->prev_temp < -270) {
		INDIGO_DRIVER_DEBUG(DRIVER_NAME, "Not compensating: PRIVATE_DATA->prev_temp = %f", PRIVATE_DATA->prev_temp);
		PRIVATE_DATA->prev_temp = new_temp;
		return;
	}

	/* we do not have current temperature reading or focuser is moving */
	if ((new_temp < -270) || (FOCUSER_POSITION_PROPERTY->state != INDIGO_OK_STATE)) {
		INDIGO_DRIVER_DEBUG(DRIVER_NAME, "Not compensating: new_temp = %f, FOCUSER_POSITION_PROPERTY->state = %d", new_temp, FOCUSER_POSITION_PROPERTY->state);
		return;
	}

	/* temperature difference if more than 1 degree so compensation needed */
	if ((abs(temp_difference) >= 1.0) && (abs(temp_difference) < 100)) {
		compensation = (int)(temp_difference * FOCUSER_COMPENSATION_ITEM->number.value);
		INDIGO_DRIVER_DEBUG(DRIVER_NAME, "Compensation: temp_difference = %.2f, Compensation = %d, steps/degC = %.1f", temp_difference, compensation, FOCUSER_COMPENSATION_ITEM->number.value);
	} else {
		INDIGO_DRIVER_DEBUG(DRIVER_NAME, "Not compensating (not needed): temp_difference = %f", temp_difference);
		return;
	}

	PRIVATE_DATA->target_position = PRIVATE_DATA->current_position + compensation;
	INDIGO_DRIVER_DEBUG(DRIVER_NAME, "Compensation: PRIVATE_DATA->current_position = %d, PRIVATE_DATA->target_position = %d", PRIVATE_DATA->current_position, PRIVATE_DATA->target_position);

	/*
	pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
	int res = EAFGetPosition(PRIVATE_DATA->dev_id, &PRIVATE_DATA->current_position);
	if (res != EAF_SUCCESS) {
		INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetPosition(%d) = %d", PRIVATE_DATA->dev_id, res);
	}
	pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
	*/
	/* Make sure we do not attempt to go beyond the limits */
	if (FOCUSER_POSITION_ITEM->number.max < PRIVATE_DATA->target_position) {
		PRIVATE_DATA->target_position = FOCUSER_POSITION_ITEM->number.max;
	} else if (FOCUSER_POSITION_ITEM->number.min > PRIVATE_DATA->target_position) {
		PRIVATE_DATA->target_position = FOCUSER_POSITION_ITEM->number.min;
	}
	INDIGO_DRIVER_DEBUG(DRIVER_NAME, "Compensating: Corrected PRIVATE_DATA->target_position = %d", PRIVATE_DATA->target_position);

	/*
	pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
	res = EAFMove(PRIVATE_DATA->dev_id, PRIVATE_DATA->target_position);
	if (res != EAF_SUCCESS) {
		INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFMove(%d, %d) = %d", PRIVATE_DATA->dev_id, PRIVATE_DATA->target_position, res);
		FOCUSER_STEPS_PROPERTY->state = INDIGO_ALERT_STATE;
	}
	pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
	*/
	PRIVATE_DATA->prev_temp = new_temp;
	FOCUSER_POSITION_ITEM->number.value = PRIVATE_DATA->current_position;
	FOCUSER_POSITION_PROPERTY->state = INDIGO_BUSY_STATE;
	indigo_update_property(device, FOCUSER_POSITION_PROPERTY, NULL);
	PRIVATE_DATA->focuser_timer = indigo_set_timer(device, 0.5, focuser_timer_callback);
}


static indigo_result eaf_enumerate_properties(indigo_device *device, indigo_client *client, indigo_property *property) {
	if (IS_CONNECTED) {
		if (indigo_property_match(EAF_BEEP_PROPERTY, property))
			indigo_define_property(device, EAF_BEEP_PROPERTY, NULL);
	}
	return indigo_focuser_enumerate_properties(device, NULL, NULL);
}


static indigo_result focuser_attach(indigo_device *device) {
	assert(device != NULL);
	assert(PRIVATE_DATA != NULL);
	if (indigo_focuser_attach(device, DRIVER_VERSION) == INDIGO_OK) {
		pthread_mutex_init(&PRIVATE_DATA->serial_mutex, NULL);
		// -------------------------------------------------------------------------------- SIMULATION
		SIMULATION_PROPERTY->hidden = true;
		// -------------------------------------------------------------------------------- DEVICE_PORT
		DEVICE_PORT_PROPERTY->hidden = false;
		// -------------------------------------------------------------------------------- DEVICE_PORTS
		DEVICE_PORTS_PROPERTY->hidden = false;
		// --------------------------------------------------------------------------------

		FOCUSER_LIMITS_PROPERTY->hidden = false;
		FOCUSER_LIMITS_MAX_POSITION_ITEM->number.min = 0;
		//FOCUSER_LIMITS_MAX_POSITION_ITEM->number.max = PRIVATE_DATA->info.MaxStep;
		FOCUSER_LIMITS_MIN_POSITION_ITEM->number.min = 0;
		FOCUSER_LIMITS_MIN_POSITION_ITEM->number.value = 0;
		FOCUSER_LIMITS_MIN_POSITION_ITEM->number.max = 0;
		//INDIGO_DRIVER_DEBUG(DRIVER_NAME, "\'%s\' MaxStep = %d",device->name ,PRIVATE_DATA->info.MaxStep);

		FOCUSER_SPEED_PROPERTY->hidden = true;

		FOCUSER_BACKLASH_PROPERTY->hidden = false;
		FOCUSER_BACKLASH_ITEM->number.min = 0;
		FOCUSER_BACKLASH_ITEM->number.max = 10000;
		FOCUSER_BACKLASH_ITEM->number.step = 1;

		FOCUSER_POSITION_ITEM->number.min = 0;
		FOCUSER_POSITION_ITEM->number.step = 1;
		//FOCUSER_POSITION_ITEM->number.max = PRIVATE_DATA->info.MaxStep;

		FOCUSER_STEPS_ITEM->number.min = 0;
		FOCUSER_STEPS_ITEM->number.step = 1;
		//FOCUSER_STEPS_ITEM->number.max = PRIVATE_DATA->info.MaxStep;

		FOCUSER_ON_POSITION_SET_PROPERTY->hidden = false;
		FOCUSER_TEMPERATURE_PROPERTY->hidden = false;
		FOCUSER_REVERSE_MOTION_PROPERTY->hidden = false;

		// -------------------------------------------------------------------------- FOCUSER_COMPENSATION
		FOCUSER_COMPENSATION_PROPERTY->hidden = false;
		FOCUSER_COMPENSATION_ITEM->number.min = -10000;
		FOCUSER_COMPENSATION_ITEM->number.max = 10000;
		// -------------------------------------------------------------------------- FOCUSER_MODE
		FOCUSER_MODE_PROPERTY->hidden = false;
		// -------------------------------------------------------------------------- BEEP_PROPERTY
		EAF_BEEP_PROPERTY = indigo_init_switch_property(NULL, device->name, EAF_BEEP_PROPERTY_NAME, "Advanced", "Beep on move", INDIGO_OK_STATE, INDIGO_RW_PERM, INDIGO_ONE_OF_MANY_RULE, 2);
		if (EAF_BEEP_PROPERTY == NULL)
			return INDIGO_FAILED;

		indigo_init_switch_item(EAF_BEEP_ON_ITEM, EAF_BEEP_ON_ITEM_NAME, "On", false);
		indigo_init_switch_item(EAF_BEEP_OFF_ITEM, EAF_BEEP_OFF_ITEM_NAME, "Off", true);
		// --------------------------------------------------------------------------
		INDIGO_DEVICE_ATTACH_LOG(DRIVER_NAME, device->name);
		return indigo_focuser_enumerate_properties(device, NULL, NULL);
	}
	return INDIGO_FAILED;
}


static indigo_result focuser_change_property(indigo_device *device, indigo_client *client, indigo_property *property) {
	assert(device != NULL);
	assert(DEVICE_CONTEXT != NULL);
	assert(property != NULL);
	if (indigo_property_match(CONNECTION_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- CONNECTION
		indigo_property_copy_values(CONNECTION_PROPERTY, property, false);

		if (CONNECTION_CONNECTED_ITEM->sw.value) {
			if (!device->is_connected) {
				pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
				if (indigo_try_global_lock(device) != INDIGO_OK) {
					pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
					INDIGO_DRIVER_ERROR(DRIVER_NAME, "indigo_try_global_lock(): failed to get lock.");
					CONNECTION_PROPERTY->state = INDIGO_ALERT_STATE;
					indigo_set_switch(CONNECTION_PROPERTY, CONNECTION_DISCONNECTED_ITEM, true);
					indigo_update_property(device, CONNECTION_PROPERTY, NULL);
				} else {
				/*
					EAFGetID(index, &(PRIVATE_DATA->dev_id));
					int res = EAFOpen(PRIVATE_DATA->dev_id);
					INDIGO_DRIVER_DEBUG(DRIVER_NAME, "EAFOpen(%d) = %d", PRIVATE_DATA->dev_id, res);
					pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
					if (!res) {
						pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
						res = EAFGetMaxStep(PRIVATE_DATA->dev_id, &(PRIVATE_DATA->max_position));
						if (res != EAF_SUCCESS) {
							INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetMaxStep(%d) = %d", PRIVATE_DATA->dev_id, res);
						}
						FOCUSER_LIMITS_MAX_POSITION_ITEM->number.value = (double)PRIVATE_DATA->max_position;

						res = EAFGetBacklash(PRIVATE_DATA->dev_id, &(PRIVATE_DATA->backlash));
						if (res != EAF_SUCCESS) {
							INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetBacklash(%d) = %d", PRIVATE_DATA->dev_id, res);
						}
						FOCUSER_BACKLASH_ITEM->number.value = (double)PRIVATE_DATA->backlash;

						res = EAFGetPosition(PRIVATE_DATA->dev_id, &(PRIVATE_DATA->target_position));
						if (res != EAF_SUCCESS) {
							INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetPosition(%d, -> %d) = %d", PRIVATE_DATA->dev_id, PRIVATE_DATA->target_position, res);
						}
						FOCUSER_POSITION_ITEM->number.value = PRIVATE_DATA->target_position;

						res = EAFGetReverse(PRIVATE_DATA->dev_id, &(FOCUSER_REVERSE_MOTION_ENABLED_ITEM->sw.value));
						if (res != EAF_SUCCESS) {
							INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetReverse(%d, -> %d) = %d", PRIVATE_DATA->dev_id, FOCUSER_REVERSE_MOTION_ENABLED_ITEM->sw.value, res);
						}
						FOCUSER_REVERSE_MOTION_DISABLED_ITEM->sw.value = !FOCUSER_REVERSE_MOTION_ENABLED_ITEM->sw.value;

						res = EAFGetBeep(PRIVATE_DATA->dev_id, &(EAF_BEEP_ON_ITEM->sw.value));
						if (res != EAF_SUCCESS) {
							INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetBeep(%d, -> %d) = %d", PRIVATE_DATA->dev_id, EAF_BEEP_ON_ITEM->sw.value, res);
						}
						EAF_BEEP_OFF_ITEM->sw.value = !EAF_BEEP_ON_ITEM->sw.value;
						pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);

						CONNECTION_PROPERTY->state = INDIGO_OK_STATE;

						indigo_define_property(device, EAF_BEEP_PROPERTY, NULL);
						PRIVATE_DATA->prev_temp = -273;
						device->is_connected = true;
						PRIVATE_DATA->focuser_timer = indigo_set_timer(device, 0.5, focuser_timer_callback);
						PRIVATE_DATA->temperature_timer = indigo_set_timer(device, 0.1, temperature_timer_callback);
					} else {
						INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFOpen(%d) = %d", index, res);
						CONNECTION_PROPERTY->state = INDIGO_ALERT_STATE;
						indigo_set_switch(CONNECTION_PROPERTY, CONNECTION_DISCONNECTED_ITEM, true);
						indigo_update_property(device, CONNECTION_PROPERTY, NULL);
					}
				*/
				}
			}
		} else {
			if (device->is_connected) {
				indigo_cancel_timer(device, &PRIVATE_DATA->focuser_timer);
				indigo_cancel_timer(device, &PRIVATE_DATA->temperature_timer);
				indigo_delete_property(device, EAF_BEEP_PROPERTY, NULL);
				pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
				/*
				int res = EAFClose(PRIVATE_DATA->dev_id);
				if (res != EAF_SUCCESS) {
					INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFClose(%d) = %d", PRIVATE_DATA->dev_id, res);
				} else {
					INDIGO_DRIVER_DEBUG(DRIVER_NAME, "EAFClose(%d) = %d", PRIVATE_DATA->dev_id, res);
				}
				res = EAFGetID(index, &(PRIVATE_DATA->dev_id));
				if (res != EAF_SUCCESS) {
					INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetID(%d, -> %d) = %d", index, PRIVATE_DATA->dev_id, res);
				} else {
					INDIGO_DRIVER_DEBUG(DRIVER_NAME, "EAFGetID(%d, -> %d) = %d", index, PRIVATE_DATA->dev_id, res);
				}
				*/
				indigo_global_unlock(device);
				pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
				device->is_connected = false;
				CONNECTION_PROPERTY->state = INDIGO_OK_STATE;
			}
		}
	} else if (indigo_property_match(FOCUSER_REVERSE_MOTION_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- FOCUSER_REVERSE_MOTION
		if (!IS_CONNECTED) return INDIGO_OK;
		indigo_property_copy_values(FOCUSER_REVERSE_MOTION_PROPERTY, property, false);
		FOCUSER_REVERSE_MOTION_PROPERTY->state = INDIGO_OK_STATE;
		pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
		/*
		int res = EAFSetReverse(PRIVATE_DATA->dev_id, FOCUSER_REVERSE_MOTION_ENABLED_ITEM->sw.value);
		if (res != EAF_SUCCESS) {
			INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFSetReverse(%d, %d) = %d", PRIVATE_DATA->dev_id, FOCUSER_REVERSE_MOTION_ENABLED_ITEM->sw.value, res);
			FOCUSER_REVERSE_MOTION_PROPERTY->state = INDIGO_ALERT_STATE;
		}
		*/
		pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
		indigo_update_property(device, FOCUSER_REVERSE_MOTION_PROPERTY, NULL);
		return INDIGO_OK;
	} else if (indigo_property_match(FOCUSER_POSITION_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- FOCUSER_POSITION
		indigo_property_copy_values(FOCUSER_POSITION_PROPERTY, property, false);
		if (FOCUSER_POSITION_ITEM->number.target < 0 || FOCUSER_POSITION_ITEM->number.target > FOCUSER_POSITION_ITEM->number.max) {
			FOCUSER_POSITION_PROPERTY->state = INDIGO_ALERT_STATE;
		} else if (FOCUSER_POSITION_ITEM->number.target == PRIVATE_DATA->current_position) {
			FOCUSER_POSITION_PROPERTY->state = INDIGO_OK_STATE;
		} else {
			FOCUSER_POSITION_PROPERTY->state = INDIGO_BUSY_STATE;
			PRIVATE_DATA->target_position = FOCUSER_POSITION_ITEM->number.target;
			FOCUSER_POSITION_ITEM->number.value = PRIVATE_DATA->current_position;
			if (FOCUSER_ON_POSITION_SET_GOTO_ITEM->sw.value) { /* GOTO POSITION */
				FOCUSER_POSITION_PROPERTY->state = INDIGO_BUSY_STATE;
				pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
				/*
				int res = EAFMove(PRIVATE_DATA->dev_id, PRIVATE_DATA->target_position);
				if (res != EAF_SUCCESS) {
					INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFMove(%d, %d) = %d", PRIVATE_DATA->dev_id, PRIVATE_DATA->target_position, res);
					FOCUSER_POSITION_PROPERTY->state = INDIGO_ALERT_STATE;
				}
				*/
				pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
				PRIVATE_DATA->focuser_timer = indigo_set_timer(device, 0.5, focuser_timer_callback);
			} else { /* RESET CURRENT POSITION */
				FOCUSER_POSITION_PROPERTY->state = INDIGO_OK_STATE;
				pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
				/*
				int res = EAFResetPostion(PRIVATE_DATA->dev_id, PRIVATE_DATA->target_position);
				if (res != EAF_SUCCESS) {
					INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFResetPostion(%d, %d) = %d", PRIVATE_DATA->dev_id, PRIVATE_DATA->target_position, res);
					FOCUSER_POSITION_PROPERTY->state = INDIGO_ALERT_STATE;
				}
				res = EAFGetPosition(PRIVATE_DATA->dev_id, &PRIVATE_DATA->current_position);
				FOCUSER_POSITION_ITEM->number.value = PRIVATE_DATA->current_position;
				if (res != EAF_SUCCESS) {
					INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetPosition(%d) = %d", PRIVATE_DATA->dev_id, res);
					FOCUSER_POSITION_PROPERTY->state = INDIGO_ALERT_STATE;
				}
				*/
				pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
			}
		}
		indigo_update_property(device, FOCUSER_POSITION_PROPERTY, NULL);
		return INDIGO_OK;
	} else if (indigo_property_match(FOCUSER_LIMITS_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- FOCUSER_LIMITS
		if (!IS_CONNECTED) return INDIGO_OK;
		indigo_property_copy_values(FOCUSER_LIMITS_PROPERTY, property, false);
		FOCUSER_LIMITS_PROPERTY->state = INDIGO_OK_STATE;
		PRIVATE_DATA->max_position = (int)FOCUSER_LIMITS_MAX_POSITION_ITEM->number.target;
		pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
		/*
		int res = EAFSetMaxStep(PRIVATE_DATA->dev_id, PRIVATE_DATA->max_position);
		INDIGO_DRIVER_DEBUG(DRIVER_NAME, "EAFSetMaxStep(%d, -> %d) = %d", PRIVATE_DATA->dev_id, PRIVATE_DATA->max_position, res);
		if (res != EAF_SUCCESS) {
			INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFSetMaxStep(%d) = %d", PRIVATE_DATA->dev_id, res);
			FOCUSER_LIMITS_PROPERTY->state = INDIGO_ALERT_STATE;
		}
		res = EAFGetMaxStep(PRIVATE_DATA->dev_id, &(PRIVATE_DATA->max_position));
		if (res != EAF_SUCCESS) {
			INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetMaxStep(%d) = %d", PRIVATE_DATA->dev_id, res);
		}
		*/
		FOCUSER_LIMITS_MAX_POSITION_ITEM->number.value = (double)PRIVATE_DATA->max_position;
		pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
		indigo_update_property(device, FOCUSER_LIMITS_PROPERTY, NULL);
		return INDIGO_OK;
	} else if (indigo_property_match(FOCUSER_BACKLASH_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- FOCUSER_BACKLASH
		if (!IS_CONNECTED) return INDIGO_OK;
		indigo_property_copy_values(FOCUSER_BACKLASH_PROPERTY, property, false);
		FOCUSER_BACKLASH_PROPERTY->state = INDIGO_OK_STATE;
		pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
		PRIVATE_DATA->backlash = (int)FOCUSER_BACKLASH_ITEM->number.target;
		/*
		int res = EAFSetBacklash(PRIVATE_DATA->dev_id, PRIVATE_DATA->backlash);
		INDIGO_DRIVER_DEBUG(DRIVER_NAME, "EAFSetBacklash(%d, -> %d) = %d", PRIVATE_DATA->dev_id, PRIVATE_DATA->backlash, res);
		if (res != EAF_SUCCESS) {
			INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFSetBacklash(%d) = %d", PRIVATE_DATA->dev_id, res);
			FOCUSER_BACKLASH_PROPERTY->state = INDIGO_ALERT_STATE;
		}
		res = EAFGetBacklash(PRIVATE_DATA->dev_id, &(PRIVATE_DATA->backlash));
		if (res != EAF_SUCCESS) {
			INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetBacklash(%d) = %d", PRIVATE_DATA->dev_id, res);
		}
		*/
		FOCUSER_BACKLASH_ITEM->number.value = (double)PRIVATE_DATA->backlash;
		pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
		indigo_update_property(device, FOCUSER_BACKLASH_PROPERTY, NULL);
		return INDIGO_OK;
	} else if (indigo_property_match(FOCUSER_STEPS_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- FOCUSER_STEPS
		indigo_property_copy_values(FOCUSER_STEPS_PROPERTY, property, false);
		if (FOCUSER_STEPS_ITEM->number.value < 0 || FOCUSER_STEPS_ITEM->number.value > FOCUSER_STEPS_ITEM->number.max) {
			FOCUSER_STEPS_PROPERTY->state = INDIGO_ALERT_STATE;
		} else {
			FOCUSER_STEPS_PROPERTY->state = INDIGO_BUSY_STATE;
			pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
			/*
			int res = EAFGetPosition(PRIVATE_DATA->dev_id, &PRIVATE_DATA->current_position);
			if (res != EAF_SUCCESS) {
				INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetPosition(%d) = %d", PRIVATE_DATA->dev_id, res);
			}
			if (FOCUSER_DIRECTION_MOVE_INWARD_ITEM->sw.value) {
				PRIVATE_DATA->target_position = PRIVATE_DATA->current_position - FOCUSER_STEPS_ITEM->number.value;
			} else {
				PRIVATE_DATA->target_position = PRIVATE_DATA->current_position + FOCUSER_STEPS_ITEM->number.value;
			}

			// Make sure we do not attempt to go beyond the limits
			if (FOCUSER_POSITION_ITEM->number.max < PRIVATE_DATA->target_position) {
				PRIVATE_DATA->target_position = FOCUSER_POSITION_ITEM->number.max;
			} else if (FOCUSER_POSITION_ITEM->number.min > PRIVATE_DATA->target_position) {
				PRIVATE_DATA->target_position = FOCUSER_POSITION_ITEM->number.min;
			}

			FOCUSER_POSITION_ITEM->number.value = PRIVATE_DATA->current_position;
			res = EAFMove(PRIVATE_DATA->dev_id, PRIVATE_DATA->target_position);
			if (res != EAF_SUCCESS) {
				INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFMove(%d, %d) = %d", PRIVATE_DATA->dev_id, PRIVATE_DATA->target_position, res);
				FOCUSER_STEPS_PROPERTY->state = INDIGO_ALERT_STATE;
			}
			*/
			pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
			PRIVATE_DATA->focuser_timer = indigo_set_timer(device, 0.5, focuser_timer_callback);
		}
		indigo_update_property(device, FOCUSER_STEPS_PROPERTY, NULL);
		return INDIGO_OK;
	} else if (indigo_property_match(FOCUSER_ABORT_MOTION_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- FOCUSER_ABORT_MOTION
		indigo_property_copy_values(FOCUSER_ABORT_MOTION_PROPERTY, property, false);
		FOCUSER_STEPS_PROPERTY->state = INDIGO_OK_STATE;
		FOCUSER_POSITION_PROPERTY->state = INDIGO_OK_STATE;
		FOCUSER_ABORT_MOTION_PROPERTY->state = INDIGO_OK_STATE;
		indigo_cancel_timer(device, &PRIVATE_DATA->focuser_timer);
		pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
		/*
		int res = EAFStop(PRIVATE_DATA->dev_id);
		if (res != EAF_SUCCESS) {
			INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFStop(%d) = %d", PRIVATE_DATA->dev_id, res);
			FOCUSER_ABORT_MOTION_PROPERTY->state = INDIGO_ALERT_STATE;
		}
		res = EAFGetPosition(PRIVATE_DATA->dev_id, &PRIVATE_DATA->current_position);
		if (res != EAF_SUCCESS) {
			INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFGetPosition(%d) = %d", PRIVATE_DATA->dev_id, res);
			FOCUSER_ABORT_MOTION_PROPERTY->state = INDIGO_ALERT_STATE;
		}
		*/
		pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
		FOCUSER_POSITION_ITEM->number.value = PRIVATE_DATA->current_position;
		FOCUSER_ABORT_MOTION_ITEM->sw.value = false;
		indigo_update_property(device, FOCUSER_POSITION_PROPERTY, NULL);
		indigo_update_property(device, FOCUSER_STEPS_PROPERTY, NULL);
		indigo_update_property(device, FOCUSER_ABORT_MOTION_PROPERTY, NULL);
		return INDIGO_OK;
	} else if (indigo_property_match(FOCUSER_COMPENSATION_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- FOCUSER_COMPENSATION_PROPERTY
		indigo_property_copy_values(FOCUSER_COMPENSATION_PROPERTY, property, false);
		FOCUSER_COMPENSATION_PROPERTY->state = INDIGO_OK_STATE;
		if (IS_CONNECTED) {
			indigo_update_property(device, FOCUSER_COMPENSATION_PROPERTY, NULL);
		}
		return INDIGO_OK;
	} else if (indigo_property_match(EAF_BEEP_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- EAF_BEEP_PROPERTY
		if (!IS_CONNECTED) return INDIGO_OK;
		indigo_property_copy_values(EAF_BEEP_PROPERTY, property, false);
		EAF_BEEP_PROPERTY->state = INDIGO_OK_STATE;
		pthread_mutex_lock(&PRIVATE_DATA->serial_mutex);
		/*
		int res = EAFSetBeep(PRIVATE_DATA->dev_id, EAF_BEEP_ON_ITEM->sw.value);
		if (res != EAF_SUCCESS) {
			INDIGO_DRIVER_ERROR(DRIVER_NAME, "EAFSetBeep(%d, %d) = %d", PRIVATE_DATA->dev_id, EAF_BEEP_ON_ITEM->sw.value, res);
			EAF_BEEP_PROPERTY->state = INDIGO_ALERT_STATE;
		}
		*/
		pthread_mutex_unlock(&PRIVATE_DATA->serial_mutex);
		indigo_update_property(device, EAF_BEEP_PROPERTY, NULL);
		return INDIGO_OK;
		// -------------------------------------------------------------------------------- FOCUSER_MODE
	} else if (indigo_property_match(FOCUSER_MODE_PROPERTY, property)) {
		indigo_property_copy_values(FOCUSER_MODE_PROPERTY, property, false);
		if (FOCUSER_MODE_MANUAL_ITEM->sw.value) {
			indigo_define_property(device, FOCUSER_ON_POSITION_SET_PROPERTY, NULL);
			indigo_define_property(device, FOCUSER_SPEED_PROPERTY, NULL);
			indigo_define_property(device, FOCUSER_REVERSE_MOTION_PROPERTY, NULL);
			indigo_define_property(device, FOCUSER_DIRECTION_PROPERTY, NULL);
			indigo_define_property(device, FOCUSER_STEPS_PROPERTY, NULL);
			indigo_define_property(device, FOCUSER_ABORT_MOTION_PROPERTY, NULL);
			indigo_define_property(device, FOCUSER_BACKLASH_PROPERTY, NULL);
			indigo_delete_property(device, FOCUSER_POSITION_PROPERTY, NULL);
			FOCUSER_POSITION_PROPERTY->perm = INDIGO_RW_PERM;
			indigo_define_property(device, FOCUSER_POSITION_PROPERTY, NULL);
		} else {
			indigo_delete_property(device, FOCUSER_ON_POSITION_SET_PROPERTY, NULL);
			indigo_delete_property(device, FOCUSER_SPEED_PROPERTY, NULL);
			indigo_delete_property(device, FOCUSER_REVERSE_MOTION_PROPERTY, NULL);
			indigo_delete_property(device, FOCUSER_DIRECTION_PROPERTY, NULL);
			indigo_delete_property(device, FOCUSER_STEPS_PROPERTY, NULL);
			indigo_delete_property(device, FOCUSER_ABORT_MOTION_PROPERTY, NULL);
			indigo_delete_property(device, FOCUSER_BACKLASH_PROPERTY, NULL);
			indigo_delete_property(device, FOCUSER_POSITION_PROPERTY, NULL);
			FOCUSER_POSITION_PROPERTY->perm = INDIGO_RO_PERM;
			indigo_define_property(device, FOCUSER_POSITION_PROPERTY, NULL);
		}
		FOCUSER_MODE_PROPERTY->state = INDIGO_OK_STATE;
		indigo_update_property(device, FOCUSER_MODE_PROPERTY, NULL);
		return INDIGO_OK;
		// --------------------------------------------------------------------------------
	}
	return indigo_focuser_change_property(device, client, property);
}


static indigo_result focuser_detach(indigo_device *device) {
	assert(device != NULL);
	indigo_device_disconnect(NULL, device->name);
	indigo_release_property(EAF_BEEP_PROPERTY);
	indigo_global_unlock(device);
	INDIGO_DEVICE_DETACH_LOG(DRIVER_NAME, device->name);
	return indigo_focuser_detach(device);
}

// --------------------------------------------------------------------------------
#define MAX_DEVICES 8
static int device_number = 1;
static dsd_private_data *private_data[MAX_DEVICES] = {NULL};
static indigo_device *focuser[MAX_DEVICES] = {NULL};

indigo_result indigo_focuser_dsd(indigo_driver_action action, indigo_driver_info *info) {
	static indigo_device focuser_template = INDIGO_DEVICE_INITIALIZER(
		FOCUSER_DSD_NAME,
		focuser_attach,
		eaf_enumerate_properties,
		focuser_change_property,
		NULL,
		focuser_detach
	);

	static indigo_driver_action last_action = INDIGO_DRIVER_SHUTDOWN;

	SET_DRIVER_INFO(info, "Deep Sky Dad Focuser", __FUNCTION__, DRIVER_VERSION, false, last_action);

	if (action == last_action)
		return INDIGO_OK;

	switch (action) {
	case INDIGO_DRIVER_INIT:
		last_action = action;

		/* figure out the number of devices to expose */
		if (getenv("FOCUSER_DSD_DEVICE_NUMBER") != NULL) {
			device_number = atoi(getenv("FOCUSER_DSD_DEVICE_NUMBER"));
			if (device_number < 1) device_number = 1;
			if (device_number > MAX_DEVICES) device_number = MAX_DEVICES;
		}


		for (int index = 0; index < device_number; index++) {
			private_data[index] = malloc(sizeof(dsd_private_data));
			assert(private_data[index] != NULL);
			memset(private_data[index], 0, sizeof(dsd_private_data));
			private_data[index]->dev_id = -1;
			focuser[index] = malloc(sizeof(indigo_device));
			assert(focuser[index] != NULL);
			memcpy(focuser[index], &focuser_template, sizeof(indigo_device));
			focuser[index]->private_data = private_data[index];
			sprintf(focuser[index]->name, "%s #%d", FOCUSER_DSD_NAME, index);
			indigo_attach_device(focuser[index]);
		}
		break;

	case INDIGO_DRIVER_SHUTDOWN:
		last_action = action;
		for (int index = 0; index < device_number; index++) {
			if (focuser[index] != NULL) {
				indigo_detach_device(focuser[index]);
				free(focuser[index]);
				focuser[index] = NULL;
			}
			if (private_data[index] != NULL) {
				free(private_data[index]);
				private_data[index] = NULL;
			}
		}
		break;

	case INDIGO_DRIVER_INFO:
		break;
	}

	return INDIGO_OK;
}