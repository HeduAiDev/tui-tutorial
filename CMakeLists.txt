cmake_minimum_required(VERSION 3.18)
project(tui-tutorial VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
message(STATUS CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR})
if (WIN32)
    file(GLOB sources CONFIGURE_DEPENDS "src/mouse_event/windows/*.cpp")
    set(platform windows)
elseif (UNIX AND NOT APPLE)
    file(GLOB sources CONFIGURE_DEPENDS "src/mouse_event/linux/*.cpp")
    set(platform linux)
endif()
# mouse event
add_library(mouse_event SHARED src/mouse_event/${platform}/mouse_event.cpp)
target_include_directories(mouse_event PUBLIC src/mouse_event/${platform} 
                                        PUBLIC src/mouse_event 
                                        PUBLIC src)
add_executable(mouse_event_test test/mouse_event_test.cpp)
target_link_libraries(mouse_event_test mouse_event)

install(TARGETS mouse_event DESTINATION lib)
add_custom_command(TARGET mouse_event POST_BUILD COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR})
