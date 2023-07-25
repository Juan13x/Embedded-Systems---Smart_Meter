#include "classes/measurements/measurements.h"

//set the day for the new measurements
bool c_Measurements::setDay(unsigned short value){ 
    if(value > 367) return false;
    //Is a leap Year
    //Is divided by 100 and by 400
    bool isNewYear = 0;
    //evenly divisible
    bool isDivisibleBy100 = (mData.year % 100) == 0;
    bool isDivisibleBy400 = (mData.year % 400) == 0;
    bool isDivisibleBy4 = ((mData.year >> 2) & 0x3) == 0;
    if(isDivisibleBy100 and isDivisibleBy400){
        if(value == 367) isNewYear = 1;
    }
    else if (isDivisibleBy4 and !isDivisibleBy100){
        if(value == 367) isNewYear = 1;
    }
    else{
        if(value == 366) isNewYear = 1;
    }
    if(isNewYear){
        mData.day = 1;
        mData.year = mData.year + 1;
    }
    return true; 
}

bool c_Measurements::setHarmonicsLimit(char value){
    if(value > k_main_MAX_NUM_HARMONICS) return false;
    mHarmonicsLimit = value ;
    return true;
}