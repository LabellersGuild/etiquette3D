#include "../include/LGAnimation.h"

using namespace osg;

int LGAnimation::myMax(int a, int b, int c, int d, int e, int f){
    int max1 = a > b ? a : b;
    int max2 = c > d ? c : d;
    int max3 = e > f ?  e : f;
    int result = max1 > max2 ? max1 : max2;
    result = result > max3 ? result : max3;
    return result;
}

int LGAnimation::myMin(int a, int b, int c, int d, int e, int f){
    int min1 = a < b ? a : b;
    int min2 = c < d ? c : d;
    int min3 = e < f ?  e : f;
    int result = min1 < min2 ? min1 : min2;
    result = result < min3 ? result : min3;
    return result;
}

void LGAnimation::operator()(Node* node, NodeVisitor* nv)
{
    // Find the world coordinates of the node :
    Matrix worldMatrix = node->getWorldMatrices()[0];
    Vec3 center = Vec3(worldMatrix(3,0), worldMatrix(3,1), worldMatrix(3,2));

    //With a line :
    {/*
         // Find the worldcoordinates of the camera :
        Matrix matrixCamera = view->getCamera()->getInverseViewMatrix();
        Vec3 originCamera= Vec3(matrixCamera(3,0), matrixCamera(3,1), matrixCamera(3,2));

         //Find the intersection between the camera and the node :
        ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::MODEL, originCamera, center);

        //Save the current matrixTransform
        Matrix matrixTransform= dynamic_cast<MatrixTransform*>(node)->getMatrix();

        osgUtil::IntersectionVisitor iv( intersector.get() );
        view->getCamera()->accept( iv );

        //If there are more than one intersection (there is another object in front of the drawable of the node)
        if ( intersector->containsIntersections() && intersector->getIntersections().size()>1)
        {
           dynamic_cast<MatrixTransform*>(node)->setMatrix(matrixTransform * Matrix::translate(0,0,0.3));
        }

        //If not, lower the node
        else if (center[2]>0)
        {
            dynamic_cast<MatrixTransform*>(node)->setMatrix(matrixTransform * Matrix::translate(0,0,-0.3));
        }

    */}

    //With a polytope :
     /// TO DO : Problems with the bounds of the polytope
    {
        //Bounding box of the label :
        BoundingBox bbox = dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(0))->getDrawable(0)->getBound();
       // std::cout << "label1 : xMax = " << bbox.xMax() << " xMin = " << bbox.xMin() << " yMax = " << bbox.yMax() << " yMin = " << bbox.yMin() << " zMax = " << bbox.zMax() << " zMin = " << bbox.zMin() << std::endl;

        //Matrix to change world coordinates in windows coordinates
        Matrix modelView = view->getCamera()->getViewMatrix();
        Matrix projection = view->getCamera()->getProjectionMatrix();
        Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
        Matrix MVPW = modelView * projection * window;

         //Projection of the bounding box of the label :
        Vec3 screenxMin = (center + Vec3(bbox.xMin(),0,0)) * MVPW;
        Vec3 screenxMax = (center + Vec3(bbox.xMax(),0,0)) * MVPW;
        Vec3 screenyMin = (center + Vec3(0,bbox.yMin(),0)) * MVPW;
        Vec3 screenyMax = (center + Vec3(0,bbox.yMax(),0)) * MVPW;
        Vec3 screenzMin = (center + Vec3(0,0,bbox.zMin())) * MVPW;
        Vec3 screenzMax = (center + Vec3(0,0,bbox.zMax())) * MVPW;

        // If info label :
        if (dynamic_cast<Group*>(node)->getNumChildren()>1)
        {
            if (dynamic_cast<osgText::Text*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(1))->getDrawable(0))->getDrawMode() != 0)
            {
                BoundingBox bboxInfo = dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(1))->getDrawable(0)->getBound();
                screenxMin = (center + Vec3(std::max(bboxInfo.xMin(),bbox.xMin()),0,0)) * MVPW;
                screenxMax = (center + Vec3(std::max(bboxInfo.xMax(),bbox.xMax()),0,0)) * MVPW;
                screenyMin = (center + Vec3(0,std::max(bboxInfo.yMin(),bbox.yMin()),0)) * MVPW;
                screenyMax = (center + Vec3(0,std::max(bboxInfo.yMax(),bbox.yMax()),0)) * MVPW;
                screenzMin = (center + Vec3(0,0,bboxInfo.zMin())) * MVPW;
                screenzMax = (center + Vec3(0,0,bboxInfo.zMax()+bbox.zMax())) * MVPW;
            }
        }

        //std::cout << "(" << screenxMin[0] <<" , " << screenxMin[1] << ")("<<screenxMax[0] << " , " <<screenxMax[1] <<")("<< screenyMin[0] << " , " << screenyMin[1] << ")(" << screenyMax[0] << " , "<<screenyMax[1]<<")(" << screenzMin[0] << " , " << screenzMin[1] << ")("<< screenzMax[0] <<" , " << screenzMax[1] << ")" <<std::endl;

        // The bounds of the polytope are determined by the largest rectangle including the projection of the bbox
        int maxX = myMax(screenxMin[0], screenxMax[0], screenyMin[0], screenyMax[0], screenzMin[0], screenzMax[0]);
        int minX = myMin(screenxMin[0], screenxMax[0], screenyMin[0], screenyMax[0], screenzMin[0], screenzMax[0]);
        int maxY = myMax(screenxMin[1], screenxMax[1], screenyMin[1], screenyMax[1], screenzMin[1], screenzMax[1]);
        int minY = myMin(screenxMin[1], screenxMax[1], screenyMin[1], screenyMax[1], screenzMin[1], screenzMax[1]);
        //std::cout << "minX = "<<minX<<" minY= "<<minY<<" maxX = "<<maxX<<" maxY "<<maxY<<std::endl;
        /// TO DO ? the minY/maxY are correct only when the camera is perfectly on the Y axis

        //Save the current matrixTransform
        Matrix matrixTransform= dynamic_cast<MatrixTransform*>(node)->getMatrix();

        //Find the intersection between the camera and the node :
        ref_ptr<osgUtil::PolytopeIntersector> intersector = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, minX, minY, maxX, maxY);

        osgUtil::IntersectionVisitor iv( intersector.get() );

        // If no info label :
        if (dynamic_cast<Group*>(node)->getNumChildren()<2)
        {
             iv.setTraversalMask( ~0x1 );
        }
        else
        {
            if (dynamic_cast<osgText::Text*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(1))->getDrawable(0))->getDrawMode() == 0)
            {
                 iv.setTraversalMask( ~0x1 );
            }
        }

        view->getCamera()->accept( iv );

        if ( intersector->containsIntersections())
        {
           osgUtil::PolytopeIntersector::Intersection result = *(intersector->getIntersections().begin());
            if (result.drawable->getParent(0)->getParent(0) != node)
            {
                dynamic_cast<MatrixTransform*>(node)->setMatrix(matrixTransform * Matrix::translate(0,0,1));
            }
            //If not, lower it
            else if (center[2]>0)
            {
                // look below the label if we can lower it :
                ref_ptr<osgUtil::PolytopeIntersector> intersectorBelow = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, minX, minY-30, maxX, maxY);
                osgUtil::IntersectionVisitor ivBelow( intersectorBelow.get() );

                view->getCamera()->accept( ivBelow );
                if (intersectorBelow->containsIntersections())
                {
                    osgUtil::PolytopeIntersector::Intersection resultBelow = *(intersectorBelow->getIntersections().begin()); /// TO DO : the label is not fully lowered if there is a building on the back
                    // Check if we still see the label
                    if (resultBelow.drawable->getParent(0)->getParent(0) == node)
                    {
                        //if so, put lower the label
                        dynamic_cast<MatrixTransform*>(node)->setMatrix(matrixTransform * Matrix::translate(0,0,-1));
                    }
                }
            }
        }
        else
        {
            //The label is not here : not normal
        }

    }
    //Allow OSG to continue the node traversal
    traverse(node, nv);
}
