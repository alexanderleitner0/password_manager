cmake_minimum_required(VERSION 3.18)
project(passwordManager VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion")

find_package(OpenSSL REQUIRED)
find_package(SQLiteCpp REQUIRED)
find_package(wxWidgets REQUIRED COMPONENTS core base)

include(${wxWidgets_USE_FILE})


set(CPP_SRCS  
  app.cpp
  database.cpp
)

add_executable(${PROJECT_NAME} app.cpp ${CPP_SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE SQLiteCpp OpenSSL::SSL OpenSSL::Crypto ${wxWidgets_LIBRARIES})