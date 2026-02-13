# CMakeList.txt : CMake project for GraphBackend, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (GraphBackend "GraphBackend.cpp" "GraphBackend.h"  "PriceGenerator.h" "PriceGenerator.cpp" "WebSocketConnection.h" "WebSocketConnection.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GraphBackend PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
