/** File: InExSwitch.cpp
 * Description : file that creates a sub-class of LGAnimation, and overrides the operator() method.
 * Author : Mario & Thomas
 */

#include "../include/InExSwitch.h"
#include "../include/lgLabel.h"

using namespace osg;
using namespace std;

InExSwitch::InExSwitch(osgViewer::Viewer* viewer, ref_ptr<InternalLabelAnimation> internalLabelAnimation,  ref_ptr<myLGAnimation2> mylGAnimation2) : lgAnimation(viewer)
{
    this->internalLabelAnimation = internalLabelAnimation;
    this->mylGAnimation2 = mylGAnimation2;
}


 /** Override of operator()
  */
void InExSwitch::operator()(Node* node, NodeVisitor* nv)
{
    lgAnimation::operator()(node, nv);

     //Label
    ref_ptr<lgLabel> label = dynamic_cast<lgLabel*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(0))->getDrawable(0));
    bool thereIsEnoughSpace = internalLabelAnimation->enoughSpace(node, nv);

    //cout << "labelType : "<< label->getLabelType() << " enough space : "<< thereIsEnoughSpace << endl;

    if (thereIsEnoughSpace && (label->getLabelType() == INTERNAL_TOP || label->getLabelType() == INTERNAL_FACE))
    {
        internalLabelAnimation->operatorForSwitch(node, nv);
    }
    else if (thereIsEnoughSpace && label->getLabelType() == SWITCH)
    {
        label->setLabelType(INTERNAL_FACE);
        internalLabelAnimation->operatorForSwitch(node, nv);
        if (label->getArrow() != NULL)
        {
        }
    }
    else if (!thereIsEnoughSpace && (label->getLabelType() == INTERNAL_TOP || label->getLabelType() == INTERNAL_FACE))
    {
        label->setLabelType(SWITCH);
        mylGAnimation2->operatorForSwitch(node, nv);
        if (label->getArrow() != NULL)
        {

        }
    }
    else if (!thereIsEnoughSpace &&label->getLabelType() == SWITCH)
    {
      mylGAnimation2->operatorForSwitch(node, nv);
    }

    //Allow OSG to continue the node traversal
    traverse(node, nv);
}


