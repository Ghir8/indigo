// Copyright (c) 2017 CloudMakers, s. r. o.
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
// 2.0 by Peter Polakovic <peter.polakovic@cloudmakers.eu>

/** INDIGO DOME Simulator driver
 \file indigo_dome_simulator.c
 */

#define DRIVER_VERSION 0x0001
#define DRIVER_NAME	"indigo_dome_simulator"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <pthread.h>

#include "indigo_driver_xml.h"
#include "indigo_dome_driver.h"
#include "indigo_dome_simulator.h"

#define PRIVATE_DATA					((simulator_private_data *)device->private_data)

typedef struct {
	int target_position, current_position;
	indigo_timer *position_timer;
} simulator_private_data;

// -------------------------------------------------------------------------------- INDIGO dome device implementation

static void dome_timer_callback(indigo_device *device) {
	if (DOME_HORIZONTAL_COORDINATES_PROPERTY->state == INDIGO_ALERT_STATE) {
		DOME_HORIZONTAL_COORDINATES_AZ_ITEM->number.value = PRIVATE_DATA->target_position = PRIVATE_DATA->current_position;
		indigo_update_dome_coordinates(device, NULL);
		DOME_STEPS_PROPERTY->state = INDIGO_ALERT_STATE;
		indigo_update_property(device, DOME_STEPS_PROPERTY, NULL);
	} else {
		if (DOME_PARK_PROPERTY->state != INDIGO_BUSY_STATE && DOME_PARK_PARKED_ITEM->sw.value) {
			indigo_set_switch(DOME_PARK_PROPERTY, DOME_PARK_UNPARKED_ITEM, true);
			DOME_PARK_PROPERTY->state = INDIGO_OK_STATE;
			indigo_update_property(device, DOME_PARK_PROPERTY, "Unparked");
		}
		if (DOME_DIRECTION_MOVE_CLOCKWISE_ITEM->sw.value && PRIVATE_DATA->current_position != PRIVATE_DATA->target_position) {
			DOME_HORIZONTAL_COORDINATES_PROPERTY->state = INDIGO_BUSY_STATE;
			int dif = (int)(PRIVATE_DATA->target_position - PRIVATE_DATA->current_position + 360) % 360;
			if (dif > DOME_SPEED_ITEM->number.value)
				DOME_HORIZONTAL_COORDINATES_AZ_ITEM->number.value = PRIVATE_DATA->current_position = (int)(PRIVATE_DATA->current_position + DOME_SPEED_ITEM->number.value + 360) % 360;
			else
				DOME_HORIZONTAL_COORDINATES_AZ_ITEM->number.value = PRIVATE_DATA->current_position = PRIVATE_DATA->target_position;
			indigo_update_dome_coordinates(device, NULL);
			DOME_STEPS_PROPERTY->state = INDIGO_BUSY_STATE;
			indigo_update_property(device, DOME_STEPS_PROPERTY, NULL);
			indigo_set_timer(device, 0.1, dome_timer_callback);
		} else if (DOME_DIRECTION_MOVE_COUNTERCLOCKWISE_ITEM->sw.value && PRIVATE_DATA->current_position != PRIVATE_DATA->target_position) {
			DOME_HORIZONTAL_COORDINATES_PROPERTY->state = INDIGO_BUSY_STATE;
			int dif = (int)(PRIVATE_DATA->current_position - PRIVATE_DATA->target_position + 360) % 360;
			if (dif > DOME_SPEED_ITEM->number.value)
				DOME_HORIZONTAL_COORDINATES_AZ_ITEM->number.value = PRIVATE_DATA->current_position = (int)(PRIVATE_DATA->current_position - DOME_SPEED_ITEM->number.value + 360) % 360;
			else
				DOME_HORIZONTAL_COORDINATES_AZ_ITEM->number.value = PRIVATE_DATA->current_position = PRIVATE_DATA->target_position;
			indigo_update_dome_coordinates(device, NULL);
			DOME_STEPS_PROPERTY->state = INDIGO_BUSY_STATE;
			indigo_update_property(device, DOME_STEPS_PROPERTY, NULL);
			indigo_set_timer(device, 0.1, dome_timer_callback);
		} else {
			DOME_HORIZONTAL_COORDINATES_PROPERTY->state = INDIGO_OK_STATE;
			DOME_HORIZONTAL_COORDINATES_AZ_ITEM->number.value = PRIVATE_DATA->current_position;
			indigo_update_dome_coordinates(device, NULL);
			DOME_STEPS_PROPERTY->state = INDIGO_OK_STATE;
			indigo_update_property(device, DOME_STEPS_PROPERTY, NULL);
			if (DOME_PARK_PROPERTY->state == INDIGO_BUSY_STATE) {
				DOME_PARK_PROPERTY->state = INDIGO_OK_STATE;
				indigo_update_property(device, DOME_PARK_PROPERTY, "Parked");
			}
		}
	}
}

static indigo_result dome_attach(indigo_device *device) {
	assert(device != NULL);
	assert(PRIVATE_DATA != NULL);
	if (indigo_dome_attach(device, DRIVER_VERSION) == INDIGO_OK) {
		// -------------------------------------------------------------------------------- DOME_SPEED
		DOME_SPEED_ITEM->number.value = 1;
		// --------------------------------------------------------------------------------
		INDIGO_DEVICE_ATTACH_LOG(DRIVER_NAME, device->name);
		return indigo_dome_enumerate_properties(device, NULL, NULL);
	}
	return INDIGO_FAILED;
}

static indigo_result dome_change_property(indigo_device *device, indigo_client *client, indigo_property *property) {
	assert(device != NULL);
	assert(DEVICE_CONTEXT != NULL);
	assert(property != NULL);
	if (indigo_property_match(CONNECTION_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- CONNECTION
		indigo_property_copy_values(CONNECTION_PROPERTY, property, false);
		CONNECTION_PROPERTY->state = INDIGO_OK_STATE;
	} else if (indigo_property_match(DOME_STEPS_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- DOME_STEPS
		indigo_property_copy_values(DOME_STEPS_PROPERTY, property, false);
		DOME_STEPS_ITEM->number.value = (int)DOME_STEPS_ITEM->number.value;
		if (DOME_DIRECTION_MOVE_COUNTERCLOCKWISE_ITEM->sw.value) {
			PRIVATE_DATA->target_position = (int)(PRIVATE_DATA->current_position - DOME_STEPS_ITEM->number.value + 360) % 360;
		} else if (DOME_DIRECTION_MOVE_CLOCKWISE_ITEM->sw.value) {
			PRIVATE_DATA->target_position = (int)(PRIVATE_DATA->current_position + DOME_STEPS_ITEM->number.value + 360) % 360;
		}
		DOME_HORIZONTAL_COORDINATES_PROPERTY->state = INDIGO_BUSY_STATE;
		DOME_HORIZONTAL_COORDINATES_AZ_ITEM->number.value = PRIVATE_DATA->current_position;
		indigo_update_dome_coordinates(device, NULL);
		DOME_STEPS_PROPERTY->state = INDIGO_BUSY_STATE;
		indigo_update_property(device, DOME_STEPS_PROPERTY, NULL);
		indigo_set_timer(device, 0.5, dome_timer_callback);
		return INDIGO_OK;
	} else if (indigo_property_match(DOME_HORIZONTAL_COORDINATES_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- DOME_HORIZONTAL_COORDINATES
		indigo_property_copy_values(DOME_HORIZONTAL_COORDINATES_PROPERTY, property, false);
		PRIVATE_DATA->target_position = (int)DOME_HORIZONTAL_COORDINATES_AZ_ITEM->number.value % 360;
		int dif = (int)(PRIVATE_DATA->target_position - PRIVATE_DATA->current_position + 360) % 360;
		if (dif < 180) {
			indigo_set_switch(DOME_DIRECTION_PROPERTY, DOME_DIRECTION_MOVE_CLOCKWISE_ITEM, true);
			DOME_STEPS_ITEM->number.value = dif;
		} else if (dif > 180) {
			indigo_set_switch(DOME_DIRECTION_PROPERTY, DOME_DIRECTION_MOVE_COUNTERCLOCKWISE_ITEM, true);
			DOME_STEPS_ITEM->number.value = 360 - dif;
		}
		DOME_DIRECTION_PROPERTY->state = INDIGO_OK_STATE;
		indigo_update_property(device, DOME_DIRECTION_PROPERTY, NULL);
		DOME_STEPS_PROPERTY->state = INDIGO_BUSY_STATE;
		indigo_update_property(device, DOME_STEPS_PROPERTY, NULL);
		DOME_HORIZONTAL_COORDINATES_PROPERTY->state = INDIGO_BUSY_STATE;
		indigo_update_dome_coordinates(device, NULL);
		indigo_set_timer(device, 0.5, dome_timer_callback);
		return INDIGO_OK;
	} else if (indigo_property_match(DOME_ABORT_MOTION_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- DOME_ABORT_MOTION
		indigo_property_copy_values(DOME_ABORT_MOTION_PROPERTY, property, false);
		if (DOME_ABORT_MOTION_ITEM->sw.value && DOME_HORIZONTAL_COORDINATES_PROPERTY->state == INDIGO_BUSY_STATE) {
			DOME_HORIZONTAL_COORDINATES_PROPERTY->state = INDIGO_ALERT_STATE;
			DOME_HORIZONTAL_COORDINATES_AZ_ITEM->number.value = PRIVATE_DATA->current_position;
			indigo_update_dome_coordinates(device, NULL);
		}
		DOME_ABORT_MOTION_PROPERTY->state = INDIGO_OK_STATE;
		DOME_ABORT_MOTION_ITEM->sw.value = false;
		indigo_update_property(device, DOME_ABORT_MOTION_PROPERTY, NULL);
		return INDIGO_OK;
	} else if (indigo_property_match(DOME_SHUTTER_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- DOME_SHUTTER
		indigo_property_copy_values(DOME_SHUTTER_PROPERTY, property, false);
		DOME_SHUTTER_PROPERTY->state = INDIGO_OK_STATE;
		indigo_update_property(device, DOME_SHUTTER_PROPERTY, NULL);
		return INDIGO_OK;
	} else if (indigo_property_match(DOME_PARK_PROPERTY, property)) {
		// -------------------------------------------------------------------------------- DOME_PARK
		indigo_property_copy_values(DOME_PARK_PROPERTY, property, false);
		if (DOME_PARK_UNPARKED_ITEM->sw.value) {
			indigo_set_switch(DOME_PARK_PROPERTY, DOME_PARK_UNPARKED_ITEM, true);
			DOME_PARK_PROPERTY->state = INDIGO_OK_STATE;
		} else {
			DOME_PARK_PROPERTY->state = INDIGO_BUSY_STATE;
			if (PRIVATE_DATA->current_position > 180) {
				DOME_DIRECTION_PROPERTY->state = INDIGO_OK_STATE;
				indigo_set_switch(DOME_DIRECTION_PROPERTY, DOME_DIRECTION_MOVE_CLOCKWISE_ITEM, true);
				DOME_STEPS_ITEM->number.value = 360 - PRIVATE_DATA->current_position;
			} else if (PRIVATE_DATA->current_position < 180) {
				DOME_DIRECTION_PROPERTY->state = INDIGO_OK_STATE;
				indigo_set_switch(DOME_DIRECTION_PROPERTY, DOME_DIRECTION_MOVE_COUNTERCLOCKWISE_ITEM, true);
				DOME_STEPS_ITEM->number.value = PRIVATE_DATA->current_position;
			}
			PRIVATE_DATA->target_position = 0;
			DOME_DIRECTION_PROPERTY->state = INDIGO_OK_STATE;
			indigo_update_property(device, DOME_DIRECTION_PROPERTY, NULL);
			DOME_STEPS_PROPERTY->state = INDIGO_BUSY_STATE;
			indigo_update_property(device, DOME_STEPS_PROPERTY, NULL);
			DOME_HORIZONTAL_COORDINATES_PROPERTY->state = INDIGO_BUSY_STATE;
			indigo_update_dome_coordinates(device, NULL);
			indigo_set_timer(device, 0.5, dome_timer_callback);
		}
		indigo_update_property(device, DOME_PARK_PROPERTY, NULL);
		return INDIGO_OK;
		// --------------------------------------------------------------------------------
	}
	return indigo_dome_change_property(device, client, property);
}

static indigo_result dome_detach(indigo_device *device) {
	assert(device != NULL);
	if (CONNECTION_CONNECTED_ITEM->sw.value)
		indigo_device_disconnect(NULL, device->name);
	INDIGO_DEVICE_DETACH_LOG(DRIVER_NAME, device->name);
	return indigo_dome_detach(device);
}

// --------------------------------------------------------------------------------

static simulator_private_data *private_data = NULL;

static indigo_device *dome = NULL;

indigo_result indigo_dome_simulator(indigo_driver_action action, indigo_driver_info *info) {
	static indigo_device dome_template = {
		DOME_SIMULATOR_NAME, false, 0, NULL, NULL, INDIGO_OK, INDIGO_VERSION_CURRENT,
		dome_attach,
		indigo_dome_enumerate_properties,
		dome_change_property,
		NULL,
		dome_detach
	};

	static indigo_driver_action last_action = INDIGO_DRIVER_SHUTDOWN;

	SET_DRIVER_INFO(info, "Dome Simulator", __FUNCTION__, DRIVER_VERSION, last_action);

	if (action == last_action)
		return INDIGO_OK;

	switch(action) {
		case INDIGO_DRIVER_INIT:
			last_action = action;
			private_data = malloc(sizeof(simulator_private_data));
			assert(private_data != NULL);
			memset(private_data, 0, sizeof(simulator_private_data));
			dome = malloc(sizeof(indigo_device));
			assert(dome != NULL);
			memcpy(dome, &dome_template, sizeof(indigo_device));
			dome->private_data = private_data;
			indigo_attach_device(dome);
			break;

		case INDIGO_DRIVER_SHUTDOWN:
			last_action = action;
			if (dome != NULL) {
				indigo_detach_device(dome);
				free(dome);
				dome = NULL;
			}
			if (private_data != NULL) {
				free(private_data);
				private_data = NULL;
			}
			break;

		case INDIGO_DRIVER_INFO:
			break;
	}
	return INDIGO_OK;
}
