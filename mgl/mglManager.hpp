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

        Manager();

    public:
        Manager(Manager const&) = delete;
        void operator=(Manager const&) = delete;
    };

    using ShaderManager = Manager<ShaderProgram*>;
    using TextureInfoManager = Manager<TextureInfo*>;
    using MeshManager = Manager<Mesh*>;
    using CallbackManager = Manager<CallBack*>;
    using SillouetteInfoManager = Manager<SillouetteInfo*>;

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_MANAGER_HPP */
