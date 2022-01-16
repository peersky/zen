/*
 MIT License

 Copyright (c) 2021 Tim Pechersky

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

/*
    This wrapper should declare API interface to application target
    Whether it is VST/Standalone/Embedded or any other target
    It must implement methods that are declared here
*/

#ifndef APP_TARGET_WRAPPER_
#define APP_TARGET_WRAPPER_
#ifdef __cplusplus
extern "C"
{
#endif

    enum SlidersEnum
    {
        SLIDER_DELAY = 0,
        SLIDER_SPREAD,
        SLIDER_FEEDBACK,
        SLIDER_LFO_FREQ,
        SLIDER_MODULATION_DEPTH,
        SLIDER_FILTER_CUTOFF,
        SLIDER_FILTER_RESO,
        SLIDER_REVERB_DECAY,
        SLIDER_REVERB_SIZE,
        SLIDER_NUM_ENUM
    };

    void printSliderValues(void);

    //float getSliderValue(String name);
    //void setLabelValue_f(float **pVal);
    float getRandomFloat(void);
    inline void setLabelValue(float *pVal);
    float getSliderValue(SlidersEnum i);
    void slidersConstruct();
    inline void setSliderValue(SlidersEnum slider, float val);

#ifdef __cplusplus
}
#endif

#endif //APP_TARGET_WRAPPER_