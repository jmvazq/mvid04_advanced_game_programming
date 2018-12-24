local project_list = {
    "AG_01"
}

local function new_project(name)
    project (name)
        kind ("ConsoleApp")
        includedirs {"include", "src/deps/glfw", "src/deps/glad", "src/deps/stb"}
        files {
            "tests/"..name.."/*.cpp",
            "tests/"..name.."?*.h",
            "tests/"..name.."/README.md",
            "tests/"..name.."/*.png",
            "tests/"..name.."/*.gif",
            "src/*.cpp",
            "include/**.h",
            "src/deps/glfw/context.c",
            "src/deps/glfw/init.c",
            "src/deps/glfw/input.c",
            "src/deps/glfw/monitor.c",
            "src/deps/glfw/window.c",
            "src/deps/glfw/vulkan.c",
            "src/deps/glad/glad.c",
            "src/deps/stb/*"
        }
    defines {"_GLFW_USE_OPENGL=1"}
    vpaths {
        ["Headers"] = "include/*.h",
        ["Source"] = {"src/*.cpp"},
        ["Deps Headers"] = {
            "include/GLFW/*", 
            "include/glad/**", 
            "include/KHR/**", 
            "include/glm/**"
        },
        ["Deps Srcs"] = "src/deps/**",
        ["Tests"] = {
            "tests/"..name.."/*.cpp",
            "tests/"..name.."/*.h"
        }
    }
    configuration {"debug"}
        flags {"Symbols"}
        targetsuffix ("_d")
    configuration {"release"}
        flags {"Optimize"}
        targetsuffix ("_r")
    configuration {"windows"}
        files {
            "src/deps/glfw/egl_context.c",
            "src/deps/glfw/win32*",
            "src/deps/glfw/wgl_*",
            "src/deps/glfw/winmm_*"
        }
        links {"OpenGL32"}
        defines {"_GLFW_WIN32", "_GLFW_WGL"}
        flags {"NoEditAndContinue"}
        windowstargetplatformversion "10.0.17134.0"
end

solution "04MVID_Class"
    configurations  {"debug", "release"}
    language        ("c++")
    platforms       {"x64"}
    location        ("build")
    debugdir        ("build")
    targetdir       ("bin")
    flags           {"ExtraWarnings"}

for k,v in pairs(project_list) do
    new_project(v)
end
