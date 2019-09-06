include(ExternalProject)

if(NOT DJV_THIRD_PARTY_DISABLE_BUILD)
    set(OPENEXR_RUNTIME_DIR lib)
    if(WIN32)
        set(OPENEXR_RUNTIME_DIR bin)
    endif()
    set(OpenEXR_ARGS
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_CXX_STANDARD=11
        -DOPENEXR_VERSION_MAJOR=2
        -DOPENEXR_VERSION_MINOR=3
        -DOPENEXR_VERSION_PATCH=0
        -DOPENEXR_VERSION=2.3.0
        -DOPENEXR_SOVERSION=24
        -DOPENEXR_BUILD_SHARED=${OPENEXR_SHARED_LIBS}
        -DILMBASE_PACKAGE_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DILMBASE_SHARED_LIBS=${ILMBASE_SHARED_LIBS}
        -DZLIB_SHARED_LIBS=${ZLIB_SHARED_LIBS}
        -DBUILD_SHARED_LIBS=${OPENEXR_SHARED_LIBS}
        -DRUNTIME_DIR=${OPENEXR_RUNTIME_DIR})
    if(NOT OPENEXR_SHARED_LIBS)
        set(OpenEXR_ARGS ${OpenEXR_ARGS} -DOPENEXR_BUILD_STATIC=1)
    endif()
    ExternalProject_Add(
        OpenEXRThirdParty
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenEXR
        DEPENDS IlmBaseThirdParty ZLIBThirdParty
        URL http://github.com/openexr/openexr/releases/download/v2.3.0/openexr-2.3.0.tar.gz
        PATCH_COMMAND
            ${CMAKE_COMMAND} -E tar xf
            ${CMAKE_SOURCE_DIR}/third-party/openexr-patch.tar.gz
        CMAKE_ARGS ${OpenEXR_ARGS})
endif()

set(OPENEXR_FOUND TRUE)
set(OPENEXR_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
set(OPENEXR_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}IlmImf_s${CMAKE_STATIC_LIBRARY_SUFFIX})
set(OPENEXR_LIBRARIES ${OPENEXR_LIBRARY} ${ILMBASE_LIBRARIES} ${ZLIB_LIBRARIES})

if(OPENEXR_FOUND AND NOT TARGET OpenEXR::IlmImf)
    add_library(OpenEXR::IlmImf UNKNOWN IMPORTED)
    add_dependencies(OpenEXR::IlmImf OpenEXRThirdParty)
    set_target_properties(OpenEXR::IlmImf PROPERTIES
        IMPORTED_LOCATION "${OPENEXR_LIBRARY}"
        INTERFACE_LINK_LIBRARIES "IlmBase;ZLIB"
        INTERFACE_INCLUDE_DIRECTORIES "${OPENEXR_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS OPENEXR_FOUND)
endif()
if(OPENEXR_FOUND AND NOT TARGET OpenEXR)
    add_library(OpenEXR INTERFACE)
    target_link_libraries(OpenEXR INTERFACE OpenEXR::IlmImf)
endif()

