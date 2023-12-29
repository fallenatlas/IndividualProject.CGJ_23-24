////////////////////////////////////////////////////////////////////////////////
//
// Application Class
//
// Copyright (c)2022-23 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "./mglManager.hpp"

#include <iostream>

#include "./mglError.hpp"

namespace mgl {

////////////////////////////////////////////////////////////////////////// SETUP
/*
template <typename T>
Manager<T>::Manager(void) {}

template <typename T>
Manager<T>& Manager<T>::getInstance(void) {
    return instance;
}

//template <typename T>
//Manager<T>::~Manager(void) {
    //for (auto it = data.begin(); it != data.end(); ++it)
    //    delete it->second;
    // for everything call delete
//}

template <typename T>
void Manager<T>::add(std::string key, T value) {
    // check if this adds
    //data.insert({ key, value });
    data[key] = value;
}

template <typename T>
T Manager<T>::get(std::string key) {
    auto pos = data.find(key);
    if (pos == data.end()) {
        return nullptr;
    }
    else {
        return pos->second;

    }
}
*/
    /*
    ShaderManager::ShaderManager(void) {}

    ShaderManager::~ShaderManager(void) {}

    ShaderManager& ShaderManager::getInstance(void) {
        //static ShaderManager instance;
        static ShaderManager instance;
        return instance;
    }

    void ShaderManager::add(std::string key, ShaderProgram* value) {
        // check if this adds
        //data.insert({ key, value });
        shaders[key] = value;
    }

    ShaderProgram* ShaderManager::get(std::string key) {
        auto pos = shaders.find(key);
        if (pos == shaders.end()) {
            return nullptr;
        }
        else {
            return pos->second;

        }
    }
    */

    template <typename T>
    Manager<T>::Manager(void) {}

    template <typename T>
    Manager<T>::~Manager(void) {}

    /*
    template <typename T>
    Manager<T>& Manager<T>::getInstance(void) {
        //static ShaderManager instance;
        static Manager<T> instance;
        return instance;
    }
    */

    template <typename T>
    Manager<T>& Manager<T>::getInstance() {
        static Manager<T> instance;
        return instance;
    }


    template <typename T>
    void Manager<T>::add(std::string key, T value) {
        // check if this adds
        //data.insert({ key, value });
        shaders[key] = value;
    }

    template <typename T>
    T Manager<T>::get(std::string key) {
        auto pos = shaders.find(key);
        if (pos == shaders.end()) {
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
