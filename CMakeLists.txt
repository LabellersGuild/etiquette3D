cmake_minimum_required(VERSION 2.6)

# name of the sources directory
set(SRCDIR "src")

# sources extension
set(SRCEXT "cpp")

# project name (also used as executable name)
project(main) 

# executable output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

# library output directory
# set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})  ### not used yet

# searching for libraries paths
find_package( osg )
find_package( osgViewer )
find_package( osgSim )
find_package( osgText ) 
find_package( osgDB )
find_package( osgGA )
find_package( osgUtil )
# ... add other useful osg libraries here
find_package( OpenThreads )
find_package( X11 )
find_package( OpenGL )

# macro used to link osg libraries
macro( config_project LIBNAME )
 include_directories( ${${LIBNAME}_INCLUDE_DIR} )
 target_link_libraries( ${CMAKE_PROJECT_NAME} ${${LIBNAME}_LIBRARY} )
endmacro()

# searching for source files : files in the SRCDIR directory with the SRCEXT extension.
file(
    GLOB_RECURSE
    source_files
    ${SRCDIR}/*.${SRCEXT}
)

# create executable
add_executable(
    ${CMAKE_PROJECT_NAME}
    ${source_files}
)

# linking osg libraries using config_project macro
config_project( OSG )
config_project( OSGDB )
config_project( OSGUTIL )
config_project( OSGVIEWER )
config_project( OSGTEXT )
config_project( OSGGA )
config_project( OSGUTIL )
# ... add other useful osg libraries here too

# linking other libraries
target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    ${X11_LIBRARIES}
    ${CMAKE_OPENGL_LIBS_INIT}
    m
    Xext
)
