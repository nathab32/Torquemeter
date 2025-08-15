#include <Arduino.h>
#include "NAU7802.h"
#include <Wire.h>

//Constructor
NAU7802::NAU7802() {}

// call this function first to initialize the NAU7802 and start it in low power mode
bool NAU7802::begin() {
    Wire.begin();

    writeRegister(0x00, 0b00000001); //reset registers
    writeRegister(0x00, 0b00000010); //enter normal operation

    delay(1);
    if ((readRegister(0x00) | 0b11110111) != 0b11111111) {
        Serial.println("NAU7802: 0x00[3] set to 0");
        return false;
    }

    return true;
}

bool NAU7802::setGain(NAU7802_Gain gain) {
    byte reg = readRegister(0x01);
    reg = reg & 0b11111000; //clear last 3 bits

    switch (gain)
    {
    case NAU7802_GAIN_1:
        writeRegister(0x01, reg);
        break;
    
    case NAU7802_GAIN_2:
        writeRegister(0x01, (reg | NAU7802_GAIN_2));
        break;

    case NAU7802_GAIN_4:
        writeRegister(0x01, (reg | NAU7802_GAIN_4));
        break;

    case NAU7802_GAIN_8:
        writeRegister(0x01, (reg | NAU7802_GAIN_8));
        break;

    case NAU7802_GAIN_16:
        writeRegister(0x01, (reg | NAU7802_GAIN_16));
        break;

    case NAU7802_GAIN_32:
        writeRegister(0x01, (reg | NAU7802_GAIN_32));
        break;

    case NAU7802_GAIN_64:
        writeRegister(0x01, (reg | NAU7802_GAIN_64));
        break;

    case NAU7802_GAIN_128:
        writeRegister(0x01, (reg | NAU7802_GAIN_128));
        break;

    default:
        Serial.println("NAU7802: invalid gain");
        return false;
        break;
    }
    return true;
}

NAU7802_Gain NAU7802::getGain() {
    byte reg = readRegister(0x01);
    reg = reg & 0b00000111;

    return static_cast<NAU7802_Gain>(reg);
}

bool NAU7802::setAnalogSupply(NAU7802_SupplyVoltage supply) {
    byte reg = readRegister(0x01);
    reg = reg & 0b11000111; //clear last bits 5:3
    
    byte reg1 = readRegister(0x00);
    reg1 = reg1 | (0b1 << 7); //add 1 to bit 7 of register

    switch (supply)
    {
    case NAU7802_SUPPLY_2V4:
        writeRegister(0x01, (reg | (NAU7802_SUPPLY_2V4 << 3)));
        writeRegister(0x00, reg1);
        break;
    
    case NAU7802_SUPPLY_2V7:
        writeRegister(0x01, (reg | (NAU7802_SUPPLY_2V7 << 3)));
        writeRegister(0x00, reg1);
        break;

    case NAU7802_SUPPLY_3V0:
        writeRegister(0x01, (reg | (NAU7802_SUPPLY_3V0 << 3)));
        writeRegister(0x00, reg1);
        break;

    case NAU7802_SUPPLY_3V3:
        writeRegister(0x01, (reg | (NAU7802_SUPPLY_3V3 << 3)));
        writeRegister(0x00, reg1);
        break;

    case NAU7802_SUPPLY_3V6:
        writeRegister(0x01, (reg | (NAU7802_SUPPLY_3V6 << 3)));
        writeRegister(0x00, reg1);
        break;

    case NAU7802_SUPPLY_3V9:
        writeRegister(0x01, (reg | (NAU7802_SUPPLY_3V9 << 3)));
        writeRegister(0x00, reg1);
        break;

    case NAU7802_SUPPLY_4V2:
        writeRegister(0x01, (reg | (NAU7802_SUPPLY_4V2 << 3)));
        writeRegister(0x00, reg1);
        break;

    case NAU7802_SUPPLY_4V5:
        writeRegister(0x01, reg);
        writeRegister(0x00, reg1);
        break;

    case NAU7802_SUPPLY_EXTERNAL:
        reg1 = reg1 & 0b01111111; //removes 1 from bit 7 of register
        writeRegister(0x00, reg1);
        break;

    default:
        Serial.println("NAU7802: invalid analog supply voltage");
        return false;
        break;
    }

    return true;
}

NAU7802_SupplyVoltage NAU7802::getAnalogSupply() {
    byte reg = readRegister(0x01);
    reg = (reg & 0b00111000) >> 3;

    return static_cast<NAU7802_SupplyVoltage>(reg);
}

bool NAU7802::setConversionRate(NAU7802_ConversionRate rate) {
    byte reg = readRegister(0x02);
    reg = reg & 0b10001111; //clear bits 6:4

    switch (rate)
    {
    case NAU7802_CONVERSIONRATE_10:
        conversionRate = 10;
        break;
    
    case NAU7802_CONVERSIONRATE_20:
        reg = reg | (NAU7802_CONVERSIONRATE_20 << 4);
        conversionRate = 20;
        break;
    
    case NAU7802_CONVERSIONRATE_40:
        reg = reg | (NAU7802_CONVERSIONRATE_40 << 4);
        conversionRate = 40;
        break;

    case NAU7802_CONVERSIONRATE_80:
        reg = reg | (NAU7802_CONVERSIONRATE_80 << 4);
        conversionRate = 80;
        break;

    case NAU7802_CONVERSIONRATE_320:
        reg = reg | (NAU7802_CONVERSIONRATE_320 << 4);
        conversionRate = 320;
        break;

    default:
        Serial.println("NAU7802: Invalid conversion rate set");
        return false;
        break;
    }

    writeRegister(0x02, reg);
    return true;
}

NAU7802_ConversionRate NAU7802::getConversionRate() {
    byte reg = readRegister(0x02);
    reg = (reg & 0b01110000) >> 4;
    return static_cast<NAU7802_ConversionRate>(reg);
}

bool NAU7802::setChannel(NAU7802_Channel channel) {
    byte reg = readRegister(0x02);
    reg = reg & (0b01111111); //clear bit 7

    switch (channel)
    {
    case NAU7802_CHANNEL_1:
        break;
    
    case NAU7802_CHANNEL_2:
        reg = reg | (NAU7802_CHANNEL_2 << 7);
        break;

    default:
        Serial.println("NAU7802: Invalid channel");
        return false;
        break;
    }

    return true;
}

NAU7802_Channel NAU7802::getChannel() {
    byte reg = readRegister(0x02);
    reg = reg >> 7;

    return static_cast<NAU7802_Channel>(reg);
}

bool NAU7802::setLdoMode(NAU7802_LDOMODE mode) {
    byte reg = readRegister(0x1B);
    reg = reg & 0b10111111;

    switch (mode)
    {
    case NAU7802_LDOMODE_0:
        break;
    
    case NAU7802_LDOMODE_1:
        reg = reg | (0b1 << 6);
        break;

    default:
        Serial.println("NAU7802: Invalid LDO Mode");
        return false;
        break;
    }

    writeRegister(0x1B, reg);
    return true;
}

NAU7802_LDOMODE NAU7802::getLdoMode() {
    byte reg = readRegister(0x1B);
    reg = (reg & 0b01000000) >> 6;

    return static_cast<NAU7802_LDOMODE>(reg);
}
    
bool NAU7802::calibrate(NAU7802_Calibrate calmode) {
    byte reg = readRegister(0x02);
    reg = reg & 0b11111100; //clear bits 1:0

    switch (calmode)
    {
    case NAU7802_CALIBRATE_INTERNAL_OFFSET:
        break;
    
    case NAU7802_CALIBRATE_OFFSET:
        reg = reg | NAU7802_CALIBRATE_OFFSET;
        break;
    
    case NAU7802_CALIBRATE_GAIN:
        reg = reg | NAU7802_CALIBRATE_GAIN;
        break;

    default:
        Serial.println("NAU7802: Invalid calmode");
        return false;
        break;
    }
    reg = reg | (1 << 2); //set cals bit to 1 to trigger cal

    writeRegister(0x02, reg); //set calmod bits and start cal

    //delay while cal is happening
    while (((readRegister(0x02) & 0b00000100) >> 2) == 1)
    {
        delay(1);
    }

    if (((readRegister(0x02) & 0b00001000) >> 3) == 1)
    {
        Serial.println("NAU7802: Calibration error");
        return false;
    }

    return true;
}

bool NAU7802::setExtendedADC(NAU7802_ADC_VCM mode) {
    byte reg = readRegister(0x15);
    reg = reg & 0b110011;

    switch (mode)
    {
    case NAU7802_EXTENDED_REFN:
        reg = reg | (NAU7802_EXTENDED_REFN << 2);
        break;

    case NAU7802_EXTENDED_REFP:
        reg = reg | (NAU7802_EXTENDED_REFP << 2);
        break;
    
    default:
        break;
    }

    writeRegister(0x15, reg);
    return true;
}

bool NAU7802::standby(bool enable) {
    byte reg = readRegister(0x00);

    if (enable == true) {
        reg = reg & 0b11111001;
    } else {
        reg = reg | 0b00000110;
    }

    writeRegister(0x00, reg);

    return true;
}

bool NAU7802::pgaBypass(bool enable) {
    byte reg = readRegister(0x1B);
    reg = reg & 0b11101111;

    if (enable == true) {
        reg = reg | (0b1 << 4);
    }
    writeRegister(0x1B, reg);
    return true;
}

// delay by 4 conversion cycles (10SPS -> 400ms)
long NAU7802::read() {
    byte reg0 = readRegister(0x00);
    if ((reg0 | 0b11011111) == 0b11011111) { //if data not ready
        Serial.println("NAU7802: Data not ready");
        return 0;
    }
    writeRegister(0x00, (reg0 | 0b00010000));

    int sampleConversionTime = (int)(1.0 / conversionRate * 4.1 * 1000);
    delay(sampleConversionTime);

    long val = 0;
    byte reg1 = readRegister(0x12);
    byte reg2 = readRegister(0x13);
    byte reg3 = readRegister(0x14);

    

    val = ((uint32_t)reg1 << 16) | ((uint32_t)reg2 << 8) | (uint32_t)reg3;
    if (val & 0x800000) { // Check if bit 23 is set (sign bit)
        val |= 0xFF000000; // Set upper 8 bits to 1 for negative numbers
    }
    return val;
}

void NAU7802::writeRegister(int reg, int value) {
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

byte NAU7802::readRegister(int reg) {
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(reg);
    Wire.endTransmission(false);

    Wire.requestFrom(I2C_ADDRESS, 1);
    return Wire.read();
}