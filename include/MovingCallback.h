#ifndef MOVINGCALLBACK_H_INCLUDED
#define MOVINGCALLBACK_H_INCLUDED

#include <iostream>
#include <osg/NodeCallback>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgUtil/LineSegmentIntersector>
#include <osgText/TextBase>
#include <osgText/Text>

using namespace osg;

class MovingCallback : public NodeCallback
{
public:
    MovingCallback(osgViewer::Viewer* viewer, Vec3Array* HUDvertices) : view(viewer), HUDBackgroundVertices(HUDvertices) {};
    virtual void operator()(Node* node, NodeVisitor* nv)  ;
protected :
    osgViewer::Viewer* view;
    Vec3Array* HUDBackgroundVertices;
};

#endif // MOVINGCALLBACK_H_INCLUDED
