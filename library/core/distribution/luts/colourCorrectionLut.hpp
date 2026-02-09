#pragma once
#include "lut.hpp"

//ColorCorrectionLut applies the same gamma correction to all channels, and also
//gives you the option to scale the output of the 3 channels. You can use this if
//get a better white balance.
//This color correction luts follows the color order of the output. I.e.: If You use GRB as output color,
//then color1 will be G. 
class ColorCorrectionLUT  : public LUT
{
    public:
        ColorCorrectionLUT(float gammaCorrection, int maxValue, uint8_t Color1, uint8_t Color2, uint8_t Color3) 
        {
            Dimension = 3;
            luts = new uint16_t*[Dimension];
            for (int i=0; i<256; i++){
                lut1[i] = pow((float)i*Color1 / 255. / 255., gammaCorrection) * maxValue;
                lut2[i] = pow((float)i*Color2 / 255. / 255., gammaCorrection) * maxValue;
                lut3[i] = pow((float)i*Color3 / 255. / 255., gammaCorrection) * maxValue;
            }
            
            luts[0] = lut1;
            luts[1] = lut2;
            luts[2] = lut3;
        }
    private:
        uint16_t lut1[256];
        uint16_t lut2[256];
        uint16_t lut3[256];
};