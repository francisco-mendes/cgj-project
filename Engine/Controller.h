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

        OptPtr<Type> next();
        OptPtr<Type> prev();
        OptPtr<Type> load(Loader&& loader);

        OptPtr<Type const> get() const;
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

        OptPtr<Type> next();
        OptPtr<Type> prev();
        OptPtr<Type> load(Loader&& loader);

        OptPtr<Type const> get() const;
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

        OptPtr<Type> next();
        OptPtr<Type> prev();

        OptPtr<Type const> get() const;
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

        OptPtr<Type> next();
        OptPtr<Type> prev();

        OptPtr<Type const> get() const;
        OptPtr<Type>       get();
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

        void                   reset();
        void                   recurse();
        OptPtr<render::Object> parent();

        OptPtr<Type> next();
        OptPtr<Type> prev();

        OptPtr<Type> create();
        void         remove();

        OptPtr<Type const> get() const;
        OptPtr<Type>       get();
    };

    class FileController
    {
    public:
        using Type = std::filesystem::path;
        using Collection = std::vector<Type>;

        enum class AssetType: bool
        {
            Mesh = false,
            Texture = true
        };

    private:
        Collection           files_;
        Collection::iterator iter_;

        Type meshes_, textures_;

    public:
        explicit FileController(config::Paths const& paths);

        void reset();
        void set(AssetType assets);

        OptPtr<Type> next();
        OptPtr<Type> prev();

        OptPtr<Type const> get() const;
        OptPtr<Type>       get();
    };
}
