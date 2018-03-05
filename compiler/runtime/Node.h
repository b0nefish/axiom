#pragma once

#include <set>

#include "ModuleRuntimeUnit.h"

namespace MaximRuntime {

    class GeneratableModuleClass;

    class Surface;

    class Control;

    class Node : public ModuleRuntimeUnit {
    public:
        explicit Node(Surface *surface);

        virtual GeneratableModuleClass *compile() = 0;

        virtual void remove();

        Surface *surface() const { return _surface; }

        virtual const std::unique_ptr<Control> *begin() const = 0;

        virtual const std::unique_ptr<Control> *end() const = 0;

        void scheduleCompile();

        bool needsCompile() const { return _needsCompile; }

    private:

        Surface *_surface;

        bool _needsCompile = false;
    };

}