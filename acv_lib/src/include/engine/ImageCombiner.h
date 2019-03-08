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

// This header file use to define a class to combine of images

#ifndef IMAGE_COMBINER_H
#define IMAGE_COMBINER_H

#include <vector>
#include <iostream>

namespace acv {

class Image;
class AMorphologicalForm;

// This enum is used to represent the result of images combining
enum class CombinationResult
{
    SUCCESS, // Success of combining
    INCORRECT_COMBINER_TYPE, // Incorrect type of combiner
    FEW_IMAGES, // The number of images should be more than 1
    NOT_SAME_IMAGES, // The images should have the same sizes
    MANY_IMAGES // Combining based on adding the differences use only two images
};

// Class to combine of images by several methods
class ImageCombiner
{

public: // Public auxiliary types

    // Used types of combining
    enum class CombineType
    {
        INFORM_PRIORITY, // Combining with priority of image with the biggest entropy
        MORPHOLOGICAL, // Morphological combining
        LOCAL_ENTROPY, // Local-entropy combining
        DIFFERENCES_ADDING, // Adding the differences
        CALC_DIFF //Calculation the difference of two open images
    };

public: // Public methods

    // Add image to combine
    void AddImage(const Image& img);

    // Clear the container with images to combine
    void ClearImages();

    // Run of combining with specified type
    // Flag needSort is used to sort container of image by entropy
    Image Combine(CombineType combineType, CombinationResult& combRes, const bool needSort = true);    
    CombinationResult Combine(CombineType combineType, Image& combImg, const bool needSort = true);

private: // Private methods

    // Combining with priority of image with the biggest entropy
    // Flag needSort is used to sort container of image by entropy. If flag value is "false" first image in container is basic
    Image InformativePriority(CombinationResult& combRes, const bool needSort = true);
    CombinationResult InformativePriority(Image& combImg, const bool needSort = true);

    // Morphological combining
    // Flag needSort is used to sort container of image by entropy. If flag value is "false" first image in container is basic
    Image Morphological(const size_t numMods, CombinationResult& combRes, const bool needSort = true);
    CombinationResult Morphological(const size_t numMods, Image& combImg, const bool needSort = true);

    // Local-entropy combining
    // Each pixel is pixel from image with the biggest local entropy in this pixel
    Image LocalEntropy(CombinationResult& combRes);
    CombinationResult LocalEntropy(Image& combImg);

    // Adding the differences
    Image DifferencesAdding(CombinationResult& combRes, const bool needSort = true);
    CombinationResult DifferencesAdding(Image& combImg, const bool needSort = true);

    // Calculating the difference of two open images
    Image CalcDiff(CombinationResult& combRes);
    CombinationResult CalcDiff(Image& combImg);


    // Check the possibility of combining images in container
    // All images should have same dimensions
    bool CanCombine(CombinationResult& combRes) const;

    // Form the images vector that is sorted by descending of images entropy
    void FormSortedImagesArray(std::vector<const Image*>& sortedVec);

private: // Private methods to morphological combining

    // Calculation the morphological forms
    // The first stage is the criterion histogram segmentation with specified number of histogram mod's
    // At the second stage we make search of forms by using the calculated histogram
    std::vector<AMorphologicalForm> CalcForms(const Image& baseImg, const int numMods);

    // Run the criterion histogram segmentation
    // The result is the matrix each pixel of which is histogram mod number
    Image Segmentation(const Image& baseImg, const int numMods);

    // Search of the image forms by using scanning algorithm
    std::vector<AMorphologicalForm> FindForms(const Image& histogramm, const int numMods);

    // Calculation the average brightness of projected image in the form
    void CalcProjectionToForms(const std::vector<AMorphologicalForm>& morphForm, const Image& projImg, Image& projection);

    // This method is used to merging the images brightness
    // The pixel value of projected image is the difference beside brightness on basic image and average brightness of projected image in form
    void MergeImages(Image& baseImg, const std::vector<Image>& projections);

private: // Private members

    // Vector of combined image
    std::vector<const Image*> mCombinedImages;

private: // Constants

    enum
    {
        DEFAULT_NUM_MODS = 16 // Default numbers of morphological mods
    };

};

}

#endif // IMAGE_COMBINER_H
