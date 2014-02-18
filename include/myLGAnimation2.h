/** File: myLGAnimation2.h
 * Description : file that creates a sub-class of LGAnimation, and overrides the operator() method.
 * Other animation method.
 * Author : Thomas
 */


#ifndef MYLGANIMATION2_H
#define MYLGANIMATION2_H

#include "lgAnimation.h"
#include "lgLabel.h"

class myLGAnimation2 : public lgAnimation
{
    public:
        /** Constructor : uses the constructor of LGAnimation
        */
         myLGAnimation2(osgViewer::Viewer* viewer) : lgAnimation(viewer){} ;

        /** Override of operator()
         */
         virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;

         /** operator for Switch
          */
         void operatorForSwitch(osg::Node* node, osg::NodeVisitor* nv);

        /** Setter of the mapLabel
        */
        void setMapLabel(std::map<osg::ref_ptr<osg::Drawable>, std::vector<osg::ref_ptr<lgLabel> > > *mapLabel);

        /** Getter of the mapLabel
        */
        std::map<osg::ref_ptr<osg::Drawable>, std::vector<osg::ref_ptr<lgLabel> > >  * getMapLabel();


         /** hashmap of the labels
         */
        static std::map<osg::ref_ptr<osg::Drawable>, std::vector<osg::ref_ptr<lgLabel> > > *mapLabel;

};

#endif // MYLGANIMATION2_H
