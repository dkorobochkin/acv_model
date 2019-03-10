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

// This file contains implementations of methods for class AImageCombiner

#include "AImageCombiner.h"
#include "ImageCombiner.h"
#include "AImageManager.h"
#include "AImage.h"

#include <cassert>

AImageCombiner::AImageCombiner()
    : mCombiner(std::make_shared<acv::ImageCombiner>())
{
}

void AImageCombiner::AddImage(const AImage& img)
{
    if (mCombiner && img.IsInitialized())
        mCombiner->AddImage(*AImageManager::GetEngineImage(img));
}

void AImageCombiner::ClearImages()
{
    if (mCombiner)
        mCombiner->ClearImages();
}

acv::ImageCombiner::CombineType ConvertToEngineCombineType(ACombineType combineType)
{
    switch (combineType)
    {
    case ACombineType::INFORM_PRIORITY:
        return acv::ImageCombiner::CombineType::INFORM_PRIORITY;
    case ACombineType::MORPHOLOGICAL:
        return acv::ImageCombiner::CombineType::MORPHOLOGICAL;
    case ACombineType::LOCAL_ENTROPY:
        return acv::ImageCombiner::CombineType::LOCAL_ENTROPY;
    case ACombineType::DIFFERENCES_ADDING:
        return acv::ImageCombiner::CombineType::DIFFERENCES_ADDING;
    case ACombineType::CALC_DIFF:
        return acv::ImageCombiner::CombineType::CALC_DIFF;
    }

    assert(false);
    return acv::ImageCombiner::CombineType::INFORM_PRIORITY;
}

ACombinationResult ConvertToServiseCombinationResult(acv::CombinationResult res)
{
    switch (res)
    {
    case acv::CombinationResult::FEW_IMAGES:
        return ACombinationResult::FEW_IMAGES;
    case acv::CombinationResult::INCORRECT_COMBINER_TYPE:
        return ACombinationResult::INCORRECT_COMBINER_TYPE;
    case acv::CombinationResult::MANY_IMAGES:
        return ACombinationResult::MANY_IMAGES;
    case acv::CombinationResult::NOT_SAME_IMAGES:
        return ACombinationResult::NOT_SAME_IMAGES;
    case acv::CombinationResult::SUCCESS:
        return ACombinationResult::SUCCESS;
    }

    assert(false);
    return ACombinationResult::OTHER_ERROR;
}

ACombinationResult AImageCombiner::Combine(ACombineType combineType, AImage& combImg, bool needSort)
{
    auto& dstImg = AImageManager::GetEngineImage(combImg);

    if (mCombiner && dstImg)
    {
        acv::CombinationResult res = mCombiner->Combine(ConvertToEngineCombineType(combineType), *dstImg, needSort);
        return ConvertToServiseCombinationResult(res);
    }

    return ACombinationResult::OTHER_ERROR;
}
