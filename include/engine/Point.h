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

// This header file use to define a class of point and his methods

#ifndef POINT_H
#define POINT_H

namespace acv {

// The point of image
class Point
{

public: // Constructors

    // Default constructor
    Point();

    // Constructor from coordinates
    Point(const int x, const int y);

public: // Public methods

    // Methods to get and set of pixel coordinates
    void SetX(const int x) { mX = x; }
    void SetY(const int y) { mY = y; }
    void SetXY(const int x, const int y) { mX = x; mY = y; }
    int GetX() const { return mX; }
    int GetY() const { return mY; }

private: // Private members

    // Coordinates
    int mX;
    int mY;

};

}

#endif // POINT_H
