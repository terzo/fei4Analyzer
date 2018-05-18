cmake_minimum_required(VERSION 3.9.3)

project(fei4Analyzer)
project(rawDataConverter)

if(APPLE)
add_definitions(-DAPPLE)
    SET( CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang" )
    SET(CMAKE_C_COMPILER "/usr/local/opt/llvm/bin/clang")
    SET(STDLIB_FLAGS "-stdlib=libc++")
    LINK_DIRECTORIES(/usr/local/opt/llvm/lib)
    SET(LINK_LIBS "-lc++")
elseif("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    ADD_DEFINITIONS(-DLINUX)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -fopenmp ${STDLIB_FLAGS}")

# include directories
INCLUDE_DIRECTORIES( ./inc /opt/local/include )

# Additional packages to be searched for by cmake
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

#option(lcio "Build ready for lcio output" OFF)
FIND_PACKAGE(LCIO)
if(LCIO_FOUND)
    INCLUDE_DIRECTORIES(${LCIO_INCLUDE_DIR})
    SET(LCIO_LIBS "-llcio -lsio")
    #SET(LCIO_LIBS_PATH "${LCIO_INCLUDE_DIR}/../lib")
    LINK_DIRECTORIES(${LCIO_INCLUDE_DIR}/../lib)
    SET(LCIO_SOURCE inc/LCIOEventMaker.h    src/LCIOEventMaker.cpp)
    ADD_DEFINITIONS(-DUSE_LCIO)
else()
    MESSAGE(STATUS "LCIO not found")
    #        message(FATAL_ERROR "LCIO not found. Stop processing. Use -Dlcio=OFF or point the system variable LCIO to the corresponding path.")

endif()

set(SOURCE_FILES inc/EventMaker.h        src/EventMaker.cpp
                inc/USBpixEventMaker.h  src/USBpixEventMaker.cpp
                inc/CosmicEventMaker.h  src/CosmicEventMaker.cpp
                inc/fei4TelEventMaker.h src/fei4TelEventMaker.cpp
                inc/tbtrackEventMaker.h src/tbtrackEventMaker.cpp
                inc/Clusterizer.h       src/Clusterizer.cpp
                inc/Plotter.h           src/Plotter.cpp
                inc/Calibrator.h        src/Calibrator.cpp
                inc/Fitter.h            src/Fitter.cpp
                inc/macros.h            inc/ANSIColors.h
                ${LCIO_SOURCE}
)

##configuration to include root
FIND_PACKAGE(ROOT REQUIRED COMPONENTS Minuit)
INCLUDE_DIRECTORIES(SYSTEM ${ROOT_INCLUDE_DIR} inc/)
EXECUTE_PROCESS(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_EXE_LINKER_FLAGS "${ROOT_LD_FLAGS}")

#find_package(OpenMP REQUIRED)
#IF(OPENMP_FOUND)
#    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#ENDIF()

SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/boost/boost-1.62.0-python2.7-x86_64-slc6-gcc62/include")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/boost/boost-1.62.0-python2.7-x86_64-slc6-gcc62/lib")
find_package(Boost COMPONENTS regex)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

add_executable(fei4Analyzer fei4Analyzer.C ${SOURCE_FILES})
target_link_libraries(fei4Analyzer Boost::regex -lz -lMinuit ${LINK_LIBS} ${LCIO_LIBS})

add_executable(rawDataConverter rawDataConverter.C ${SOURCE_FILES})
target_link_libraries(rawDataConverter Boost::regex -lz -lMinuit ${LINK_LIBS} ${LCIO_LIBS})

SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
INSTALL(TARGETS fei4Analyzer rawDataConverter
        DESTINATION bin)
