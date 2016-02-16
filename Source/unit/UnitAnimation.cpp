#include "TheEngine/unit/UnitAnimation.h"
#include "TheEngine/filesystem/stream.h"

#include "TheEngine/logger.h"

#include "TheEngine/unit/UnitAnimation.h"

UnitAnimation::UnitAnimation(the::animesh::cref mesh, const TransitionPtr &transition, strref &name, float frame, float frame2, bool revert) :
    transition(transition),
    mesh(mesh),
    revert(revert),
    name(name)

{
    auto anim   = mesh->getAnimation(name);
    endFrame = frame2;
    fps = anim->getFps();

    currentFrame = frame;
    lastNotificationFrame = currentFrame;
}

bool UnitAnimation::updateRevert(float delta_t)
{
    if (currentFrame == endFrame && sound != nullptr) sound->play();
    
    currentFrame -= delta_t * fps;
 
    bool finished = false;

    if (currentFrame >= endFrame)
        mesh->setFrame(name, currentFrame);
    else
    {
        mesh->setFrame(name, endFrame);
        if (!transition->isLooped())
            finished = true;
        else
            currentFrame = endFrame;
    }

    if (static_cast<float>(lastNotificationFrame) - currentFrame >= 1.0f)
    {
        for(int i = lastNotificationFrame-1;i>=currentFrame;i--)
        {
            transition->update(i);
            lastNotificationFrame = i;
        }
    }

    return finished;
}

bool UnitAnimation::updateForward(float delta_t)
{
    if (currentFrame == 0 && sound != nullptr) sound->play();

    currentFrame += delta_t * fps;

    bool finished = false;

    if (currentFrame <= endFrame)
        mesh->setFrame(name, currentFrame);
    else
    {
        mesh->setFrame(name, endFrame);
        if (!transition->isLooped())
            finished = true;
        else
            currentFrame = 0;
    }

    if (lastNotificationFrame != (int)currentFrame)
    {
        for(int i = lastNotificationFrame+1;i<=currentFrame;i++)
            transition->update(i);
        lastNotificationFrame = (int)currentFrame;
    }

    return finished;
}

bool UnitAnimation::update(float delta_t)
{
    if(revert)
        return updateRevert(delta_t);
    else
        return updateForward(delta_t);
}

void UnitAnimation::stop()
{
    mesh->setFrame(name, 0);
    if (sound)
        sound->stop();
}