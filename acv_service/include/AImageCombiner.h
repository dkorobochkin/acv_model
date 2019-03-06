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

// This header file is used to define a wrapper for class ImageCombiner from engine level

#ifndef AIMAGE_COMBINER_H
#define AIMAGE_COMBINER_H

#include "ImageCombiner.h"

#include <memory>

// Used types of combining
enum class ACombineType
{
    INFORM_PRIORITY, // Combining with priority of image with the biggest entropy
    MORPHOLOGICAL, // Morphological combining
    LOCAL_ENTROPY, // Local-entropy combining
    DIFFERENCES_ADDING, // Adding the differences
    CALC_DIFF // Calculation the difference of two open images
};

// This enum is used to represent the result of images combining
enum class ACombinationResult
{
    SUCCESS, // Success of combining
    INCORRECT_COMBINER_TYPE, // Incorrect type of combiner
    FEW_IMAGES, // The number of images should be more than 1
    NOT_SAME_IMAGES, // The images should have the same sizes
    MANY_IMAGES, // Combining based on adding the differences use only two images
    OTHER_ERROR // Error during combining
};

class AImage;

// Wrapper for class ImageCombiner from engine level
class AImageCombiner
{

public:

    // Default constructor
    AImageCombiner();

    // Add image to combine
    void AddImage(const AImage& img);

    // Clear the container with images to combine
    void ClearImages();

    // Run of combining with specified type
    // Flag needSort is used to sort container of image by entropy
    ACombinationResult Combine(ACombineType combineType, AImage& combImg, bool needSort = true);

private:

    // Low level representation of combiner
    std::shared_ptr<acv::ImageCombiner> mCombiner;

};

#endif // AIMAGE_COMBINER_H
