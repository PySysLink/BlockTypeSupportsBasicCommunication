# Define the shared library
add_library(BlockTypeSupportsBasicCommunicationSupport SHARED 
            RegisterBlockFactories.cpp
            LoggerInstance.cpp
            OPCUAServerChannel.cpp)



include(FetchContent)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.15.0
)

FetchContent_MakeAvailable(spdlog)

add_dependencies(BlockTypeSupportsBasicCommunicationSupport spdlog::spdlog)
target_link_libraries(BlockTypeSupportsBasicCommunicationSupport PRIVATE spdlog::spdlog)


set(YAML_CPP_STATIC_LIBS TRUE)
find_package(yaml-cpp REQUIRED)


find_package(PkgConfig)
pkg_check_modules(open62541 REQUIRED open62541)

set_target_properties(BlockTypeSupportsBasicCommunicationSupport PROPERTIES
    BUILD_RPATH "/usr/local/lib64"
    INSTALL_RPATH "/usr/local/lib64"
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

target_link_libraries(BlockTypeSupportsBasicCommunicationSupport PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(BlockTypeSupportsBasicCommunicationSupport PRIVATE /usr/local/lib64/libopen62541.a)
target_link_libraries(BlockTypeSupportsBasicCommunicationSupport PRIVATE PySysLinkBase::PySysLinkBase)
target_link_libraries(BlockTypeSupportsBasicCommunicationSupport PRIVATE yaml-cpp)

# Include directories for header files
target_include_directories(BlockTypeSupportsBasicCommunicationSupport PUBLIC ${PySysLinkBase_INCLUDE_DIRS})


set_target_properties(BlockTypeSupportsBasicCommunicationSupport PROPERTIES 
    POSITION_INDEPENDENT_CODE ON
    VERSION ${PROJECT_VERSION}
    IMPORTED_LOCATION "${CMAKE_INSTALL_PREFIX}${INSTALLED_DYNAMIC_LIB_NAME}.so"
    OUTPUT_NAME "${INSTALLED_DYNAMIC_LIB_NAME}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_include_directories(BlockTypeSupportsBasicCommunicationSupport
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  # Include path for the build tree
        $<INSTALL_INTERFACE:include>                   # Include path for the installed tree
)
