#include "Controller.h"

namespace
{
    using namespace std::filesystem;

    template <class Container, class Iter>
    OptPtr<typename Iter::value_type> rerefImpl(Container& cs, Iter& iter)
    {
        return std::end(cs) == iter ? nullptr : &*iter;
    }

    template <class Container, class Iter>
    OptPtr<typename Iter::value_type> nextImpl(Container& cs, Iter& iter)
    {
        if (iter == std::end(cs)) { iter = std::begin(cs); }
        else { ++iter; }
        return rerefImpl(cs, iter);
    }

    template <class Container, class Iter>
    OptPtr<typename Iter::value_type> prevImpl(Container& cs, Iter& iter)
    {
        if (iter == std::begin(cs)) { iter = std::end(cs); }
        else { --iter; }
        return rerefImpl(cs, iter);
    }

    template <class Container, class Type>
    typename Container::iterator findImpl(Container& cs, Ptr<Type> ptr)
    {
        return std::find_if(std::begin(cs), std::end(cs), [&](Type const& it) { return &it == ptr; });
    }

    template <class Iter, class Container, class Loader>
    OptPtr<typename Iter::value_type> loadImpl(Container& cs, Iter& iter, Loader&& loader)
    {
        cs.emplace_back(std::move(loader));
        iter = --std::end(cs);
        return rerefImpl(cs, iter);
    }

    std::vector<path>::iterator scanImpl(std::vector<path>& files, path const& dir, std::wstring_view const extension)
    {
        files.clear();
        for (auto& entry : recursive_directory_iterator(dir, directory_options::skip_permission_denied))
            if (auto& p = entry.path(); entry.is_regular_file() && p.extension() == extension)
                files.push_back(p);
        return files.end();
    }
}

namespace engine
{
    MeshController::MeshController(Ptr<Collection> const items)
        : items_ {items},
          iter_ {items->end()}
    {}

    TextureController::TextureController(Ptr<Collection> const items)
        : items_ {items},
          iter_ {items->end()}
    {}

    PipelineController::PipelineController(Ptr<Collection> const items)
        : items_ {items},
          iter_ {items->end()}
    {}

    FilterController::FilterController(Ptr<Collection> const items)
        : items_ {items},
          iter_ {items->end()}
    {}

    ObjectController::ObjectController(Ptr<Type> const root)
        : obj_ {root},
          iter_ {root->children.end()}
    {}

    FileController::FileController(config::Paths const& paths)
        : iter_ {files_.end()},
          meshes_ {paths.meshes},
          textures_ {paths.textures}
    {}


    void MeshController::reset() { iter_ = items_->end(); }
    void TextureController::reset() { iter_ = items_->end(); }
    void PipelineController::reset() { iter_ = items_->end(); }
    void FilterController::reset() { iter_ = items_->end(); }
    void ObjectController::reset() { iter_ = obj_->children.end(); }

    void FileController::reset()
    {
        iter_    = files_.erase(files_.begin(), files_.end());
        current_ = std::nullopt;
    }

    void MeshController::set(Ptr<Type const> const mesh) { iter_ = findImpl(*items_, mesh); }
    void PipelineController::set(Ptr<Type const> const pipeline) { iter_ = findImpl(*items_, pipeline); }
    void TextureController::set(Ptr<Type const> const texture) { iter_ = findImpl(*items_, texture); }

    auto FileController::set(AssetType const assets) -> void
    {
        switch (assets)
        {
        case AssetType::Mesh:
            iter_ = scanImpl(files_, meshes_, L".obj");
            break;
        case AssetType::Texture:
            iter_ = scanImpl(files_, textures_, L".png");
            break;
        default:
            throw std::invalid_argument("Invalid AssetType enum variant.");
        }
        current_ = assets;
    }


    void ObjectController::recurse()
    {
        if (std::end(obj_->children) != iter_)
        {
            obj_  = &*iter_;
            iter_ = obj_->children.end();
        }
    }

    Ptr<ObjectController::Type> ObjectController::parent()
    {
        if (obj_->parent)
        {
            iter_ = findImpl(obj_->parent->children, obj_);
            obj_  = obj_->parent;
        }
        return rerefImpl(obj_->children, iter_);
    }

    OptPtr<MeshController::Type>    MeshController::next() { return nextImpl(*items_, iter_); }
    OptPtr<TextureController::Type> TextureController::next() { return nextImpl(*items_, iter_); }
    OptPtr<FilterController::Type>  FilterController::next() { return nextImpl(*items_, iter_); }
    OptPtr<ObjectController::Type>  ObjectController::next() { return nextImpl(obj_->children, iter_); }

    OptPtr<PipelineController::Type> PipelineController::next()
    {
        OptPtr<Type> out;

        do { out = nextImpl(*items_, iter_); }
        while (out != nullptr && out->isFilter());

        return out;
    }

    OptPtr<FileController::Type> FileController::next() { return nextImpl(files_, iter_); }


    OptPtr<MeshController::Type>    MeshController::prev() { return prevImpl(*items_, iter_); }
    OptPtr<TextureController::Type> TextureController::prev() { return prevImpl(*items_, iter_); }
    OptPtr<FilterController::Type>  FilterController::prev() { return prevImpl(*items_, iter_); }
    OptPtr<ObjectController::Type>  ObjectController::prev() { return prevImpl(obj_->children, iter_); }

    OptPtr<PipelineController::Type> PipelineController::prev()
    {
        OptPtr<Type> out;

        do { out = prevImpl(*items_, iter_); }
        while (out != nullptr && out->isFilter());

        return out;
    }

    OptPtr<FileController::Type> FileController::prev() { return prevImpl(files_, iter_); }


    OptPtr<MeshController::Type> MeshController::load(Loader&& loader)
    {
        return loadImpl(*items_, iter_, std::move(loader));
    }

    OptPtr<TextureController::Type> TextureController::load(Loader&& loader)
    {
        return loadImpl(*items_, iter_, std::move(loader));
    }

    OptPtr<ObjectController::Type> ObjectController::create()
    {
        obj_->emplaceChild();
        iter_ = --obj_->children.end();
        return &*iter_;
    }

    void ObjectController::remove()
    {
        obj_->children.erase(iter_);
        iter_ = obj_->children.end();
    }

    OptPtr<MeshController::Type const>     MeshController::get() const { return rerefImpl(*items_, iter_); }
    OptPtr<TextureController::Type const>  TextureController::get() const { return rerefImpl(*items_, iter_); }
    OptPtr<PipelineController::Type const> PipelineController::get() const { return rerefImpl(*items_, iter_); }

    OptPtr<FilterController::Type const> FilterController::get() const { return rerefImpl(*items_, iter_); }
    OptPtr<FilterController::Type>       FilterController::get() { return rerefImpl(*items_, iter_); }

    OptPtr<ObjectController::Type const> ObjectController::get() const { return rerefImpl(obj_->children, iter_); }
    OptPtr<ObjectController::Type>       ObjectController::get() { return rerefImpl(obj_->children, iter_); }

    OptPtr<FileController::Type const> FileController::get() const { return rerefImpl(files_, iter_); }
    OptPtr<FileController::Type>       FileController::get() { return rerefImpl(files_, iter_); }

    bool FileController::loadingMeshes() const { return current_.has_value() && current_ == AssetType::Mesh; }
    bool FileController::loadingTextures() const { return current_.has_value() && current_ == AssetType::Texture; }
}
