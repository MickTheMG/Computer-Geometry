cmake_minimum_required(VERSION 3.21)
project(KompGeom LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)

set(CPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp)
set(QT_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/qt)

set(SRC_CPP
  ${CPP_DIR}/common/BigInt.cpp
  ${CPP_DIR}/common/BigDecimal.cpp
  ${CPP_DIR}/common/Geometry.cpp
)

set(SRC_QT
  ${QT_DIR}/main.cpp
  ${QT_DIR}/MainWindow.cpp
  ${QT_DIR}/widgets/tasks7_12/Task07to09Widget.cpp
  ${QT_DIR}/widgets/tasks7_12/Task10Widget.cpp
  ${QT_DIR}/widgets/tasks7_12/Task11Widget.cpp
  ${QT_DIR}/widgets/tasks7_12/Task12Widget.cpp
  ${QT_DIR}/widgets/tasks7_12/Task13Widget.cpp
)

qt_standard_project_setup()

qt_add_executable(KompGeom ${SRC_CPP} ${SRC_QT})

target_include_directories(KompGeom PRIVATE ${CPP_DIR} ${QT_DIR})

target_link_libraries(KompGeom PRIVATE Qt6::Widgets Qt6::Gui)

install(TARGETS KompGeom RUNTIME DESTINATION bin)
