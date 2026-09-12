cmake_minimum_required(VERSION 3.12)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(join_server  VERSION ${PROJECT_VERSION})

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)



add_executable(${PROJECT_NAME} main.cpp
        database.cpp
        database.h
        parser.cpp
        parser.h
        session.h
        session.cpp
        server.h
        server.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system Threads::Threads)

set_target_properties(${PROJECT_NAME}  PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

if(MSVC)
    target_compile_options(${PROJECT_NAME}  PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME}  PRIVATE
            -Wall -Wextra -pedantic -Werror
    )
endif()


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT evgeniy.retc@gmail.com)
include(CPack)

