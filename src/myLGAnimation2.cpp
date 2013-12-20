/** File: myLGAnimation2.cpp
 * Description : file that creates a sub-class of LGAnimation, and overrides the operator() method.
 * Other animation method.
 * Author : Thomas
 */

#include "../include/myLGAnimation2.h"
#include "../include/lgLabel.h"

using namespace osg;

/** Override of operator()
 */
void myLGAnimation2::operator()(Node* node, NodeVisitor* nv)
{
    lgAnimation::operator()(node, nv);

     //Label
    ref_ptr<lgLabel> label = dynamic_cast<lgLabel*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(0))->getDrawable(0));

    //Intersection
    osgUtil::PolytopeIntersector::Intersection* intersection = intersectCameraLabel(node, nv);

    if (intersection != NULL)
    {
        //Position of the intersection
        Vec4 intersectionPoint = Vec4(intersection->localIntersectionPoint.x(), intersection->localIntersectionPoint.y(), intersection->localIntersectionPoint.z(), 1);

        //getting the list of transposed transformation matrices, from node to root
        MatrixList matricesList = intersection->drawable->getParent(0)->getWorldMatrices();
        for (unsigned i=0;i<matricesList.size();i++){
            intersectionPoint = intersectionPoint*matricesList[i];
        }
        Vec3 intersectionPosition = Vec3(intersectionPoint.x(),intersectionPoint.y(),intersectionPoint.z());

         //Bounding box of the intersected drawable :
        BoundingBox intersectionBox = intersection->drawable->getBound();

        //distance between the intersection and the maxZ of the drawable
        float deltaZ = fabs(intersection->localIntersectionPoint.z() -intersectionBox.zMax());

        //Distance intersection-camera :
        Matrix matrixCamera = view->getCamera()->getInverseViewMatrix();
        Vec3 positionCamera= Vec3(matrixCamera(3,0), matrixCamera(3,1), matrixCamera(3,2));
        float distanceIntersectionCamera = sqrt(pow(intersectionPosition.x()-positionCamera.x(),2.0)+pow(intersectionPosition.y()-positionCamera.y(),2.0)+pow(intersectionPosition.z()-positionCamera.z(),2.0));

        //Thales :
        float deltaZLabel = deltaZ * label->distanceCamera(view) / distanceIntersectionCamera;

        label->translateLabel(0,0,deltaZLabel);
    }
    else //If the label is not hidden, we look at its initial position and test if we can put it there without being hidden
    {
        if (intersectCameraLabel(node, nv, 0,label->getAbsolutePosition().z() - label->getPositionInit().z()+20,0,0) == NULL)
        {
            label->setUpdatedMatrixMatrix(Matrixd::translate(label->getPositionInit()));
        }
    }

    //Allow OSG to continue the node traversal
    traverse(node, nv);
}
