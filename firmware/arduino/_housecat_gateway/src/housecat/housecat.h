
#ifndef _HOUSECAT_H_
#define _HOUSECAT_H_

#include "housecat_protocol.h"

#include "housecat_inputs.h"
#include "housecat_input_button.h"

#include "housecat_outputs.h"
#include "housecat_output_relay.h"
#include "housecat_output_blinds.h"

#include "housecat_analog_outputs.h"
#include "housecat_analog_output_dimmer.h"

housecatProtocol protocol;

housecatInputs inputs;
housecatOutputs outputs;
housecatAnalogOutputs analog_outputs;

#endif