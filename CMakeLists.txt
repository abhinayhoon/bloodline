cmake_minimum_required(VERSION 3.14)
project(BloodLine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ─── Fetch header-only dependencies ───
include(FetchContent)

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.15.3
)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

FetchContent_MakeAvailable(httplib json)

# ─── Main server executable ───
add_executable(bloodline_server backend/src/main.cpp)

target_include_directories(bloodline_server PRIVATE
    ${CMAKE_SOURCE_DIR}/backend/include
)

target_link_libraries(bloodline_server PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
)

# On Linux, httplib needs pthread
if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(bloodline_server PRIVATE Threads::Threads)
endif()

# ─── Test executable ───
add_executable(bloodline_tests backend/tests/test_bloodline.cpp)

target_include_directories(bloodline_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/backend/include
)

target_link_libraries(bloodline_tests PRIVATE
    nlohmann_json::nlohmann_json
)

enable_testing()
add_test(NAME BloodLineTests COMMAND bloodline_tests)
