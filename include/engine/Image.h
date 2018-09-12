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

// This header file use to define a class of image, his methods and auxiliary types

#ifndef IMAGE_H
#define IMAGE_H

#include <vector>

namespace acv {

// Class of one-channel image. Each pixel is represents one byte
class Image
{

public: // Constants

    enum
    {
        MIN_PIXEL_VALUE = 0, // Minimum value of pixel brightness
        MAX_PIXEL_VALUE = 255 // Maximum value of pixel brightness
    };

public: // Auxiliary enums

    //Types of data buffer that used to creation of image
    enum class BufferType
    {
        RGB, // RGB representation
        DIRECT_SHOW // DirectShow representation
    };

public: // Auxiliary types

    typedef unsigned char Byte; // This type is used to representation of pixel brightness

    typedef std::vector<Byte> Row; // This type is used to representation of pixels row

    typedef std::vector<Row> Matrix; // This type is used to representation of pixels matrix

public: // Constructors

    // Default constructor
    Image();

    // Constructor of image with specified dimensions
    Image(const int height, const int width);

    // Constructor of image with specified dimensions from buffer of data
    Image(const int height, const int width, const void* buf, BufferType bufType);

public: // Public methods

    // Get the width of image
    int GetWidth() const;

    // Get the height of image
    int GetHeight() const;

    // Get the pixel value by coordinates
    Byte GetPixel(const int rowNum, const int colNum) const;

    // Set the pixel value by coordinates
    void SetPixel(const int rowNum, const int colNum, const Byte val);

    // Get the reference to row by the row number
    Row& GetRow(const int rowNum);
    const Row& GetRow(const int rowNum) const;

    // Get the reference to row by the row number (array style)
    Row& operator[](const int rowNum);
    const Row& operator[](const int rowNum) const;

    // Get the reference to pixel by coordinates
    Byte& operator()(const int rowNum, const int colNum);
    const Byte& operator()(const int rowNum, const int colNum) const;

    // Check the initialization of image
    // Image is not initialized if was created by default constructor
    bool IsInitialized() const;

    // Check pixel coordinates out of image boundaries
    bool IsInvalidCoordinates(const int rowNum, const int colNum);

    // Adjust pixel coordinates if they are out of image boundaries
    // In this case the coordinates are mirrored
    void CorrectCoordinates(int& rowNum, int& colNum) const;

    // Check the pixel value to out the minimum and maximum values
    static void CheckPixelValue(int& value);

private: // Private methods

    // Calculation of auxiliary parameters that are used to adjust pixels coordinates
    void CalcAuxParameters();

    // Fill the pixels from buffer of specific type
    void FillPixels(const void* buf, BufferType bufType);

    // Fill the pixels from buffer of RGB type
    void FillPixelFromRGB(Byte* buf);

    // Fill the pixels from buffer of DirectShow type
    void FillPixelFromDirectShow(Byte* buf);

public: // Private members (representation of image)

    // Matrix of pixels
    Matrix mPixels;

    // Image width (value -1 if image was not initialized)
    int mWidth;

    // Image height (value -1 if image was not initialized)
    int mHeight;

private: // Private auxiliary members to fast correction of pixel coordinates

    // This value is used to correct x (colNum) coordinate
    int mAuxWidth;

    // This value is used to correct y (rowNum) coordinate
    int mAuxHeight;

};

}

#endif // IMAGE_H
