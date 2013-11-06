#ifndef PICKHANDLER_H_INCLUDED
#define PICKHANDLER_H_INCLUDED

#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgUtil/LineSegmentIntersector>
#include <osgText/Text>

using namespace osg;

class PickHandler : public osgGA::GUIEventHandler
{
public:
    virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
    PickHandler(ref_ptr<MatrixTransform> m) : model(m) {};
protected:
    ref_ptr<MatrixTransform> model;
};

#endif // PICKHANDLER_H_INCLUDED
