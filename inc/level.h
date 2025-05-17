#pragma once

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "Peggy.h"

namespace godot {
    class Level : public Node2D {
        GDCLASS(Level, Node2D);

    Vector2 offset;

    protected:
        static void _bind_methods();

    public:
        virtual void _ready() override;
    };
}