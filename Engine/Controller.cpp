#include "Controller.h"

namespace
{
    template <class Container, class Iter>
    Ptr<typename Iter::value_type> rerefImpl(Container& cs, Iter& iter)
    {
        return std::end(cs) == iter ? nullptr : &*iter;
    }

    template <class Container, class Iter>
    Ptr<typename Iter::value_type> nextImpl(Container& cs, Iter& iter)
    {
        if (iter == std::end(cs)) { iter = std::begin(cs); }
        else { ++iter; }
        return rerefImpl(cs, iter);
    }

    template <class Container, class Iter>
    Ptr<typename Iter::value_type> prevImpl(Container& cs, Iter& iter)
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
    Ptr<typename Iter::value_type> loadImpl(Container& cs, Iter& iter, Loader&& loader)
    {
        cs.emplace_back(std::move(loader));
        iter = --std::end(cs);
        return rerefImpl(cs, iter);
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

    void MeshController::reset() { iter_ = items_->end(); }
    void TextureController::reset() { iter_ = items_->end(); }
    void PipelineController::reset() { iter_ = items_->end(); }
    void FilterController::reset() { iter_ = items_->end(); }
    void ObjectController::reset() { iter_ = obj_->children.end(); }

    void MeshController::set(Ptr<Type const> const mesh) { iter_ = findImpl(*items_, mesh); }
    void PipelineController::set(Ptr<Type const> const pipeline) { iter_ = findImpl(*items_, pipeline); }
    void TextureController::set(Ptr<Type const> const texture) { iter_ = findImpl(*items_, texture); }

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

    Ptr<MeshController::Type>    MeshController::next() { return nextImpl(*items_, iter_); }
    Ptr<TextureController::Type> TextureController::next() { return nextImpl(*items_, iter_); }
    Ptr<FilterController::Type>  FilterController::next() { return nextImpl(*items_, iter_); }
    Ptr<ObjectController::Type>  ObjectController::next() { return nextImpl(obj_->children, iter_); }

    Ptr<PipelineController::Type> PipelineController::next()
    {
        Ptr<Type> out;

        do { out = nextImpl(*items_, iter_); }
        while (out != nullptr && out->isFilter());

        return out;
    }


    Ptr<MeshController::Type>    MeshController::prev() { return prevImpl(*items_, iter_); }
    Ptr<TextureController::Type> TextureController::prev() { return prevImpl(*items_, iter_); }
    Ptr<FilterController::Type>  FilterController::prev() { return prevImpl(*items_, iter_); }
    Ptr<ObjectController::Type>  ObjectController::prev() { return prevImpl(obj_->children, iter_); }

    Ptr<PipelineController::Type> PipelineController::prev()
    {
        Ptr<Type> out;

        do { out = prevImpl(*items_, iter_); }
        while (out != nullptr && out->isFilter());

        return out;
    }


    Ptr<MeshController::Type> MeshController::load(Loader&& loader)
    {
        return loadImpl(*items_, iter_, std::move(loader));
    }

    Ptr<TextureController::Type> TextureController::load(Loader&& loader)
    {
        return loadImpl(*items_, iter_, std::move(loader));
    }

    Ptr<ObjectController::Type> ObjectController::create()
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

    Ptr<MeshController::Type const>     MeshController::get() const { return rerefImpl(*items_, iter_); }
    Ptr<TextureController::Type const>  TextureController::get() const { return rerefImpl(*items_, iter_); }
    Ptr<PipelineController::Type const> PipelineController::get() const { return rerefImpl(*items_, iter_); }

    Ptr<FilterController::Type const> FilterController::get() const { return rerefImpl(*items_, iter_); }
    Ptr<FilterController::Type>       FilterController::get() { return rerefImpl(*items_, iter_); }

    Ptr<ObjectController::Type const> ObjectController::get() const { return rerefImpl(obj_->children, iter_); }
    Ptr<ObjectController::Type>       ObjectController::get() { return rerefImpl(obj_->children, iter_); }
}
