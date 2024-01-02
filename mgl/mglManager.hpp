////////////////////////////////////////////////////////////////////////////////
//
// Application Class
//
// Copyright (c)2022 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_MANAGER_HIGH_HPP
#define MGL_MANAGER_HIGH_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <map>
#include <string>

#include "mglMesh.hpp"
#include "mglShader.hpp"
#include "mglTexture.hpp"
#include "mglCallBack.hpp"
#include "mglSillouette.hpp"

namespace mgl {

/*
template <typename T>
class Manager;

//using ShaderManager = Manager<ShaderProgram*>;
//using TextureManager = Manager<Texture>;

///////////////////////////////////////////////////////////////////////// Engine

template <typename T>
class Manager {
private:
    std::map<std::string, T> data;
    static Manager instance;

    Manager(void);

public:
    static Manager& getInstance(void);

    //~Manager(void);

    //Manager<T>(Manager<T> const&) = delete;

    void add(std::string key, T value);

    T get(std::string key);
};

//template <typename T>
//Manager<T> Manager<T>::instance;
*/

    //class ShaderManager;

    template <typename T>
    class Manager;

    ///////////////////////////////////////////////////////////////////////// Engine

    /*
    class ShaderManager {
    public:
        static ShaderManager& getInstance();

        void add(std::string key, ShaderProgram* value);

        ShaderProgram* get(std::string key);

    protected:
        virtual ~ShaderManager();

    private:
        std::map<std::string, mgl::ShaderProgram*> shaders;
        //static ShaderManager instance;

        ShaderManager();

    public:
        ShaderManager(ShaderManager const&) = delete;
        void operator=(ShaderManager const&) = delete;
    };
    */

    ///////////////////////////////////////////////////////////////////////// Engine

    template <typename T>
    class Manager {
    public:
        static Manager<T>& getInstance();

        void add(std::string key, T value);

        T get(std::string key);

        virtual void DestroyObjects();

    protected:
        virtual ~Manager();

    private:
        std::map<std::string, T> objects;
        //static ShaderManager instance;

        Manager();

    public:
        Manager(Manager const&) = delete;
        void operator=(Manager const&) = delete;
    };

    //class ShaderManager : public Manager<ShaderProgram*> {};

    using ShaderManager = Manager<ShaderProgram*>;
    using TextureInfoManager = Manager<TextureInfo*>;
    using MeshManager = Manager<Mesh*>;
    using CallbackManager = Manager<CallBack*>;
    using SillouetteInfoManager = Manager<SillouetteInfo*>;

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_MANAGER_HPP */
