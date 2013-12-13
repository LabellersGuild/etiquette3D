/** File : testLGAnimation.h
 * Description : Class used to test the LGAnimation functions.
 * Author :
 */

#ifndef TESTLGANIMATION_H
#define TESTLGANIMATION_H

#include "lgAnimation.h"


class testLGAnimation : public lgAnimation
{
    public:
        /** Constructor : uses the constructor of LGAnimation
         */
         testLGAnimation(osgViewer::Viewer* viewer) : lgAnimation(viewer) {};

         /** Override of operator()
          */
         virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;
};

#endif // TESTLGANIMATION_H
