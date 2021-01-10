#pragma once
#include <deque>

#include "../Utils.h"
#include "../Render/Filter.h"
#include "../Render/Mesh.h"
#include "../Render/Object.h"
#include "../Render/Texture.h"

namespace engine
{
    class MeshController
    {
    public:
        using Type = render::Mesh;
        using Loader = render::MeshLoader;
        using Collection = std::deque<Type>;
    private:
        Ptr<Collection>      items_;
        Collection::iterator iter_;
    public:
        explicit MeshController(Ptr<Collection> items);

        void reset();
        void set(Ptr<Type const> mesh);

        Ptr<Type> next();
        Ptr<Type> prev();
        Ptr<Type> load(Loader&& loader);

        Ptr<Type const> get() const;
    };

    class TextureController
    {
    public:
        using Type = render::Texture;
        using Loader = render::TextureLoader;
        using Collection = std::deque<Type>;
    private:
        Ptr<Collection>      items_;
        Collection::iterator iter_;
    public:
        explicit TextureController(Ptr<Collection> items);

        void reset();
        void set(Ptr<Type const> texture);

        Ptr<Type> next();
        Ptr<Type> prev();
        Ptr<Type> load(Loader&& loader);

        Ptr<Type const> get() const;
    };

    class PipelineController
    {
    public:
        using Type = render::Pipeline;
        using Collection = std::deque<Type>;
    private:
        Ptr<Collection>      items_;
        Collection::iterator iter_;
    public:
        explicit PipelineController(Ptr<Collection> items);

        void reset();
        void set(Ptr<Type const> pipeline);

        Ptr<Type> next();
        Ptr<Type> prev();

        Ptr<Type const> get() const;
    };

    class FilterController
    {
    public:
        using Type = render::Filter;
        using Collection = std::deque<Type>;
    private:
        Ptr<Collection>      items_;
        Collection::iterator iter_;
    public:
        explicit FilterController(Ptr<Collection> items);

        void reset();

        Ptr<Type> next();
        Ptr<Type> prev();

        Ptr<Type const> get() const;
        Ptr<Type>       get();
    };

    class ObjectController
    {
    public:
        using Type = render::Object;
        using Collection = std::list<Type>;
    private:
        Ptr<Type>            obj_;
        Collection::iterator iter_;
    public:
        explicit ObjectController(Ptr<Type> root);

        void                reset();
        void                recurse();
        Ptr<render::Object> parent();

        Ptr<Type> next();
        Ptr<Type> prev();

        Ptr<Type> create();
        void      remove();

        Ptr<Type const> get() const;
        Ptr<Type>       get();
    };
}
