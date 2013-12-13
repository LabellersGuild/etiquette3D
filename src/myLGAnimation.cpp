/** File: myLGAnimation.cpp
 * Description : file that creates a sub-class of LGAnimation, and overrides the operator() method.
 * Author : Thomas
 */

#include "../include/myLGAnimation.h"
#include "../include/lgLabel.h"

using namespace osg;

 /** Override of operator()
  */
void myLGAnimation::operator()(Node* node, NodeVisitor* nv)
{
    lgAnimation::operator()(node, nv);

     //Label
    ref_ptr<lgLabel> label = dynamic_cast<lgLabel*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(0))->getDrawable(0));

    Vec3 center = label->getAbsolutePosition();

    if (intersectCameraLabel(node, nv) != NULL)
    {
       label->translateLabel(0, 0, 1);
    }
    else if (center.z()>0)
    {
        //Look if below the label is free space
        if (intersectCameraLabel(node, nv, 0,30,0,0) == NULL )
        {
            label->translateLabel(0, 0, -1);
        }
    }

    //Allow OSG to continue the node traversal
    traverse(node, nv);
}
