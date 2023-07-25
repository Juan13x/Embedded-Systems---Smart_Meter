#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "program_variables.h"

union u_FloatInt{
    float _f;
    unsigned int _i;
};

struct s_SampleData{
    u_FloatInt voltage;
    u_FloatInt current;
    u_FloatInt power;
    u_FloatInt apparentPower;
    u_FloatInt reactivePower;
    u_FloatInt frequency;
    u_FloatInt harmonics[k_main_MAX_NUM_HARMONICS+1];
    uint32_t lastTime = 0;
    unsigned short day = 125;
    unsigned short year = 1900;
};

class c_Measurements{
private:
    static const unsigned int mVOLTAGE_BIT = 0x1U;
    static const unsigned int mCURRENT_BIT = 0x2U;
    static const unsigned int mPOWER_BIT = 0x4U;
    static const unsigned int mAPPARENT_POWER_BIT = 0x8U;
    static const unsigned int mREACTIVE_POWER_BIT = 0x10U;
    static const unsigned int mFREQUENCY_BIT = 0x20U;
    static const unsigned int mHARMONICS_BIT = 0x80U;

    s_SampleData mData;
    int32_t mControl = 0;
    char mHarmonicsLimit = k_main_MAX_NUM_HARMONICS;
public:
    //---------------------------------
    //Get Methods
    //---------------------------------
    
    //return the data structure
    s_SampleData * getMeasurementData() { return &mData; }

    //return the voltage as float
    float getVoltageAsFloat() { return mData.voltage._f; }
    //return the current as float
    float getCurrentAsFloat() { return mData.current._f; }
    //return the power as float
    float getPowerAsFloat() { return mData.power._f; }
    //return the apparent power as float
    float getApparrentPowerAsFloat() { return mData.apparentPower._f; }
    //return the reactive power as float
    float getReactivePowerAsFloat() { return mData.reactivePower._f; }
    //return the frequency as float
    float getFrequencyAsFloat() { return mData.frequency._f; }
    //return one harmonic as float
    float getHarmonicAsFloat(unsigned char pos){ return mData.harmonics[pos]._f; }

    //return the voltage as int
    int getVoltageAsInt() { return mData.voltage._i; }
    //return the current as int
    int getCurrentAsInt() { return mData.current._i; }
    //return the power as int
    int getPowerAsInt() { return mData.power._i; }
    //return the apparent power as int
    int getApparrentPowerAsInt() { return mData.apparentPower._i; }
    //return the reactive power as int
    int getReactivePowerAsInt() { return mData.reactivePower._i; }
    //return the frequency as int
    int getFrequencyAsInt() { return mData.frequency._i; }
    //return one harmonic as int
    int getHarmonicAsInt(unsigned char pos){ return mData.harmonics[pos]._i; }
    //get the timestamp in miliseconds of the last measurement
    uint32_t getLastTime(){ return mData.lastTime; }
    //get the last value of the day registered
    unsigned short getDay(void){ return mData.day; }
    //get current input identifier for NVS
    unsigned short getYear(void){ return mData.year; }

    //---------------------------------
    //Set Methods
    //---------------------------------

    //set the voltage value
    void setVoltage(float value) { mData.voltage._f = value; }
    //set the current value
    void setCurrent(float value) { mData.current._f = value; }
    //set the power value
    void setPower(float value) { mData.power._f = value; }
    //set the apparent power value
    void setApparrentPower(float value) { mData.apparentPower._f = value; }
    //set the reactive power value
    void setReactivePower(float value) { mData.reactivePower._f = value; }
    //set the frequency value
    void setFrequency(float value) { mData.frequency._f = value; }
    //set one harmonic value
    void setHarmonic(unsigned char pos, float value){ mData.harmonics[pos]._f = value; }
    //set the timestamp in miliseconds of the last measurement
    void setLastTime(uint32_t value){ mData.lastTime = value; }
    //set the day for the new measurements
    bool setDay(unsigned short value);
    //set the Year for the new measurements
    void setYear(unsigned short value){ mData.year = value; }

    //Control
    //---------------------------------
    //GET METHODS
    //---------------------------------
    bool isVoltageReadingEnable(){ return (mControl & mVOLTAGE_BIT) == mVOLTAGE_BIT; }
    bool isCurrentReadingEnable(){ return (mControl & mCURRENT_BIT) == mCURRENT_BIT; }
    bool isPowerReadingEnable(){ return (mControl & mPOWER_BIT) == mPOWER_BIT; }
    bool isApparentPowerReadingEnable(){ return (mControl & mAPPARENT_POWER_BIT) == mAPPARENT_POWER_BIT; }
    bool isReactivePowerReadingEnable(){ return (mControl & mREACTIVE_POWER_BIT) == mREACTIVE_POWER_BIT; }
    bool isFrequencyReadingEnable(){ return (mControl & mFREQUENCY_BIT) == mFREQUENCY_BIT; }
    bool isHormonicsReadingEnable(){ return (mControl & mHARMONICS_BIT) == mHARMONICS_BIT; }
    char getHarmonicsLimit(){ return mHarmonicsLimit; }

    //---------------------------------
    //SET METHODS
    //---------------------------------
    void enableVoltageReading(){ mControl |= mVOLTAGE_BIT;}
    void enableCurrentReading(){ mControl |= mCURRENT_BIT; }
    void enablePowerReading(){ mControl |= mPOWER_BIT; }
    void enableApparentPowerReading(){ mControl |= mAPPARENT_POWER_BIT; }
    void enableReactivePowerReading(){ mControl |= mREACTIVE_POWER_BIT; }
    void enableFrequencyReading(){ mControl |= mFREQUENCY_BIT; }
    void enableHormonicsReading(){ mControl |= mHARMONICS_BIT; }
    void enableAllReading(){ mControl = 0xFFFF; }
    bool setHarmonicsLimit(char value);

    //---------------------------------
    //CLEAR METHODS
    //---------------------------------
    void disableVoltageReading(){ mControl &= ~mVOLTAGE_BIT;}
    void disableCurrentReading(){ mControl &= ~mCURRENT_BIT; }
    void disablePowerReading(){ mControl &= ~mPOWER_BIT; }
    void disableApparentPowerReading(){ mControl &= ~mAPPARENT_POWER_BIT; }
    void disableReactivePowerReading(){ mControl &= ~mREACTIVE_POWER_BIT; }
    void disableFrequencyReading(){ mControl &= ~mFREQUENCY_BIT; }
    void disableHormonicsReading(){ mControl &= ~mHARMONICS_BIT; }
    void disableAllReading(){ mControl = 0x0; }
};

struct s_SampleDataAndControl{
    s_SampleData measurements[k_main_AMOUNT_MEASUREMENTS_FOR_PAYLOAD];
    unsigned char counter = 0;
};

struct s_SampleArrays{
    s_SampleData * sampleReturn;

    s_SampleDataAndControl original;
    s_SampleDataAndControl internet;
    s_SampleDataAndControl lora;
    s_SampleDataAndControl bluetooth;    
};

#endif //MEASUREMENT_H