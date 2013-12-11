/** File: myLGAnimation.h
 * Description : file that creates a sub-class of LGAnimation, and overrides the operator() method.
 * Author : Thomas
 */

#ifndef MYLGANIMATION_H_INCLUDED
#define MYLGANIMATION_H_INCLUDED

#include "LGAnimation.h"

class myLGAnimation : public LGAnimation
{
    public :

     /** Constructor : uses the constructor of LGAnimation
      */
     myLGAnimation(osgViewer::Viewer* viewer) : LGAnimation(viewer) {};

       /** Override of operator()
        */
     virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;
};
#endif // MYLGANIMATION_H_INCLUDED
