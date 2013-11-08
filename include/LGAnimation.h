#ifndef LGAnimation_H_INCLUDED
#define LGAnimation_H_INCLUDED

#include <iostream>
#include <osg/NodeCallback>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgUtil/LineSegmentIntersector>
#include <osgText/TextBase>
#include <osgText/Text>

class LGAnimation : public osg::NodeCallback
{
    public:
        LGAnimation(osgViewer::Viewer* viewer) : view(viewer) {};
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)  ;
    protected :
        osg::ref_ptr<osgViewer::Viewer> view;
    private :
        int myMax(int a, int b, int c, int d, int e, int f);
        int myMin(int a, int b, int c, int d, int e, int f);
};

#endif // LGAnimation_H_INCLUDED
