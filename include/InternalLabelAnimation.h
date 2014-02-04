#ifndef INTERNALLABELANIMATION_H_INCLUDED
#define INTERNALLABELANIMATION_H_INCLUDED

/** File: InternalLabelAnimation.h
 * Description : file that creates a sub-class of LGAnimation for internal labels.
 * Author : Thomas
 */

 #include "lgAnimation.h"
 #include "lgLabel.h"

class InternalLabelAnimation : public lgAnimation
{
    public :

     /** Constructor : uses the constructor of LGAnimation
      */
     InternalLabelAnimation(osgViewer::Viewer* viewer, osg::ref_ptr<lgLabel> label);

       /** Override of operator()
        */
     virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;

    protected :

    /** Label attached to the node
     */
    osg::ref_ptr<lgLabel> label;

    /** Bounding box of the object
     */
    osg::BoundingBox bbox;

    /** Corners of the bounding box, absolute frame of reference
     */
    osg::Vec3 aCorners[8];

    /** Corners of the bounding box, local frame of reference
     */
     osg::Vec3 lCorners[8];

     /** Normals of each face
      */
     osg::Vec3 normals[5];

     /**  Corners linked to each face,
      */
    int faceCorners[5][4];
};
#endif // INTERNALLABELANIMATION_H_INCLUDED
