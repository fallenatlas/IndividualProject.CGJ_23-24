#include "./mglManager.hpp"

#include <iostream>

#include "./mglError.hpp"

namespace mgl {

    template <typename T>
    Manager<T>::Manager(void) {}

    template <typename T>
    Manager<T>::~Manager(void) {}

    template <typename T>
    void Manager<T>::DestroyObjects(void) {
        for (auto object : objects) {
            delete object.second;
        }
    }

    template <typename T>
    Manager<T>& Manager<T>::getInstance() {
        static Manager<T> instance;
        return instance;
    }

    template <typename T>
    void Manager<T>::add(std::string key, T value) {
        objects[key] = value;
    }

    template <typename T>
    T Manager<T>::get(std::string key) {
        auto pos = objects.find(key);
        if (pos == objects.end()) {
            return nullptr;
        }
        else {
            return pos->second;

        }
    }

    template class Manager<ShaderProgram*>;
    template class Manager<TextureInfo*>;
    template class Manager<Mesh*>;
    template class Manager<CallBack*>;
    template class Manager<SillouetteInfo*>;

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl
