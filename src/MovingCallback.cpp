#include "../include/MovingCallback.h"

using namespace osg;

int myMax(int a, int b, int c, int d, int e, int f){
    int max1 = a > b ? a : b;
    int max2 = c > d ? c : d;
    int max3 = e > f ?  e : f;
    int result = max1 > max2 ? max1 : max2;
    result = result > max3 ? result : max3;
    return result;
}

int myMin(int a, int b, int c, int d, int e, int f){
    int min1 = a < b ? a : b;
    int min2 = c < d ? c : d;
    int min3 = e < f ?  e : f;
    int result = min1 < min2 ? min1 : min2;
    result = result < min3 ? result : min3;
    return result;
}

void MovingCallback::operator()(Node* node, NodeVisitor* nv)
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
    {

        //Bounding box of the label :
        BoundingBox bbox = dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(0))->getDrawable(0)->getBound();
       // std::cout << "label1 : xMax = " << bbox.xMax() << " xMin = " << bbox.xMin() << " yMax = " << bbox.yMax() << " yMin = " << bbox.yMin() << " zMax = " << bbox.zMax() << " zMin = " << bbox.zMin() << std::endl;
        /// TO DO : the bounding box does not represent the place taken by the label on the screen. Find a way to know the bounding box of the label according to the window.
        ///Try to use the projection matrix and a PolytopeIntersector

        Matrix modelView = view->getCamera()->getViewMatrix();
        Matrix projection = view->getCamera()->getProjectionMatrix();
        Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
        Matrix MVPW = modelView * projection * window;

        Vec3 screenxMin = (center + Vec3(bbox.xMin(),0,0)) * MVPW;
        Vec3 screenxMax = (center + Vec3(bbox.xMax(),0,0)) * MVPW;
        Vec3 screenyMin = (center + Vec3(0,bbox.yMin(),0)) * MVPW;
        Vec3 screenyMax = (center + Vec3(0,bbox.yMax(),0)) * MVPW;
        Vec3 screenzMin = (center + Vec3(0,0,bbox.zMin())) * MVPW;
        Vec3 screenzMax = (center + Vec3(0,0,bbox.zMax())) * MVPW;

        //std::cout << "(" << screenxMin[0] <<" , " << screenxMin[1] << ")("<<screenxMax[0] << " , " <<screenxMax[1] <<")("<< screenyMin[0] << " , " << screenyMin[1] << ")(" << screenyMax[0] << " , "<<screenyMax[1]<<")(" << screenzMin[0] << " , " << screenzMin[1] << ")("<< screenzMax[0] <<" , " << screenzMax[1] << ")" <<std::endl;

        int maxX = myMax(screenxMin[0], screenxMax[0], screenyMin[0], screenyMax[0], screenzMin[0], screenzMax[0]);
        int minX = myMin(screenxMin[0], screenxMax[0], screenyMin[0], screenyMax[0], screenzMin[0], screenzMax[0]);
        int maxY = myMax(screenxMin[1], screenxMax[1], screenyMin[1], screenyMax[1], screenzMin[1], screenzMax[1]);
        int minY = myMin(screenxMin[1], screenxMax[1], screenyMin[1], screenyMax[1], screenzMin[1], screenzMax[1]);

        //std :: cout << screenCenter[0] << " , " << screenCenter[1] << " , " << screenCenter[2] << std::endl;
        std::cout << "minX = "<<minX<<" minY= "<<minY<<" maxX = "<<maxX<<" maxY "<<maxY<<std::endl;


        /// 2 problems : update not done correctly, and the minY/maxY are correct only when the camera is perfectly on the Y axis (maybe ??).
        /*
        *HUDBackgroundVertices)[0] = Vec3(   960, 646,-1);
        (*HUDBackgroundVertices)[1] = Vec3(   1096,  646,-1);
        (*HUDBackgroundVertices)[2] = Vec3(   1096,  678,-1);
        (*HUDBackgroundVertices)[3] = Vec3(   960,  678,-1);
        */

        //Save the current matrixTransform
        Matrix matrixTransform= dynamic_cast<MatrixTransform*>(node)->getMatrix();

        //Find the intersection between the camera and the node :
        ref_ptr<osgUtil::PolytopeIntersector> intersector = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, minX, minY, maxX, maxY);

        osgUtil::IntersectionVisitor iv( intersector.get() );
        view->getCamera()->accept( iv );

        if ( intersector->containsIntersections())
        {
           osgUtil::PolytopeIntersector::Intersection result = *(intersector->getIntersections().begin());
            if (! result.drawable->isSameKindAs(new osgText::Text()))
            {
                dynamic_cast<MatrixTransform*>(node)->setMatrix(matrixTransform * Matrix::translate(0,0,0.3));
            }
            //If not, lower the node
            else if (center[2]>0)
            {
                dynamic_cast<MatrixTransform*>(node)->setMatrix(matrixTransform * Matrix::translate(0,0,-0.3));
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

