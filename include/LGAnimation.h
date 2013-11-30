/** File : LGAnimation.h
 * Description : Header of LGAnimation class
 *      This class is used to dynamically place the labels.
 *      To use it, create a sub-class that redefines the method operator().
 *      In this method, you can use the other methods of the class, but it must start by :
 *      " LGAnimation::operator()(node, nv);"
 *      and finish with : "traverse(node, nv);" which allows OSG to continue the node traversal.
 *      Then, create an instance of the sub-class and link it to the transformation matrix of the label :
 *      matriceDeTransformation->setUpdateCallback( new LGAnimationSousClasse(&viewer));
 *      or use it as an argument of the function setLabelType of the lgLabel class.
 * Author : Thomas Brunel
 */

#ifndef LGAnimation_H_INCLUDED
#define LGAnimation_H_INCLUDED

#include <osg/NodeCallback>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgUtil/LineSegmentIntersector>
#include <osgText/TextBase>
#include <osgText/Text>

class LGAnimation : public osg::NodeCallback
{
    public:
        /** Constructor
         * @param viewer : osgViewer::Viewer* : the viewer object of the main file.
         */
        LGAnimation(osgViewer::Viewer* viewer) : view(viewer) {};

        /** Manages the label position
         * @param node : Node* : the node linked to the lgAnimation instance. It should be the matrix transformation of the label.
         * @param nv : NodeVisitor
         */
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;

       /** Tell if the labek and its surrounding are hidden by an object.
        * To know if the the label is hidden, do not set the last 4 arguments (0 by default)
        * @param node : Node* : the node linked to the lgAnimation instance. It should be the matrix transformation of the label.
        * @param NodeVisitor* : NodeVisitor
        * @param int Xmin : Increase the checking window on the left of the screen. 0 by default
        * @param int Ymin : Increase the checking window on the bottom of the screen. 0 by default
        * @param int Xmax : Increase the checking window on the right of the screen. 0 by default
        * @param int Ymax : Increase the checking window on the top of the screen. 0 by default
        * @return the position of the point of the object that is between the labele and the camera. Return NULL if the label is not hidden.
        */
        osgUtil::PolytopeIntersector::Intersection* intersectCameraLabel(osg::Node* node, osg::NodeVisitor* nv, int Xmin=0, int Ymin=0, int Xmax=0, int Ymax=0);

    protected :

        /** viewer object from the main file
        */
        osg::ref_ptr<osgViewer::Viewer> view;

        /** Record the object interesected between the label and the camera. Returned by the method intersectCameraLabel
         */
        osgUtil::PolytopeIntersector::Intersection intersection;
};

#endif // LGAnimation_H_INCLUDED
