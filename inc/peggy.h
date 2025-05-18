#pragma once

#include <random>

#include <godot_cpp/classes/animated_sprite2D.hpp>
#include <godot_cpp/classes/audio_stream_player2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>

namespace godot {

    // enum for adequate scale change used in squish()
    enum class SquishAction {
        JUMP,
        LAND,
        CHANGE_DIRECTION
    };

    // the player class
    class Peggy : public CharacterBody2D {
        GDCLASS(Peggy, CharacterBody2D);

        // movement variables for setters/getters
        float speed = 750.f;
        float jumpForce = 1500.f;
        float gravity = 80.f;

        // direction variables used for movement and animations
        float directionX = .0f;
        float oldDirectionX = .0f;
        bool facingRight = true;

        // variables used for gliding
        bool hasLaunched = false;
        bool airbourne = true;

        // current animation name
        StringName animation = "glide";

        // rand variables to determine the pitch scale of the sounds
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<> distr;

    protected:
        // obligatory method used for binding methods in the godot editor
        static void _bind_methods();

    public:
        // construcors/destructor
        Peggy();
        ~Peggy() {};

        // methods overrided from the godot classes
        virtual void _ready() override;
        virtual void _physics_process(double delta) override;
        virtual void _process(double delta) override;

        // movement/animation methods
        void getInput();
        void applyGravity();
        void getFacingDirection();
        void getAnimation();
        void jump();

        // additional methods for cosmetics
        void squish(const SquishAction);
        void unsquish(const double delta);
        void changeDirectionBodyResponse();
        void playSound(const NodePath& sound, const double defaultPitchValue);

        // methods triggered after adequate godot signals
        void on_animated_sprite_2d_frame_changed();

        // setters/getters
        inline void setSpeed(const float _speed) { speed = _speed; }
        inline float getSpeed() const { return speed; }

        inline void setJumpForce(const float _jumpForce) { jumpForce = _jumpForce; }
        inline float getJumpForce() const { return jumpForce; }

        inline void setGravity(const float _gravity) { gravity = _gravity; }
        inline float getGravity() const { return gravity; }
    };
}