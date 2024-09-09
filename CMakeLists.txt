cmake_minimum_required(VERSION 3.20)
project(ecosystem)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Widgets Charts REQUIRED)


include_directories(include)

set(SOURCES
        src/main.cpp
        src/Reserve.cpp
        src/Organism.cpp
        src/Herbivore.cpp
        src/Carnivore.cpp
        src/Plant.cpp
        src/Scavenger.cpp
        src/SimulationThread.cpp
        src/PoisonousPlant.cpp
        src/SimulationController.cpp
        gui/MainWindow.cpp
        gui/InteractiveOrganism.cpp
)

set(HEADERS
        include/Reserve.h
        include/Carnivore.h
        include/Herbivore.h
        include/Organism.h
        include/Plant.h
        include/Scavenger.h
        include/PoisonousPlant.h
        include/SimulationThread.h
        include/SimulationController.h
        include/TerrainType.h
        gui/MainWindow.h
        gui/InteractiveOrganism.h
)
qt5_wrap_cpp(MOC_SOURCES include/Reserve.h)
qt5_wrap_cpp(MOC_SOURCES include/SimulationThread.h)
qt5_wrap_cpp(MOC_SOURCES gui/MainWindow.h)

qt5_add_resources(RESOURCES resources.qrc)

add_executable(ecosystem ${SOURCES} ${MOC_SOURCES} ${RESOURCES})
target_link_libraries(ecosystem Qt5::Widgets)
target_link_libraries(ecosystem Qt5::Widgets Qt5::Charts)


include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/release-1.12.1.tar.gz
)
FetchContent_MakeAvailable(googletest)

enable_testing()


set(TEST_SOURCES
        tests/OrganismTest.cpp

)

add_executable(ecosystem_tests ${TEST_SOURCES} ${SOURCES})
target_link_libraries(ecosystem_tests gtest gtest_main pthread)
target_link_libraries(ecosystem_tests Qt5::Widgets Qt5::Charts)

add_test(NAME OrganismTest COMMAND ecosystem_tests)