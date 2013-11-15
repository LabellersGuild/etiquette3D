cmake_minimum_required(VERSION 2.6)

set(SRCDIR "src")
set(SRCEXT "cpp")

project(LGProject)
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})


find_package( osg )
find_package( osgViewer )
find_package( osgSim )
find_package( osgText ) 
find_package( osgDB )
find_package( OpenThreads )
find_package( X11 )
find_package( OpenGL )

macro( config_project PROJNAME LIBNAME )
 include_directories( ${${LIBNAME}_INCLUDE_DIR} )
 target_link_libraries( ${PROJNAME} ${${LIBNAME}_LIBRARY} )
endmacro()


file(
    GLOB_RECURSE
    source_files
    ${SRCDIR}/*.${SRCEXT}
)

add_executable(
    LGProject
    ${source_files}
)
config_project( LGProject OPENTHREADS )
config_project( LGProject OSG )
config_project( LGProject OSGDB )
config_project( LGProject OSGUTIL )
config_project( LGProject OSGVIEWER )

target_link_libraries(
    LGProject
	${X11_LIBRARIES}
	${CMAKE_OPENGL_LIBS_INIT}
	m
	Xext
)
