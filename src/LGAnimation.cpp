/** File : LGAnimation.cpp
 * Description : Source file of LGAnimation class
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

#include "../include/LGAnimation.h"
#include "../include/lgLabel.h"

using namespace osg;
using namespace std;

/** Manages the label position
  * @param node : Node* : the node linked to the lgAnimation instance. It should be the matrix transformation of the label.
  * @param nv : NodeVisitor
  */
void LGAnimation::operator()(Node* node, NodeVisitor* nv)
{
    //Get the label linked to the node
    ref_ptr<lgLabel> label = dynamic_cast<lgLabel*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(0))->getDrawable(0));

    //Hide the label if it further than the hiding distance
    int hidingDistance = label->getHidingDistance();

    if ( hidingDistance != -1 && hidingDistance < label->distanceCamera(view))
    {
        label->setDrawMode(0);
    }
    else
    {
        //Get the drawMode before it was hidden, or the current drawMode if it is not hidden.
        label->setDrawMode(label->getPreviousDrawMode());
    }
}

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
osgUtil::PolytopeIntersector::Intersection* LGAnimation::intersectCameraLabel(Node* node, NodeVisitor* nv, int Xmin, int Ymin, int Xmax, int Ymax)
{
    //Get the label linked to the node
    ref_ptr<lgLabel> label = dynamic_cast<lgLabel*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(0))->getDrawable(0));

    //2D bounding box of the label
    Vec4 bbox2d = label->compute2dBox(view);

    //Save the current matrixTransform
    Matrix matrixTransform= dynamic_cast<MatrixTransform*>(node)->getMatrix();

    //Find the object between the camera and the node :
    ref_ptr<osgUtil::PolytopeIntersector> intersector = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, bbox2d[0]-Xmin, bbox2d[1]-Ymin, bbox2d[2]-Xmax, bbox2d[3]-Ymax);
    osgUtil::IntersectionVisitor iv( intersector.get() );

    iv.setTraversalMask(~0x1);

    view->getCamera()->accept( iv );
    //If an object is detected
    if ( intersector->containsIntersections())
    {
        intersection = *(intersector->getIntersections().begin());
        if (intersection.drawable != label)
        {
            return &intersection;
        }
        return NULL;
    }
    return NULL;
}

