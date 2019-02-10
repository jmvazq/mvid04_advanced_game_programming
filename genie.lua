local project_list = {
    "AG_01",
    "AG_02",
    "EJ_02_01",
    "EJ_02_02",
    "EJ_02_03",
    "EJ_02_04",
    "EJ_02_05",
    "EJ_02_06",
    "AG_03",
    "EJ_03_01",
    "EJ_03_02",
    "EJ_03_03",
    "EJ_03_04",
    "AG_04",
    "AG_05_01",
    "AG_05_02",
    "AG_06",
    "AG_07_01",
    "AG_07_02",
    "AG_08_01",
    "AG_08_02",
    "AG_08_03",
    "AG_08_04",
    "AG_08_05",
    "AG_09",
    "AG_10_01",
    "AG_10_02",
    "AG_10_03"
}

local function new_project(name)
    project (name)
        kind ("ConsoleApp")
        includedirs {"include", "src/deps/glfw", "src/deps/glad", "src/deps/stb"}
        files {
            "tests/"..name.."/*.cpp",
            "tests/"..name.."?*.h",
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
        libdirs {"libs/Debug"}
    configuration {"release"}
        flags {"Optimize"}
        targetsuffix ("_r")
        libdirs {"libs/Release"}
    configuration {"windows"}
        files {
            "src/deps/glfw/egl_context.c",
            "src/deps/glfw/win32*",
            "src/deps/glfw/wgl_*",
            "src/deps/glfw/winmm_*"
        }
        links {"OpenGL32", "assimp-vc140-mt"}
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
