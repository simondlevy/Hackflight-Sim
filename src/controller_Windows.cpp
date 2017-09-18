/*
   controller_Windows.cpp : WIN32 support for controllers in Hackflight-VREP simulator plugin

   Copyright (C) Simon D. Levy, Matt Lubas, and Julio Hidalgo Lopez 2016

   This file is part of Hackflight-VREP.

   Hackflight-VREP is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Hackflight-VREP is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with Hackflight-VREP.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "controller.hpp"

#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include <conio.h>

#include <stdio.h>

// Adapted from http://olek.matthewm.com.pl//courses/ee-ces/examples/02_JOYSTICK_WIN32.C.HTML

controller_t controllerInit(void)
{ 
    // Default to PS3
    controller_t controller = PS3;

    JOYCAPS joycaps;
    if (joyGetDevCaps(JOYSTICKID1, &joycaps, sizeof(joycaps))==JOYERR_NOERROR) {

        switch (joycaps.wMid) {

            case 3727:
                controller = PS3;
                break;

            case 1155:
                controller = TARANIS;
                break;

            case 1133:
                controller = EXTREME3D;
                break;

            case 9414:
                controller = XBOX360;
                break;
        }
    }

    return controller;
}

// Turns button value into aux-switch demand
static void buttonToAuxDemand(float * demands, int buttons)
{
    if (buttons == 1)
        demands[4] = -1;

    if (buttons == 2)
        demands[4] = 0;

    if (buttons == 4)
        demands[4] = +1;
}

static float joynorm(int axisval) 
{
    return (axisval - (float)32767) / 32767;
}


void controllerRead(controller_t controller, float * demands) 
{
    JOYINFOEX joyState;
    joyState.dwSize=sizeof(joyState);
    joyState.dwFlags=JOY_RETURNALL | JOY_RETURNPOVCTS | JOY_RETURNCENTERED | JOY_USEDEADZONE;
    joyGetPosEx(JOYSTICKID1, &joyState);

    /*
       printf("X:%d Y:%d Z:%d   R:%d U:%d V:%d  b:%d\n", 
       joyState.dwXpos, joyState.dwYpos, joyState.dwZpos, 
       joyState.dwRpos, joyState.dwUpos, joyState.dwVpos,
       joyState.dwButtons);
    */

    // Handle each controller differently
    switch (controller) {

        case TARANIS:
            demands[0] =   joynorm(joyState.dwXpos);			// throttle        
            demands[1] =  -joynorm(joyState.dwYpos);			// roll
            demands[2] =  -joynorm(joyState.dwZpos);			// pitch
            demands[3] =   joynorm(joyState.dwVpos);			// yaw
            demands[4] =   -1;			                        // aux switch
            break;

        case PS3:
            demands[0] = -joynorm(joyState.dwYpos);            // throttle
            demands[1] = -joynorm(joyState.dwZpos);            // roll
            demands[2] =  joynorm(joyState.dwRpos);            // pitch
            demands[3] =  joynorm(joyState.dwXpos);            // yaw
            buttonToAuxDemand(demands, joyState.dwButtons);    // aux switch
            break;

        case XBOX360:
            demands[0] = -joynorm(joyState.dwYpos);            // throttle
            demands[1] = -joynorm(joyState.dwUpos);            // roll
            demands[2] =  joynorm(joyState.dwRpos);            // pitch
            demands[3] =  joynorm(joyState.dwXpos);            // yaw
            buttonToAuxDemand(demands, joyState.dwButtons); // aux switch
            break;

        case EXTREME3D:
            demands[0] = -joynorm(joyState.dwZpos);            // throttle
            demands[1] = -joynorm(joyState.dwXpos);            // roll
            demands[2] =  joynorm(joyState.dwYpos);            // pitch
            demands[3] =  joynorm(joyState.dwRpos);            // yaw
            buttonToAuxDemand(demands, joyState.dwButtons); // aux switch
            break;
    }
}

void controllerClose(void)
{ 
}
