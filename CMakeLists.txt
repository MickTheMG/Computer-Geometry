cmake_minimum_required(VERSION 3.21)
project(KompGeom LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)

set(SRC
  src/main.cpp
  src/MainWindow.h
  src/MainWindow.cpp
  src/common/Geometry.h
  src/common/Geometry.cpp
  src/common/BigInt.h
  src/common/BigInt.cpp
  src/common/BigDecimal.h
  src/common/BigDecimal.cpp
  src/widgets/Task01Widget.h
  src/widgets/Task01Widget.cpp
  src/widgets/Task02Widget.h
  src/widgets/Task02Widget.cpp
  src/widgets/Task03Widget.h
  src/widgets/Task03Widget.cpp
  src/widgets/Task04Widget.h
  src/widgets/Task04Widget.cpp
  src/widgets/Task05Widget.h
  src/widgets/Task05Widget.cpp
  src/widgets/Task06Widget.h
  src/widgets/Task06Widget.cpp
  src/widgets/tasks7_12/Task07to09Widget.h
  src/widgets/tasks7_12/Task07to09Widget.cpp
  src/widgets/tasks7_12/Task10Widget.h
  src/widgets/tasks7_12/Task10Widget.cpp
  src/widgets/tasks7_12/Task11Widget.h
  src/widgets/tasks7_12/Task11Widget.cpp
  src/widgets/tasks7_12/Task12Widget.h
  src/widgets/tasks7_12/Task12Widget.cpp
)

qt_standard_project_setup()

qt_add_executable(KompGeom ${SRC})

target_link_libraries(KompGeom PRIVATE Qt6::Widgets Qt6::Gui)

install(TARGETS KompGeom RUNTIME DESTINATION bin)
