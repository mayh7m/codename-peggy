#include "../inc/peggy.h"

using namespace godot;

void Peggy::_bind_methods() {

    // signals
    ClassDB::bind_method(D_METHOD("on_animated_sprite_2d_frame_changed"), &Peggy::on_animated_sprite_2d_frame_changed);

    // properties
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

    set_velocity({ 0, 0 }); // to ensure starting velocity is zeroed

    gen = std::mt19937(rd());   // seed the generator
    distr = std::uniform_real_distribution<>(0.75, 1.25);   // define the range
}

void Peggy::_physics_process(double delta) {
    getInput();
    getFacingDirection();

    applyGravity();
    set_velocity({ directionX * speed, get_velocity()[1] });

    getAnimation();

    move_and_slide();   // execute the movement

    // landing squish
    if (airbourne && is_on_floor()) {
        squish(SquishAction::LAND);
        playSound("Sfx/SfxFall", 2.0);
        airbourne = false;
    }
}

void Peggy::_process(double delta) {
    unsquish(delta);
}

void Peggy::getInput() {
    directionX = Input::get_singleton()->get_axis("left", "right");

    // executes when the direction (< 0.0, 0.0, > 0.0) has been changed after the input
    if(directionX != oldDirectionX) {
        oldDirectionX = directionX;
        changeDirectionBodyResponse(); 
    }

    if (Input::get_singleton()->is_action_just_pressed("jump") && is_on_floor())
        jump();
}

void Peggy::applyGravity() {
    set_velocity({ get_velocity()[0], get_velocity()[1] += gravity } );
}

void Peggy::getFacingDirection() {
    if (directionX != 0)
        facingRight = directionX > 0;
}

void Peggy::getAnimation() {

    animation = "idle"; // default animation

    // determine if the character is jumping or has finished and is gliding now
    if (!is_on_floor()) {
        if (hasLaunched) {
            animation = "jump";
            if(get_node<AnimatedSprite2D>("AnimatedSprite2D")->get_frame() == 4)
                hasLaunched = false;
        }
        else
            animation = "glide";
    }
    // else if the chacter is on the ground and is moving, select running animation
    else if (directionX != 0.f) {
        animation = "run";
    }

    get_node<AnimatedSprite2D>("AnimatedSprite2D")->set_flip_h(!facingRight);   // flip sprites adequate to facing direction
    get_node<AnimatedSprite2D>("AnimatedSprite2D")->set_animation(animation);
}

void Peggy::jump() {
    set_velocity({ get_velocity()[0], -jumpForce });

    squish(SquishAction::JUMP);
    playSound("Sfx/SfxJump", 1.5);

    hasLaunched = true;
    airbourne = true;
}

void Peggy::squish(SquishAction action) {
    switch(action) {
        case SquishAction::JUMP:
            get_node<AnimatedSprite2D>("AnimatedSprite2D")->set_scale({ 0.67f, 1.33f });
        break;
        case SquishAction::LAND:
            get_node<AnimatedSprite2D>("AnimatedSprite2D")->set_scale({ 1.33f, 0.67f });
        break;
        case SquishAction::CHANGE_DIRECTION:
            get_node<AnimatedSprite2D>("AnimatedSprite2D")->set_scale({ 0.67f, 1.33f });
        break;
        default:
        break;
    }
}

void Peggy::unsquish(const double delta) {
    get_node<AnimatedSprite2D>("AnimatedSprite2D")->set_scale(
        { static_cast<real_t>(Math::move_toward(static_cast<double>(get_node<AnimatedSprite2D>("AnimatedSprite2D")->get_scale().x), 1.0, 1.5 * delta)), 
          static_cast<real_t>(Math::move_toward(static_cast<double>(get_node<AnimatedSprite2D>("AnimatedSprite2D")->get_scale().y), 1.0, 1.5 * delta)) }
    ); // fix the size a little more to the default sprite scale
}

void Peggy::changeDirectionBodyResponse() {
    if(directionX < 0.f && facingRight || directionX > 0.f && !facingRight) {
        squish(SquishAction::CHANGE_DIRECTION);
        playSound("Sfx/SfxChangeDirection", 2.5);
    }

    if(is_on_floor())
        playSound("Sfx/SfxFootstep", 2.0);
}

void Peggy::playSound(const NodePath& sound, double defaultPitchValue) {
    get_node<AudioStreamPlayer2D>(sound)->set_pitch_scale(defaultPitchValue);
    get_node<AudioStreamPlayer2D>(sound)->set_pitch_scale(get_node<AudioStreamPlayer2D>(sound)->get_pitch_scale() * distr(gen));
    get_node<AudioStreamPlayer2D>(sound)->play();
}

void Peggy::on_animated_sprite_2d_frame_changed() {
    // execute when the character is running and is making a step (adequate frame)
    if (get_node<AnimatedSprite2D>("AnimatedSprite2D")->get_animation().get_basename() == "run" && get_node<AnimatedSprite2D>("AnimatedSprite2D")->get_frame() == 2)
        playSound("Sfx/SfxFootstep", 2.0);
}