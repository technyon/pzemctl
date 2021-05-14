#pragma once

// pzem004t
#define PZEM1_ADDRESS 0xf8
#define PZEM2_ADDRESS 0xf8
#define PZEM3_ADDRESS 0xf8

// LEDs
#define LED1_PIN 5
#define LED2_PIN 6
#define LED3_PIN 7
#define LED4_PIN 8

// Input
#define SWITCH_PHASE_PIN 44
#define SWITCH_VIEW_PIN 46
#define SWITCH_ON_OFF_PIN 48
#define LONG_PRESS_THRESHOLD 1000

// Switch
#define SWITCH_PIN 30

// Ethernet
#define ETHERNET_CS_PIN 53
#define ETHERNET_RESET_PIN 49

// MQTT
#define phase1VoltageTopic "energy/phase1/voltage"
#define phase1CurrentTopic "energy/phase1/current"
#define phase1PowerTopic "energy/phase1/power"
#define phase1EnergyTopic  "energy/phase1/energy"
#define phase1FrequencyTopic "energy/phase1/frequency"
#define phase1PowerFactorTopic "energy/phase1/powerfactor"
#define phase1EnergyOffsetTopic  "energy/phase1/energyOffset"

#define phase2VoltageTopic  "energy/phase2/voltage"
#define phase2CurrentTopic "energy/phase2/current"
#define phase2PowerTopic "energy/phase2/power"
#define phase2EnergyTopic  "energy/phase2/energy"
#define phase2FrequencyTopic "energy/phase2/frequency"
#define phase2PowerFactorTopic "energy/phase2/powerfactor"
#define phase2EnergyOffsetTopic  "energy/phase2/energyOffset"

#define phase3VoltageTopic  "energy/phase3/voltage"
#define phase3CurrentTopic "energy/phase3/current"
#define phase3PowerTopic "energy/phase3/power"
#define phase3EnergyTopic  "energy/phase3/energy"
#define phase3FrequencyTopic "energy/phase3/frequency"
#define phase3PowerFactorTopic "energy/phase3/powerfactor"
#define phase3EnergyOffsetTopic  "energy/phase3/energyOffset"

/*
#define phasesCombinedVoltageTopic  "energy/phases/voltage"
#define phasesCombinedCurrentTopic "energy/phases/current"
#define phasesCombinedPowerTopic "energy/phases/power"
#define phasesCombinedEnergyTopic  "energy/phases/energy"
#define phasesCombinedFrequencyTopic "energy/phases/frequency"
#define phasesCombinedPowerFactorTopic "energy/phases/powerfactor"
*/

#define led1BrightnessTopic  "energy/control/led1Brightness"
#define led2BrightnessTopic  "energy/control/led2Brightness"
#define selectedViewTopic  "energy/control/view"
#define selectedPhaseTopic  "energy/control/phase"
#define switchStateTopic  "energy/control/switchState"
#define resetEnergyTopic  "energy/control/resetEnergy"
#define reconnectCountTopic  "energy/control/reconnectCount"
#define customViewValue1Topic  "energy/control/customValue1"
#define customViewValue2Topic  "energy/control/customValue2"
#define customViewValue3Topic  "energy/control/customValue3"