cmake_minimum_required(VERSION 2.6)

set(SRCDIR "src")
set(SRCEXT "cpp")

project(main)
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})


find_package( osg )
find_package( osgViewer )
find_package( osgSim )
find_package( osgText ) 
find_package( osgDB )
find_package( osgGA )
find_package( osgUtil )
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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

add_executable(
    ${CMAKE_PROJECT_NAME}
    ${source_files}
)
config_project( ${CMAKE_PROJECT_NAME} OPENTHREADS )
config_project( ${CMAKE_PROJECT_NAME} OSG )
config_project( ${CMAKE_PROJECT_NAME} OSGDB )
config_project( ${CMAKE_PROJECT_NAME} OSGUTIL )
config_project( ${CMAKE_PROJECT_NAME} OSGVIEWER )
config_project( ${CMAKE_PROJECT_NAME} OSGTEXT )
config_project( ${CMAKE_PROJECT_NAME} OSGGA )
config_project( ${CMAKE_PROJECT_NAME} OSGUTIL )

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    ${X11_LIBRARIES}
    ${CMAKE_OPENGL_LIBS_INIT}
    m
    Xext
)
