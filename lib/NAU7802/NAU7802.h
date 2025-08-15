#ifndef NAU7802_H
#define NAU7802_H

enum NAU7802_Gain 
{
    NAU7802_GAIN_1,
    NAU7802_GAIN_2,
    NAU7802_GAIN_4,
    NAU7802_GAIN_8,
    NAU7802_GAIN_16,
    NAU7802_GAIN_32,
    NAU7802_GAIN_64,
    NAU7802_GAIN_128
};

enum NAU7802_SupplyVoltage 
{
    NAU7802_SUPPLY_4V5,
    NAU7802_SUPPLY_4V2,
    NAU7802_SUPPLY_3V9,
    NAU7802_SUPPLY_3V6,
    NAU7802_SUPPLY_3V3,
    NAU7802_SUPPLY_3V0,
    NAU7802_SUPPLY_2V7,
    NAU7802_SUPPLY_2V4,
    NAU7802_SUPPLY_EXTERNAL
};

enum NAU7802_ConversionRate
{
    NAU7802_CONVERSIONRATE_10,
    NAU7802_CONVERSIONRATE_20,
    NAU7802_CONVERSIONRATE_40,
    NAU7802_CONVERSIONRATE_80,
    NAU7802_CONVERSIONRATE_320,
};

enum NAU7802_Channel
{
    NAU7802_CHANNEL_1,
    NAU7802_CHANNEL_2
};

//look at datasheet to see details of ldo modes
enum NAU7802_LDOMODE
{
    NAU7802_LDOMODE_0,
    NAU7802_LDOMODE_1
};

enum NAU7802_ADC_VCM
{
    NAU7802_EXTENDED_DISABLED0,
    NAU7802_EXTENDED_DISABLED1,
    NAU7802_EXTENDED_REFN,
    NAU7802_EXTENDED_REFP
};

enum NAU7802_Calibrate
{
    NAU7802_CALIBRATE_INTERNAL_OFFSET,
    RESERVED,
    NAU7802_CALIBRATE_OFFSET,
    NAU7802_CALIBRATE_GAIN
};

class NAU7802 {
    public:
        NAU7802(); //Constructor
        bool begin();

        bool setGain(NAU7802_Gain gain);
        NAU7802_Gain getGain();

        bool setAnalogSupply(NAU7802_SupplyVoltage supply);
        NAU7802_SupplyVoltage getAnalogSupply();

        bool setConversionRate(NAU7802_ConversionRate rate);
        NAU7802_ConversionRate getConversionRate();

        bool setChannel(NAU7802_Channel channel);
        NAU7802_Channel getChannel();

        bool setLdoMode(NAU7802_LDOMODE mode);
        NAU7802_LDOMODE getLdoMode();

        bool calibrate(NAU7802_Calibrate calmode);

        bool setExtendedADC(NAU7802_ADC_VCM mode);

        bool standby(bool enable);
        bool pgaBypass(bool enable);
        long read();

    private:
        void writeRegister(int reg, int value);
        byte readRegister(int reg);
        const int I2C_ADDRESS = 0x2A;
        int conversionRate = 10;
};

#endif // NAU7802_H