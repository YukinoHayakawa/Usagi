#pragma once

#include <Usagi/Core/PredefinedElement.hpp>

namespace usagi
{
struct AnimationComponent;
class GpuImage;
}

namespace usagi::moeloop
{
class ImageFrame;

class TransitionableImage : public PredefinedElement<AnimationComponent>
{
    ImageFrame *mCurrentImage, *mTargetImage;

public:
    TransitionableImage(Element *parent, std::string name);

    void switchImage(
        double duration,
        const std::string &easing,
        std::shared_ptr<GpuImage> image);

    ImageFrame * currentImage() const { return mCurrentImage; }
    ImageFrame * targetImage() const { return mTargetImage; }
};
}
