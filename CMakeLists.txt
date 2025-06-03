cmake_minimum_required(VERSION 3.18)
project(passwordManager VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion")

Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)
FetchContent_MakeAvailable(Catch2)

find_package(OpenSSL REQUIRED)
find_package(SQLiteCpp REQUIRED)
find_package(wxWidgets REQUIRED COMPONENTS core base)

include(${wxWidgets_USE_FILE})


set(CPP_SRCS  
  app.cpp
  database.cpp
)
set(TEST_NAME pwManagerTest)
add_executable(${PROJECT_NAME} app.cpp ${CPP_SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE SQLiteCpp OpenSSL::SSL OpenSSL::Crypto ${wxWidgets_LIBRARIES})

# --- TESTS ---
add_executable(${TEST_NAME} pwManagerTest.cpp database.cpp)
target_link_libraries(${TEST_NAME} PRIVATE Catch2::Catch2WithMain SQLiteCpp OpenSSL::SSL OpenSSL::Crypto)
include(CTest)
add_test(NAME ${TEST_NAME}
         COMMAND ${TEST_NAME})