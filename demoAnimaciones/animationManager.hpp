#ifndef ANIMATION_MANAGER_HPP
#define ANIMATION_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <string>

class AnimationManager {
public:
    /**
     * @struct Frame
     * @brief Represents a single frame of an animation
     *
     * Each frame consists of a texture rectangle and a duration for how long it should be displayed
     */
    struct Frame {
        sf::IntRect rect;
        float duration; // Duration of the frame in seconds
    };

    /**
     * @struct Animation
     * @brief Represents an animation sequence
     *
     * An animation consists of multiple frames and a loop flag
     */
    struct Animation {
        std::vector<Frame> frames;
        bool loop;
    };

    /**
     * @brief Constructs an AnimationManager
     * @param sprite The sprite to which animations will be applied
     */
    AnimationManager(sf::Sprite& sprite);

    /**
     * @brief Adds a new animation to the manager.
     * @param name The name of the animation.
     * @param frames The frames that make up the animation.
     * @param loop Whether the animation should loop (default: true).
     */
    void addAnimation(const std::string& name, const std::vector<Frame>& frames, bool loop = true);
    
    /**
     * @brief Starts playing an animation.
     * @param name The name of the animation to play.
     */
    void playAnimation(const std::string& name);

    /**
     * @brief Updates the animation based on elapsed time.
     * @param deltaTime Time elapsed since the last update (in seconds).
     */
    void update(float deltaTime);

    /**
     * @brief Updates the current animation based on movement conditions.
     * @param isOnGround Indicates if the character is on the ground.
     * @param haciaDerecha Indicates if the character is moving to the right.
     * @param haciaIzquierda Indicates if the character is moving to the left.
     * @param currentAnimation The current animation being played.
     */
    void updateAnimation(bool isOnGround, bool haciaDerecha, bool haciaIzquierda, std::string& currentAnimation);
    
    /**
     * @brief Checks if a specific animation is currently playing.
     * @param name The name of the animation.
     * @return True if the animation is playing, false otherwise.
     */
    bool isPlaying(const std::string& name);

private:
    sf::Sprite& sprite;
    std::unordered_map<std::string, Animation> animations;
    Animation* currentAnimation;
    float elapsedTime;
    std::size_t currentFrame;
};

#endif // ANIMATION_MANAGER_HPP
