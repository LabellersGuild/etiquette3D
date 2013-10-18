cmake_minimum_required(VERSION 2.6)

set(SRCDIR "src")
set(SRCEXT "cpp")

project(LGProject)
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})


FIND_PACKAGE(OpenSceneGraph REQUIRED)
FIND_PACKAGE(osgViewer REQUIRED)
FIND_PACKAGE(osgSim REQUIRED)
FIND_PACKAGE(osgText REQUIRED)
FIND_PACKAGE(osgDB REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(X11 REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

file(
    GLOB_RECURSE
    source_files
    ${SRCDIR}/*.${SRCEXT}
)

add_executable(
    LGProject
    ${source_files}
)

include_directories(${OSG_INCLUDE_DIR})
	
target_link_libraries(
    LGProject
	${OPENSCENEGRAPH_LIBRARIES}
	${osgViewer_LIBRARIES}
	${osgSim_LIBRARIES}
	${osgText_LIBRARIES}
	${osgDB_LIBRARIES}
	${X11_LIBRARIES}
	${CMAKE_OPENGL_LIBS_INIT}
	${OPENTHREADS_LIBRARIES}
	m
	Xext
)