set(header
    EnumFuncTest.h
    ImageConvertTest.h
    MeshCacheTest.h
    MeshTest.h
    MeshFuncTest.h
    OffscreenBufferFuncTest.h
    OffscreenBufferTest.h
    ShaderTest.h
    TextureAtlasTest.h
    TextureFuncTest.h
    TextureTest.h)
set(source
    EnumFuncTest.cpp
    ImageConvertTest.cpp
    MeshCacheTest.cpp
    MeshTest.cpp
    MeshFuncTest.cpp
    OffscreenBufferFuncTest.cpp
    OffscreenBufferTest.cpp
    ShaderTest.cpp
    TextureAtlasTest.cpp
    TextureFuncTest.cpp
    TextureTest.cpp)

add_library(djvGLTest ${header} ${source})
target_link_libraries(djvGLTest djvTestLib djvGL)
set_target_properties(
    djvGLTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)

