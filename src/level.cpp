#include "../inc/level.h"

using namespace godot;

void Level::_bind_methods() {
}

void Level::_ready() {
    if (Engine::get_singleton()->is_editor_hint())
        set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
    else 
        set_process_mode(Node::ProcessMode::PROCESS_MODE_INHERIT);
}