/** File : testLGAnimation.h
 * Description : Class used to test the LGAnimation functions.
 * Author : Thomas Brunel
 */

#ifndef TESTLGANIMATION_H
#define TESTLGANIMATION_H

#include "LGAnimation.h"


class testLGAnimation : public LGAnimation
{
    public:
        /** Constructor : uses the constructor of LGAnimation
         */
         testLGAnimation(osgViewer::Viewer* viewer) : LGAnimation(viewer) {};

         /** Override of operator()
          */
         virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;
};

#endif // TESTLGANIMATION_H
