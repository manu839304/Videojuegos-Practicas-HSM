#include "animationManager.hpp"

AnimationManager::AnimationManager(sf::Sprite& sprite)
    : sprite(sprite), currentAnimation(nullptr), elapsedTime(0.f), currentFrame(0) {}

void AnimationManager::addAnimation(const std::string& name, const std::vector<Frame>& frames, bool loop) {
    animations[name] = { frames, loop };
}

void AnimationManager::playAnimation(const std::string& name) {
    if (animations.find(name) != animations.end()) {
        currentAnimation = &animations[name];
        currentFrame = 0;
        elapsedTime = 0.f;
        sprite.setTextureRect(currentAnimation->frames[0].rect);
    }
}

void AnimationManager::update(float deltaTime) {
    if (!currentAnimation || currentAnimation->frames.empty()) return;

    elapsedTime += deltaTime;
    if (elapsedTime >= currentAnimation->frames[currentFrame].duration) {
        elapsedTime = 0.f;
        currentFrame++;

        if (currentFrame >= currentAnimation->frames.size()) {
            if (currentAnimation->loop) {
                currentFrame = 0;
            } else {
                currentFrame--;
                return; // Mantener el último frame si la animación no es en bucle
            }
        }

        sprite.setTextureRect(currentAnimation->frames[currentFrame].rect);
    }
}

void AnimationManager::updateAnimation(bool isOnGround, bool haciaDerecha, bool haciaIzquierda, std::string& currentAnimation) {
    if (!isOnGround) {
        if (currentAnimation != "jump") {
            playAnimation("jump");
            currentAnimation = "jump";
        }
    }
    else if (haciaDerecha || haciaIzquierda) {
        if (currentAnimation != "walk") {
            playAnimation("walk");
            currentAnimation = "walk";
        }
    }
    else {
        if (currentAnimation != "idle") {
            playAnimation("idle");
            currentAnimation = "idle";
        }
    }
}


bool AnimationManager::isPlaying(const std::string& name){
    return currentAnimation && animations.find(name) != animations.end() && &animations[name] == currentAnimation;
}