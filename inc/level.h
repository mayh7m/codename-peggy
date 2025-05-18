#pragma once

#include <godot_cpp/classes/engine.hpp>

#include "peggy.h"

namespace godot {
    // level class
    class Level : public Node2D {
        GDCLASS(Level, Node2D);

    protected:
        // obligatory method used for binding methods in the godot editor
        static void _bind_methods();

    public:
        // methods overrided from the godot classes
        virtual void _ready() override;
    };
}