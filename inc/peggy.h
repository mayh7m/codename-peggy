#pragma once

#include <random>

#include <godot_cpp/classes/animated_sprite2D.hpp>
#include <godot_cpp/classes/audio_stream_player2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/progress_bar.hpp>
#include <godot_cpp/classes/property_tweener.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/sprite2D.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/math.hpp>

namespace godot {

    enum SquishAction {
        JUMP,
        LAND,
        CHANGE_DIRECTION
    };

    class Peggy : public CharacterBody2D {
        GDCLASS(Peggy, CharacterBody2D);

        float directionX = .0f;
        float oldDirectionX = .0f;
        bool facingRight = true;
        float speed = 100.f;
        float jumpForce = 750.f;
        float gravity = 10.f;
        bool hasLaunched = false;
        bool airbourne = true;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<> distr;

        StringName animation = "idle";

    protected:
        static void _bind_methods();

    public:
        Peggy();
        ~Peggy() {};

        virtual void _ready() override;
        virtual void _physics_process(double) override;
        virtual void _process(double) override;

        void getInput();
        void applyGravity();
        void getFacingDirection();
        void getAnimation();

        void squish(SquishAction);

        void playSfxFootstep();

        void setSpeed(const float);
        float getSpeed() const;
        void setJumpForce(const float _jumpForce);
        float getJumpForce() const;
        void setGravity(const float _gravity);
        float getGravity() const;
    };
}