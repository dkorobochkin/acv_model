//
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

// This header file is used to define a wrapper for class ImageCorrector from engine level

#ifndef AIMAGE_CORRECTOR_H
#define AIMAGE_CORRECTOR_H

// Used types of correction
enum class ACorrectorType
{
    SSRETINEX, // Single-scale Retinex
    AUTO_LEVELS, // Contrast correction using the auto-levels algorithm
    NORM_AUTO_LEVELS, // Algorithm of auto-levels with pixels correction in three sigma range
    GAMMA // Gamma-correction
};

class AImage;

// Wrapper for class ImageCorrector from engine level
class AImageCorrector
{

public:

    // Correct image using a special method
    static bool Correct(const AImage& srcImg, AImage& dstImg, ACorrectorType corType);

};

#endif // AIMAGECORRECTOR_H
