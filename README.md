<p align="center">
  <img src="./assets/light-switch.svg" width="154">
  <h1 align="center">TrigPLC</h1>
  <p align="center">TrigPLC allows to <b>integrate</b> a Controllino device with Home Assistant using MQTT.<p>
  <p align="center">
    <a href="https://github.com/mrwolfjunior/TrigPLC/blob/master/LICENSE">
      <img src="https://img.shields.io/badge/license-GPLv3-blue.svg" />
    </a>
    <a href="https://www.arduino.cc">
    	<img src="https://img.shields.io/badge/built%20for-Arduino-green.svg" />
    </a>
    <a href="https://www.controllino.com">
    	<img src="https://img.shields.io/badge/built%20for-Controllino-green.svg" />
    </a>
  </p>
</p>

# Features

This repo is my Controllino firmware for home automation

Current feature:

- [x] Home assistant integration with auto discovery
- [x] FreeRTOS integration
- [ ] Multiple press on single button (eg. double press --> 2 lights)
- [ ] OTA update 

# Brief introduction

TrigPLC is an open source firmware for the Controllino, an industry-grade PLC Arduino compatible. The firmware should be working with an Arduino Mega 2560 and a w5100 shield for ethernet connection.

Check [Controllino webpage](https://www.controllino.com/) if you are interested.

# Why FreeRTOS

I do like task management based on FreeRTOS, thus I have created two independent task with different priority:

- Hight priority: Task that manage the basic functionality ( button pressed --> light on)
- Low priority: Task that manage the IoT functionality

For further information about freertos library please check [FreeRTOS website](https://www.freertos.org/)

# Instructions

1. Clone the repo
2. Create a new project on PlatformIO and add the repo folder
3. Customize TrigPLC/src/config.h
5. Build and flash the board

Please make sure to adjust properly the stack size for each task.

## Control multiple lights from multiple buttons

If you want to control multiple lights from multiple buttons simply add one Trigger for each button / light couple. 

Example - two buttons control 1 light:

    Trigger(&button_A4, &light_R3, "Light 1", "p1_T4"),       // Light 1 - button 1
    Trigger(&button_A3, &light_R3, "", "p1_T4"),              // Light 1 - button 2

NOTE: Make sure to set a friendly name only for the first Trigger (in the above example "Light 1"), for the others button that manage the same light you must set an empty string.

# Notes

If you intend to modify the FreeRTOS tasks make sure to adjust properly the stack size for each task.

<p align="center">
<div>Icons made by <a href="https://www.flaticon.com/authors/pixel-perfect" title="Pixel perfect">Pixel perfect</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a></div>
</p>
