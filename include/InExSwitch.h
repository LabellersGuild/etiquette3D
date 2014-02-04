/** File: InExSwitch.h
 * Description : file that creates a sub-class of LGAnimation, and overrides the operator() method.
 * Author : Mario & Thomas
 */

#ifndef INEXSWITCH_H_INCLUDED
#define INEXSWITCH_H_INCLUDED

#include "InternalLabelAnimation.h"
#include "myLGAnimation2.h"

class InExSwitch : public lgAnimation
{
    public :

         /** Constructor : uses the constructor of InExSwitch
          */
         InExSwitch(osgViewer::Viewer* viewer, osg::ref_ptr<InternalLabelAnimation> internalLabelAnimation,  osg::ref_ptr<myLGAnimation2> mylGAnimation );

         /** Override of operator()
          */
         virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;

    protected :

          /** InternalLabelAnimation
           */
          osg::ref_ptr<InternalLabelAnimation> internalLabelAnimation;
          osg::ref_ptr<myLGAnimation2> mylGAnimation2;
};

#endif // INEXSWITCH_H_INCLUDED
