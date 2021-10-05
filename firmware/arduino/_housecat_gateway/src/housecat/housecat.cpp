
#include "housecat_protocol.h"
#include "housecat_inputs.h"
#include "housecat_outputs.h"
#include "housecat_analog_outputs.h"

housecatProtocol g_housecat_protocol;

housecatInputs g_housecat_inputs(g_housecat_protocol);
housecatOutputs g_housecat_outputs(g_housecat_protocol);
housecatAnalogOutputs g_housecat_analog_outputs(g_housecat_protocol);