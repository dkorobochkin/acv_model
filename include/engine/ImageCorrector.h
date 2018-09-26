//
// MIT License
//
// Copyright (c) 2018 Dmitriy Korobochkin, Vitaliy Garmash.
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

// This header file is used to define a class to correct of image

#ifndef IMAGE_CORRECTOR_H
#define IMAGE_CORRECTOR_H

namespace acv {

class Image;

// Class is used to correct image by several methods
// Class contains only static methods
class ImageCorrector
{

public: // Public auxiliary types

    // Used types of correction
    enum class CorrectorType
    {
        SSRETINEX // Single-scale Retinex
    };

public: // Public methods

    static bool Correct(const Image& srcImg, Image& dstImg, CorrectorType corType);

public: // Public methods

    // SSR algorith
    static bool SingleScaleRetinex(const Image& srcImg, Image& dstImg);

};

}

#endif // IMAGE_CORRECTOR_H
