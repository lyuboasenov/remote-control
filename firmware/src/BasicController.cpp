#include "BasicController.h"

#ifdef CONTROLLER

const uint8_t receiver_mac[] = { 0x10, 0x52, 0x1C, 0x75, 0x86, 0x24 };

BasicController::BasicController() :
   Controller() {
   _radios = new Radio *[2] { new EspNowRadio(), new WiFiRadio() };
   _endpoints = new Endpoint * [2] { new EspNowEndpoint(receiver_mac), new WiFiEndpoint("esp32_remote_controller", "p@ssw0rd", true) };
}

void BasicController::begin() {
   Controller::begin();
   _radios[0]->begin(_endpoints[0]);
   _radios[1]->begin(_endpoints[1]);
   Wire.begin();
   _mcp.begin_I2C();

/*

   [                 ] [   ]          [   ]    [   ]          [   ] [                 ]
   [                 ] [ R ] [ S1 ]   [   ]    [   ]   [ S4 ] [ R ] [                 ]
   [       J1        ] [ S ]          [   ]    [   ]          [ S ] [       J2        ]
   [                 ] [ 1 ] [ S2 ]   [ P ]    [ P ]   [ S5 ] [ 2 ] [                 ]
   [                 ] [   ]          [ 2 ]    [ 3 ]          [   ] [                 ]
                             [ S3 ]   [   ]    [   ]
                                      [   ]    [   ]
                   [    ]             [   ]    [   ]              [     ]
   [     ]         [ S6 ]                                         [ RE1 ]       [     ]
   [ P1  ]         [    ]                                         [     ]       [  P4 ]
   [     ]                                                                      [     ]
                  [                                                     ]
                  [                                                     ]
   [           ]  [                                                     ]  [           ]
   [     J3    ]  [                         D1                          ]  [     J4    ]
   [           ]  [                                                     ]  [           ]
                  [                                                     ]
                  [                                                     ]

*/

   Control * controls[] = {
      new Control("J2_H", new AnalogPinReader(CTRL_RIGHT_JOY_TOP_X_PIN), &_control_data[0], true),
      new Control("J2_V", new AnalogPinReader(CTRL_RIGHT_JOY_TOP_Y_PIN), &_control_data[1], true),

      new Control("J4_H", new AnalogPinReader(CTRL_RIGHT_JOY_BOTTOM_X_PIN), &_control_data[2], true),
      new Control("J4_V", new AnalogPinReader(CTRL_RIGHT_JOY_BOTTOM_Y_PIN), &_control_data[3], true),

      new Control("P4__", new AnalogPinReader(CTRL_RIGHT_ROTARY_POT_PIN), &_control_data[4]),
      new Control("P3__", new AnalogPinReader(CTRL_RIGHT_SLIDER_POT_PIN), &_control_data[5]),

      new Control("J1_H", new AvrExtensionAnalogReader(&Wire, 0x6B, 0), &_control_data[6], true),
      new Control("J1_V", new AvrExtensionAnalogReader(&Wire, 0x6B, 5), &_control_data[7], true),

      new Control("J3_H", new AvrExtensionAnalogReader(&Wire, 0x6B, 1), &_control_data[8], true),
      new Control("J3_V", new AvrExtensionAnalogReader(&Wire, 0x6B, 2), &_control_data[9], true),

      new Control("P1__", new AvrExtensionAnalogReader(&Wire, 0x6B, 3), &_control_data[10]),
      new Control("P2__", new AvrExtensionAnalogReader(&Wire, 0x6B, 4), &_control_data[11]),

      new Control("S1_1", new MCP23017ValueReader(&_mcp, 14, true), &_control_data[12]),
      new Control("S1_2", new MCP23017ValueReader(&_mcp, 15, true), &_control_data[13]),

      new Control("S2_1", new MCP23017ValueReader(&_mcp, 12, true), &_control_data[14]),
      new Control("S2_2", new MCP23017ValueReader(&_mcp, 13, true), &_control_data[15]),

      new Control("S3_1", new MCP23017ValueReader(&_mcp, 10, true), &_control_data[16]),
      new Control("S3_2", new MCP23017ValueReader(&_mcp, 11, true), &_control_data[17]),

      new Control("S4_1", new MCP23017ValueReader(&_mcp, 6, true), &_control_data[18]),
      new Control("S4_2", new MCP23017ValueReader(&_mcp, 7, true), &_control_data[19]),

      new Control("S5_1", new MCP23017ValueReader(&_mcp, 4, true), &_control_data[20]),
      new Control("S5_2", new MCP23017ValueReader(&_mcp, 5, true), &_control_data[21]),

      new Control("J1_S", new MCP23017ValueReader(&_mcp, 2, true), &_control_data[22]),
      new Control("J2_S", new MCP23017ValueReader(&_mcp, 4, true), &_control_data[23]),

      new Control("J3_S", new MCP23017ValueReader(&_mcp, 1, true), &_control_data[24]),
      new Control("J4_S", new MCP23017ValueReader(&_mcp, 3, true), &_control_data[25]),
   };
   uint8_t control_count = 26;

   CONTROLLER_DEBUG("CTRL:CTRL_CNT:%hhu:%hhu", control_count);
   for (uint8_t i = 0; i < control_count; i++) {
      controls[i]->begin();
   }

   set_controls(controls, control_count);
}

Radio * BasicController::get_radio(uint8_t index) {
   return _radios[index];
}

Endpoint * BasicController::get_endpoint(uint8_t index) {
   return _endpoints[index];
}

#endif