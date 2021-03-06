﻿//
// MIT License
//
// Copyright (c) 2018-2019 Dmitriy Korobochkin, Vitaliy Garmash.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

// This header is used to define a class that filters images

#ifndef IMAGE_FILTER_H
#define IMAGE_FILTER_H

namespace acv {

class Image;

// This enum is used to represent the result of image filtering
enum class FiltrationResult
{
    SUCCESS, // Success of filtration
    INTERNAL_ERROR, // Error during filtration
    INCORRECT_FILTER_TYPE, // Incorrect type of filter
    INCORRECT_FILTER_SIZE, // Incorrect filter size (he should be odd)
    SMALL_FILTER_SIZE // Filter size is small (this code is used for IIR Gaussian filtration)
};

// Class is used to filter images by several methods
// Class contains only static methods
class ImageFilter
{

public: // Public auxiliary types

    // Used types of filtration
    enum class FilterType
    {
        MEDIAN, // Median filtration
        GAUSSIAN, // Gaissian filtration
        SEP_GAUSSIAN, // Separated gaussian filtration
        IIR_GAUSSIAN, // IIR-imitated gaussian filtration
        SHARPEN // Increase the sharpness of the image
    };

    // Used types of threshold
    enum class ThresholdType
    {
        MAX_MORE_THRESHOLD, // All pixels more than threshold will be Image::MAX_VALUE
        MIN_MORE_THRESHOLD  // All pixels more than threshold will be Image::MIN_VALUE
    };

    template <typename T>
    class IIRfilter{
        public:
            IIRfilter(T sigma); // Calculation of the IIR-filter's ratios to imit gaussian with specified sigma
            T Solve(T input); // Calculation of the IIR-filter's output signal
            void Reset(){ y[0] = y[1] = y[2] = 0.; }
            void Reset(T s0, T s1, T s2){ y[0] = s0, y[1] = s1, y[2] = s2; }
        private:
            T b0, a[3], y[3];
    };

public: // Public methods

    // Run a filtration by the specified method
    // Source image WILL BE CHANGED!!!
    static FiltrationResult Filter(Image& img, FilterType type, const int filterSize = -1);

    // Run a filtration by the specified method
    static FiltrationResult Filter(const Image& srcImg, Image& dstImg, FilterType type, const int filterSize = -1);

    // Run an adaptive threshold processing
    static bool AdaptiveThreshold(Image& img, const int filterSize, const int threshold, ThresholdType thresholdType);
    static bool AdaptiveThreshold(const Image& srcImg, Image& dstImg, const int filterSize, const int threshold, ThresholdType thresholdType);

private: // Private methods

    // Median filtration (filter size must be odd)
    // Source image WILL BE CHANGED!!!
    static FiltrationResult Median(Image& img, const int filterSize);

    // Median filtration (filter size must be odd)
    static FiltrationResult Median(const Image& srcImg, Image& dstImg, const int filterSize);

    // Gaussian filtration (filter size must be odd)
    // Source image WILL BE CHANGED!!!
    static FiltrationResult Gaussian(Image& img, const int filterSize);

    // Gaussian filtration (filter size must be odd)
    static FiltrationResult Gaussian(const Image& srcImg, Image& dstImg, const int filterSize);

    // Separate Gaussian filtration
    static FiltrationResult SeparateGaussian(const Image& srcImg, Image& dstImg, const int filterSize);

    // Gaussian imitation by IIR-filter
    static FiltrationResult GaussianIIR(Image& img, float sigma);
    static FiltrationResult GaussianIIR(const Image& srcImg, Image& dstImg, float sigma);

    // Increase the sharpness of the image
    static FiltrationResult Sharpen(Image& img);
    static FiltrationResult Sharpen(const Image& srcImg, Image& dstImg);
};


template <typename T>
ImageFilter::IIRfilter<T>::IIRfilter(T sigma)
{
     if (sigma){
        T sigma_inv_4;

        sigma_inv_4 = sigma * sigma;
        sigma_inv_4 = 1.0 / (sigma_inv_4 * sigma_inv_4);

        T coef_A = sigma_inv_4 * (sigma * (sigma* (sigma * 1.1442707 + 0.0130625) - 0.7500910) + 0.2546730);
        T coef_W = sigma_inv_4 * (sigma * (sigma* (sigma * 1.3642870 + 0.0088755) - 0.3255340) + 0.3016210);
        T coef_B = sigma_inv_4 * (sigma * (sigma* (sigma * 1.2397166 - 0.0001644) - 0.6363580) - 0.0536068);

        T z0_abs   = exp(coef_A);

        T z0_real  = z0_abs * cos(coef_W);
        //T z0_im    = z0_abs*sin(coef_W);
        T z2       = exp(coef_B);

        T z0_abs_2 = z0_abs * z0_abs;

        a[2] = 1.0 / (z2 * z0_abs_2);
        a[0] = (z0_abs_2 + 2 * z0_real * z2) * a[2];
        a[1] = - (2 * z0_real + z2) * a[2];

        b0 = 1.0 - a[0] - a[1] - a[2];
    }

}

template <typename T>
T ImageFilter::IIRfilter<T>::Solve(T input)
{
    T output= input * b0 + a[0] * y[0] + a[1] * y[1] + a[2] * y[2];

    y[2]= y[1];
    y[1]= y[0];
    y[0]= output;

    return output;
}

}

#endif // IMAGE_FILTER_H
