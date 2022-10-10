cmake_minimum_required(VERSION "3.24.0")

project("GoldenEmu8")

set(CMAKE_CXX_STANDARD 17)

set(SDL2_INCLUDE_DIRS "GoldenEmu/middleware/sdl/include")

# Support both 32 and 64 bit builds
if(${CMAKE_SIZEOF_VOID_P} MATCHES 8)
    set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/GoldenEmu/middleware/sdl/lib/x64/SDL2.lib;${CMAKE_CURRENT_SOURCE_DIR}/GoldenEmu/middleware/sdl/lib/x64/SDL2main.lib;${CMAKE_CURRENT_SOURCE_DIR}/GoldenEmu/middleware/sdl/lib/x64/SDL2_mixer.lib")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/GoldenEmu/middleware/sdl/lib/x64/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR}/SDL2.dll COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/GoldenEmu/middleware/sdl/lib/x64/SDL2_mixer.dll ${CMAKE_CURRENT_BINARY_DIR}/SDL2_mixer.dll COPYONLY)

else()
    set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/GoldenEmu/middleware/sdl/lib/x86/SDL2.lib;${CMAKE_CURRENT_SOURCE_DIR}/GoldenEmu/middleware/sdl/lib/x86/SDL2main.lib;${CMAKE_CURRENT_SOURCE_DIR}/GoldenEmu/middleware/sdl/lib/x86/SDL2_mixer.lib")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/GoldenEmu/middleware/sdl/lib/x86/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR}/SDL2.dll COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/GoldenEmu/middleware/sdl/lib/x86/SDL2_mixer.dll ${CMAKE_CURRENT_BINARY_DIR}/SDL2_mixer.dll COPYONLY)
endif()

string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

add_library("YAML"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/anchor.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/binary.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/depthguard.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/dll.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/emitfromevents.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/emitter.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/emitterdef.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/emittermanip.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/emitterstyle.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/eventhandler.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/exceptions.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/mark.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/noexcept.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/null.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/ostream_wrapper.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/parser.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/stlemitter.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/traits.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/yaml.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/contrib/anchordict.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/contrib/graphbuilder.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/convert.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/emit.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/impl.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/iterator.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/node.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/parse.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/ptr.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/type.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/detail/impl.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/detail/iterator.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/detail/iterator_fwd.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/detail/memory.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/detail/node.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/detail/node_data.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/detail/node_iterator.h"
    "GoldenEmu/middleware/yaml/include/yaml-cpp/node/detail/node_ref.h"
    "GoldenEmu/middleware/yaml/src/collectionstack.h"
    "GoldenEmu/middleware/yaml/src/directives.h"
    "GoldenEmu/middleware/yaml/src/emitterstate.h"
    "GoldenEmu/middleware/yaml/src/emitterutils.h"
    "GoldenEmu/middleware/yaml/src/exp.h"
    "GoldenEmu/middleware/yaml/src/indentation.h"
    "GoldenEmu/middleware/yaml/src/nodebuilder.h"
    "GoldenEmu/middleware/yaml/src/nodeevents.h"
    "GoldenEmu/middleware/yaml/src/ptr_vector.h"
    "GoldenEmu/middleware/yaml/src/regeximpl.h"
    "GoldenEmu/middleware/yaml/src/regex_yaml.h"
    "GoldenEmu/middleware/yaml/src/scanner.h"
    "GoldenEmu/middleware/yaml/src/scanscalar.h"
    "GoldenEmu/middleware/yaml/src/scantag.h"
    "GoldenEmu/middleware/yaml/src/setting.h"
    "GoldenEmu/middleware/yaml/src/singledocparser.h"
    "GoldenEmu/middleware/yaml/src/stream.h"
    "GoldenEmu/middleware/yaml/src/streamcharsource.h"
    "GoldenEmu/middleware/yaml/src/stringsource.h"
    "GoldenEmu/middleware/yaml/src/tag.h"
    "GoldenEmu/middleware/yaml/src/token.h"
    "GoldenEmu/middleware/yaml/src/contrib/graphbuilderadapter.h"

    "GoldenEmu/middleware/yaml/src/binary.cpp"
    "GoldenEmu/middleware/yaml/src/convert.cpp"
    "GoldenEmu/middleware/yaml/src/depthguard.cpp"
    "GoldenEmu/middleware/yaml/src/directives.cpp"
    "GoldenEmu/middleware/yaml/src/emit.cpp"
    "GoldenEmu/middleware/yaml/src/emitfromevents.cpp"
    "GoldenEmu/middleware/yaml/src/emitter.cpp"
    "GoldenEmu/middleware/yaml/src/emitterstate.cpp"
    "GoldenEmu/middleware/yaml/src/emitterutils.cpp"
    "GoldenEmu/middleware/yaml/src/exceptions.cpp"
    "GoldenEmu/middleware/yaml/src/exp.cpp"
    "GoldenEmu/middleware/yaml/src/memory.cpp"
    "GoldenEmu/middleware/yaml/src/node.cpp"
    "GoldenEmu/middleware/yaml/src/nodebuilder.cpp"
    "GoldenEmu/middleware/yaml/src/nodeevents.cpp"
    "GoldenEmu/middleware/yaml/src/node_data.cpp"
    "GoldenEmu/middleware/yaml/src/null.cpp"
    "GoldenEmu/middleware/yaml/src/ostream_wrapper.cpp"
    "GoldenEmu/middleware/yaml/src/parse.cpp"
    "GoldenEmu/middleware/yaml/src/parser.cpp"
    "GoldenEmu/middleware/yaml/src/regex_yaml.cpp"
    "GoldenEmu/middleware/yaml/src/scanner.cpp"
    "GoldenEmu/middleware/yaml/src/scanscalar.cpp"
    "GoldenEmu/middleware/yaml/src/scantag.cpp"
    "GoldenEmu/middleware/yaml/src/scantoken.cpp"
    "GoldenEmu/middleware/yaml/src/simplekey.cpp"
    "GoldenEmu/middleware/yaml/src/singledocparser.cpp"
    "GoldenEmu/middleware/yaml/src/stream.cpp"
    "GoldenEmu/middleware/yaml/src/tag.cpp"
    "GoldenEmu/middleware/yaml/src/contrib/graphbuilder.cpp"
    "GoldenEmu/middleware/yaml/src/contrib/graphbuilderadapter.cpp"
    "GoldenEmu/middleware/yaml/util/parse.cpp"
    "GoldenEmu/middleware/yaml/util/read.cpp"
    "GoldenEmu/middleware/yaml/util/sandbox.cpp"
)

add_library("ImGui"
    "GoldenEmu/middleware/imgui/imgui.cpp"
    "GoldenEmu/middleware/imgui/imgui_demo.cpp"
    "GoldenEmu/middleware/imgui/imgui_draw.cpp"
    "GoldenEmu/middleware/imgui/imgui_tables.cpp"
    "GoldenEmu/middleware/imgui/imgui_widgets.cpp"
    "GoldenEmu/middleware/imgui/backends/imgui_impl_sdl.cpp"
    "GoldenEmu/middleware/imgui/backends/imgui_impl_sdlrenderer.cpp"

    "GoldenEmu/middleware/imgui/imconfig.h"
    "GoldenEmu/middleware/imgui/imgui.h"
    "GoldenEmu/middleware/imgui/imgui_internal.h"
    "GoldenEmu/middleware/imgui/imstb_rectpack.h"
    "GoldenEmu/middleware/imgui/imstb_textedit.h"
    "GoldenEmu/middleware/imgui/imstb_truetype.h"
    "GoldenEmu/middleware/imgui/backends/imgui_impl_sdl.h"
    "GoldenEmu/middleware/imgui/backends/imgui_impl_sdlrenderer.h"
)

add_executable("${PROJECT_NAME}"
    "GoldenEmu/src/application.h"
    "GoldenEmu/src/chip8.h"
    "GoldenEmu/src/input.h"
    "GoldenEmu/src/settings.h"
    "GoldenEmu/src/ui.h"
    "GoldenEmu/src/util.h"

    "GoldenEmu/src/application.cpp"
    "GoldenEmu/src/chip8.cpp"
    "GoldenEmu/src/input.cpp"
    "GoldenEmu/src/main.cpp"
    "GoldenEmu/src/settings.cpp"
    "GoldenEmu/src/ui.cpp"
    "GoldenEmu/src/util.cpp"
)

target_link_libraries("${PROJECT_NAME}" PRIVATE "YAML")
target_link_libraries("${PROJECT_NAME}" PRIVATE ${SDL2_LIBRARIES})
target_link_libraries("${PROJECT_NAME}" PRIVATE "ImGui")

target_include_directories("YAML" PRIVATE "GoldenEmu/middleware/yaml/include/")

target_include_directories("ImGui" PRIVATE "GoldenEmu/middleware/imgui/")
target_include_directories("ImGui" PRIVATE "GoldenEmu/middleware/sdl/include")

target_include_directories("${PROJECT_NAME}" PRIVATE "GoldenEmu/middleware/yaml/include/")
target_include_directories("${PROJECT_NAME}" PRIVATE "GoldenEmu/middleware/sdl/include")
target_include_directories("${PROJECT_NAME}" PRIVATE "GoldenEmu/middleware/imgui")
