/* Fichier : myLGAnimation2.cpp
 * Description : Fichier créant une sous classe de LGAnimation, pour redéfinir la méthode operator().
 * Autre méthode de placement des étiquettes
 * Auteur : Thomas Brunel
 */

#include "../include/myLGAnimation2.h"
#include "../include/lgLabel.h"

using namespace osg;

void myLGAnimation2::operator()(Node* node, NodeVisitor* nv)
{
    LGAnimation::operator()(node, nv);

     //Label
    ref_ptr<lgLabel> label = dynamic_cast<lgLabel*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(0))->getDrawable(0));

    //Intersection
    osgUtil::PolytopeIntersector::Intersection* intersection = intersectCameraLabel(node, nv);

    if (intersection != NULL)
    {
        //Position of the intersection
        osg::Vec4 intersectionPoint = osg::Vec4(intersection->localIntersectionPoint.x(), intersection->localIntersectionPoint.y(), intersection->localIntersectionPoint.z(), 1);

        //getting the list of transposed transformation matrices, from node to root
        osg::MatrixList matricesList = intersection->drawable->getParent(0)->getWorldMatrices();
        for (unsigned i=0;i<matricesList.size();i++){
            intersectionPoint = intersectionPoint*matricesList[i];
        }
        osg::Vec3 intersectionPosition = osg::Vec3(intersectionPoint.x(),intersectionPoint.y(),intersectionPoint.z());

         //Bounding box of the intersected drawable :
        BoundingBox intersectionBox = intersection->drawable->getBound();

        //distance between the intersection and the maxZ of the drawable
        float deltaZ = fabs(intersectionPoint.z()-intersectionBox.zMax());

        //Distance intersection-camera :
        osg::Matrix matrixCamera = view->getCamera()->getInverseViewMatrix();
        osg::Vec3 positionCamera= osg::Vec3(matrixCamera(3,0), matrixCamera(3,1), matrixCamera(3,2));
        float distanceIntersectionCamera = sqrt(pow(intersectionPosition.x()-positionCamera.x(),2.0)+pow(intersectionPosition.y()-positionCamera.y(),2.0)+pow(intersectionPosition.z()-positionCamera.z(),2.0));

        //Thales :
        float deltaZLabel = deltaZ * label->distanceCamera(view) / distanceIntersectionCamera;
        label->translateLabel(0,0,deltaZLabel);
    }
    else
    {
        int i = 0;
        while (intersectCameraLabel(node, nv, 0,i+30,0,0) == NULL && (label->getAbsolutePosition().z()-i-30 > 0))
        {
            i+=30;
        }
        label->translateLabel(0,0, -i*0.1);
    }

    //Allow OSG to continue the node traversal
    traverse(node, nv);
}
