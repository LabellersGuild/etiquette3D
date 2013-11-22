/* Fichier : myLGAnimation.cpp
 * Description : Fichier créant une sous classe de LGAnimation, pour redéfinir la méthode operator()
 * Auteur : Thomas Brunel
 */

#include "../include/myLGAnimation.h"
#include "../include/lgLabel.h"

using namespace osg;

void myLGAnimation::operator()(Node* node, NodeVisitor* nv)
{
    LGAnimation::operator()(node, nv);

     //Label
    ref_ptr<lgLabel> label = dynamic_cast<lgLabel*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(0))->getDrawable(0));

    Vec3 center = label->getAbsolutePosition();

    if (!isFree(node, nv))
    {
       label->translateLabel(0, 0, 1);
    }
    else if (center[2]>0)
    {
        //Look if below the label is free space
        if (isFree(node, nv, 0,30,0,0))
        {
            label->translateLabel(0, 0, -1);
        }
    }

    //Allow OSG to continue the node traversal
    traverse(node, nv);
}
