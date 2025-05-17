#include "../inc/peggy.h"

using namespace godot;

void Peggy::_bind_methods() {

    ClassDB::bind_method(D_METHOD("playSfxFootstep"), &Peggy::playSfxFootstep);

    ClassDB::bind_method(D_METHOD("getSpeed"), &Peggy::getSpeed);
    ClassDB::bind_method(D_METHOD("setSpeed", "_speed"), &Peggy::setSpeed);
    ClassDB::add_property("Peggy", PropertyInfo(Variant::FLOAT, "speed"), "setSpeed", "getSpeed");

    ClassDB::bind_method(D_METHOD("getJumpForce"), &Peggy::getJumpForce);
    ClassDB::bind_method(D_METHOD("setJumpForce", "_jumpForce"), &Peggy::setJumpForce);
    ClassDB::add_property("Peggy", PropertyInfo(Variant::FLOAT, "jumpForce"), "setJumpForce", "getJumpForce");

    ClassDB::bind_method(D_METHOD("getGravity"), &Peggy::getGravity);
    ClassDB::bind_method(D_METHOD("setGravity", "_gravity"), &Peggy::setGravity);
    ClassDB::add_property("Peggy", PropertyInfo(Variant::FLOAT, "gravity"), "setGravity", "getGravity");
}

Peggy::Peggy() {
    InputMap::get_singleton()->load_from_project_settings();
}

void Peggy::_ready() {
    if (Engine::get_singleton()->is_editor_hint())
        set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
    else 
        set_process_mode(Node::ProcessMode::PROCESS_MODE_INHERIT);

    set_velocity({0, 0});

    gen = std::mt19937(rd()); // seed the generator
    distr = std::uniform_real_distribution<>(0.75, 1.25); // define the range
}

void Peggy::_physics_process(double delta) {
    getInput();
    applyGravity();
    getFacingDirection();
    getAnimation();
    set_velocity({ directionX * speed, get_velocity()[1] });
    move_and_slide();

    if (airbourne && is_on_floor()) {
        airbourne = false;
        squish(LAND);
        get_node<AudioStreamPlayer2D>("Sfx/SfxFall")->set_pitch_scale(2.0);
        get_node<AudioStreamPlayer2D>("Sfx/SfxFall")->set_pitch_scale(get_node<AudioStreamPlayer2D>("Sfx/SfxFall")->get_pitch_scale() * distr(gen));
        get_node<AudioStreamPlayer2D>("Sfx/SfxFall")->play();
    }

}

void Peggy::_process(double delta) {
    get_node<AnimatedSprite2D>("AnimatedSprite2D")->set_scale(
        { static_cast<real_t>(Math::move_toward(static_cast<double>(get_node<AnimatedSprite2D>("AnimatedSprite2D")->get_scale().x), 1.0, 1.5 * delta)), 
          static_cast<real_t>(Math::move_toward(static_cast<double>(get_node<AnimatedSprite2D>("AnimatedSprite2D")->get_scale().y), 1.0, 1.5 * delta)) }
    );
}

void Peggy::getInput() {
    directionX = Input::get_singleton()->get_axis("left", "right");
    if(directionX != oldDirectionX) {
        oldDirectionX = directionX;
        squish(CHANGE_DIRECTION);
        if(directionX) {
            get_node<AudioStreamPlayer2D>("Sfx/SfxChangeDirection")->set_pitch_scale(2.5);
            get_node<AudioStreamPlayer2D>("Sfx/SfxChangeDirection")->set_pitch_scale(get_node<AudioStreamPlayer2D>("Sfx/SfxChangeDirection")->get_pitch_scale() * distr(gen));
            get_node<AudioStreamPlayer2D>("Sfx/SfxChangeDirection")->play();
            if(is_on_floor()) {
                get_node<AudioStreamPlayer2D>("Sfx/SfxFootstep")->set_pitch_scale(2.0);
                get_node<AudioStreamPlayer2D>("Sfx/SfxFootstep")->set_pitch_scale(get_node<AudioStreamPlayer2D>("Sfx/SfxFootstep")->get_pitch_scale() * distr(gen));
                get_node<AudioStreamPlayer2D>("Sfx/SfxFootstep")->play();
            }
        }
        if(!directionX) {
            if(is_on_floor()) {
                get_node<AudioStreamPlayer2D>("Sfx/SfxFootstep")->set_pitch_scale(2.0);
                get_node<AudioStreamPlayer2D>("Sfx/SfxFootstep")->set_pitch_scale(get_node<AudioStreamPlayer2D>("Sfx/SfxFootstep")->get_pitch_scale() * distr(gen));
                get_node<AudioStreamPlayer2D>("Sfx/SfxFootstep")->play();
            }
        }
    }

    if (Input::get_singleton()->is_action_just_pressed("jump") && is_on_floor()) {
        set_velocity({ get_velocity()[0], -jumpForce });
        hasLaunched = true;
        airbourne = true;

        squish(JUMP);
        get_node<AudioStreamPlayer2D>("Sfx/SfxJump")->set_pitch_scale(1.5);
        get_node<AudioStreamPlayer2D>("Sfx/SfxJump")->set_pitch_scale(get_node<AudioStreamPlayer2D>("Sfx/SfxJump")->get_pitch_scale() * distr(gen));
        get_node<AudioStreamPlayer2D>("Sfx/SfxJump")->play();
    }
}

void Peggy::applyGravity() {
    set_velocity({ get_velocity()[0], get_velocity()[1] += gravity } );
}

void Peggy::getFacingDirection() {
    if (directionX != 0) {
        facingRight = directionX > 0;
    }
}

void Peggy::getAnimation() {

    animation = "idle";

    if (!is_on_floor()) {
        if (hasLaunched) {
            animation = "jump";
            if(get_node<AnimatedSprite2D>("AnimatedSprite2D")->get_frame() == 4)
                hasLaunched = false;
        }
        else
            animation = "glide";
    }
    else if (directionX != 0.f) {
        animation = "run";
    }

    get_node<AnimatedSprite2D>("AnimatedSprite2D")->set_animation(animation);
    get_node<AnimatedSprite2D>("AnimatedSprite2D")->set_flip_h(!facingRight);
}

void Peggy::squish(SquishAction action) {
    switch(action) {
        case JUMP:
            get_node<AnimatedSprite2D>("AnimatedSprite2D")->set_scale({ 0.75f, 1.25f });
        break;
        case LAND:
            get_node<AnimatedSprite2D>("AnimatedSprite2D")->set_scale({ 1.25f, 0.75f });
        break;
        case CHANGE_DIRECTION:
            get_node<AnimatedSprite2D>("AnimatedSprite2D")->set_scale({ 0.75f, 1.25f });
        break;
        default:
        break;
    }
}

void Peggy::playSfxFootstep() {
    if (get_node<AnimatedSprite2D>("AnimatedSprite2D")->get_animation().get_basename() == "run" && get_node<AnimatedSprite2D>("AnimatedSprite2D")->get_frame() == 0) {
        get_node<AudioStreamPlayer2D>("Sfx/SfxFootstep")->set_pitch_scale(2.0);
        get_node<AudioStreamPlayer2D>("Sfx/SfxFootstep")->set_pitch_scale(get_node<AudioStreamPlayer2D>("Sfx/SfxFootstep")->get_pitch_scale() * distr(gen));
        get_node<AudioStreamPlayer2D>("Sfx/SfxFootstep")->play();
    }
}

void Peggy::setSpeed(const float _speed) {
    speed = _speed;
}

float Peggy::getSpeed() const {
    return speed;
}

void Peggy::setJumpForce(const float _jumpForce) {
    jumpForce = _jumpForce;
}

float Peggy::getJumpForce() const {
    return jumpForce;
}

void Peggy::setGravity(const float _gravity) {
    gravity = _gravity;
}

float Peggy::getGravity() const {
    return gravity;
}