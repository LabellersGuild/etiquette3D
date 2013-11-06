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
        MovingCallback(osgViewer::Viewer* viewer) : view(viewer) {};
        virtual void operator()(Node* node, NodeVisitor* nv)  ;
    protected :
        ref_ptr<osgViewer::Viewer> view;
};

#endif // MOVINGCALLBACK_H_INCLUDED
