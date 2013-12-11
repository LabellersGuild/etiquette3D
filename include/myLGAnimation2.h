/** File: myLGAnimation2.h
 * Description : file that creates a sub-class of LGAnimation, and overrides the operator() method.
 * Other animation method.
 * Author : Thomas
 */


#ifndef MYLGANIMATION2_H
#define MYLGANIMATION2_H

#include "LGAnimation.h"


class myLGAnimation2 : public LGAnimation
{
    public:
        /** Constructor : uses the constructor of LGAnimation
        */
         myLGAnimation2(osgViewer::Viewer* viewer) : LGAnimation(viewer) {};

        /** Override of operator()
         */
         virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;
};

#endif // MYLGANIMATION2_H
