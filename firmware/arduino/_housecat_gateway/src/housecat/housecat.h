
#ifndef _HOUSECAT_H_
#define _HOUSECAT_H_

#include "housecat_protocol.h"

#include "housecat_onewire.h"

#include "housecat_inputs.h"
#include "housecat_input_button.h"
#include "housecat_input_sensor.h"

#include "housecat_outputs.h"
#include "housecat_output_relay.h"
#include "housecat_output_blinds.h"

#include "housecat_analog_outputs.h"
#include "housecat_analog_output_dimmer.h"

extern housecatProtocol g_housecat_protocol;

extern housecatInputs g_housecat_inputs;
extern housecatOutputs g_housecat_outputs;
extern housecatAnalogOutputs g_housecat_analog_outputs;

#endif