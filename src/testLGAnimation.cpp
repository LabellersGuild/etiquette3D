/** File : testLGAnimation.cpp
 * Description : Class used to test the LGAnimation functions.
 * Author : Thomas
 */

#include "../include/testLGAnimation.h"
#include "../include/lgLabel.h"

using namespace osg;
using namespace std;

/** Override of operator()
 */
void testLGAnimation::operator()(Node* node, NodeVisitor* nv)
{
    LGAnimation::operator()(node, nv);

     //Label
    ref_ptr<lgLabel> label = dynamic_cast<lgLabel*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(0))->getDrawable(0));

    //Intersection
    osgUtil::PolytopeIntersector::Intersection* intersection = intersectCameraLabel(node, nv);

    if (intersection != NULL)
    {
        cout << "Hidden :"  << endl << "object hiding : " ;
        for (unsigned i=0;i<intersection->nodePath.size();i++)
        {
            cout << "--" << (intersection->nodePath[i])->getName();
        }
        cout << "--" << intersection->drawable->getName() << endl;
        cout << "Label hidden : " << label->getText().createUTF8EncodedString() << endl;

        //Local position of the interesection
        Vec4 intersectionLocalPosition = Vec4(intersection->localIntersectionPoint.x(), intersection->localIntersectionPoint.y(), intersection->localIntersectionPoint.z(), 1);

        //World position of the intersection
        MatrixList matricesList = intersection->drawable->getParent(0)->getWorldMatrices();
        for (unsigned i=0;i<matricesList.size();i++)
        {
            intersectionLocalPosition = intersectionLocalPosition*matricesList[i];
        }
        Vec3 intersectionWorldPosition = Vec3(intersectionLocalPosition.x(),intersectionLocalPosition.y(),intersectionLocalPosition.z());

        //Projection of the intersection :
        Matrix modelView = view->getCamera()->getViewMatrix();
        Matrix projection = view->getCamera()->getProjectionMatrix();
        Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
        Matrix MVPW = modelView * projection * window;
        Vec3 intersection2DPosition =  intersectionWorldPosition* MVPW;
        cout << "Position of the intersection  : (" << intersection2DPosition.x() << " , " << intersection2DPosition.y() << " )" << endl;

         Vec4 bbox = label->compute2dBox(view);
         cout << "Bounding box of the label : xMin = " << bbox[0] << " , xMax = " << bbox[2] << " ,yMin = " << bbox[1] << " ,yMax = " << bbox[3] << endl;
    }

    //Allow OSG to continue the node traversal
    traverse(node, nv);
}
