/* Fichier : LGAnimation.h
 * Description : Header de la classe LGAnimation
 *      Cette classe facilite le placement dynamique des etiquettes
 *      Pour l'utiliser, il faut d'abord creer une sous classe qui redefinit la methode operator().
 *      Dans cette methode, on peut utiliser les autres methodes de la classe, et il faut finir par la ligne "traverse(node, nv);" qui permet a OSG de continuer la traversee de l'arbre.
 *      Ensuite, il faut creer une instance de la sous classe et la rattacher a la matrice de transformation d'une etiquette :
 *          matriceDeTransformation->setUpdateCallback( new LGAnimationSousClasse(&viewer));
 * Auteur : Thomas Brunel
 */

 #include "../include/LGAnimation.h"
#include "../include/lgLabel.h"

using namespace osg;

/* Fonction qui gere le deplacement dynamique des etiquettes
  * Arguments :
  * - node : osg::Node* : le noeuds rattache a l'objet LGAnimation : normalement, la matrice de transformation reliee a l'etiquette
  * - nv : osg::NodeVisitor
  */
void LGAnimation::operator()(Node* node, NodeVisitor* nv)
{
    ref_ptr<lgLabel> label = dynamic_cast<lgLabel*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(0))->getDrawable(0));

    int hidingDistance = label->getHidingDistance();

    if ( hidingDistance != -1 && hidingDistance < label->distanceCamera(view))
    {
        label->setDrawMode(0);
    }
    else
    {
        label->setDrawMode(osgText::Text::TEXT | osgText::Text::ALIGNMENT);
    }
}

/* Fonction qui permet de deplacer l'etiquette.
 * Arguments :
 * - x : deplacement selon l'axe X
 * - y : deplacement selon l'axe Y
 * - z : deplacement selon l'axe Z
*/

/* Precise si l'etiquette est ses environs sont caches par d'autres drawables
 * Pour savoir si l'etiquette est cachee, ne pas preciser les valeurs des 4 derniers arguments (0 par defaut)
 * Arguments :
 * - node : osg::Node* : meme argument que operator()
 * - nv : osg::NodeVisitor* : meme argument que operator()
 * - int Xminimum : 0 par defaut. Augmente la fenetre de verification selon l'axe X de la fenetre
 * - int Yminimum : 0 par defaut. Augmente la fenetre de verification selon l'axe Y de la fenetre
 * - int Xmaximum : 0 par defaut. Augmente la fenetre de verification selon l'axe X de la fenetre
 * - int Ymaximum : 0 par defaut. Augmente la fenetre de verification selon l'axe Y de la fenetre
*/
bool LGAnimation::isFree(Node* node, NodeVisitor* nv, int Xminimum, int Yminimum, int Xmaximum, int Ymaximum)
{
    //Label
    ref_ptr<lgLabel> label = dynamic_cast<lgLabel*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(0))->getDrawable(0));

     // Find the world coordinates of the node :
    Vec3 center = label->getAbsolutePosition();

     /// TO DO : Problems with the bounds of the polytope
    //Bounding box of the label :
    BoundingBox bbox = label->computeBound();

    //Matrix to change world coordinates in windows coordinates
    Matrix modelView = view->getCamera()->getViewMatrix();
    Matrix projection = view->getCamera()->getProjectionMatrix();
    Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
    Matrix MVPW = modelView * projection * window;

    //Projection of the bounding box of the label :
    Vec3 screenxyz, screenXyz, screenxYz, screenxyZ, screenXYz, screenXyZ, screenxYZ, screenXYZ;

    //If info label :
    if (dynamic_cast<Group*>(node)->getNumChildren() > 1)
    {
        if (dynamic_cast<osgText::Text*>(dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(1))->getDrawable(0))->getDrawMode() != 0)
        {
            BoundingBox bboxInfo = dynamic_cast<Geode*>(dynamic_cast<MatrixTransform*>(node)->getChild(1))->getDrawable(0)->getBound();
            int bboxXMin = (bbox.xMin() < bboxInfo.xMin() ? bbox.xMin() : bboxInfo.xMin());
            int bboxXMax = (bbox.xMax() > bboxInfo.xMax() ? bbox.xMax() : bboxInfo.xMax());
            int bboxYMin = (bbox.yMin() < bboxInfo.yMin() ? bbox.yMin() : bboxInfo.yMin());
            int bboxYMax = (bbox.yMax() > bboxInfo.yMax() ? bbox.yMax() : bboxInfo.yMax());
            int bboxZMin = (bbox.zMin() < bboxInfo.zMin() ? bbox.zMin() : bboxInfo.zMin());
            int bboxZMax = (bbox.zMax() > bboxInfo.zMax() ? bbox.zMax() : bboxInfo.zMax());

            screenxyz = (center + Vec3(bboxXMin,bboxYMin,bboxZMin)) * MVPW;
            screenXyz = (center + Vec3(bboxXMax,bboxYMin,bboxZMin)) * MVPW;
            screenxYz = (center + Vec3(bboxXMin,bboxYMax,bboxZMin)) * MVPW;
            screenxyZ = (center + Vec3(bboxXMin,bboxYMin,bboxZMax)) * MVPW;
            screenXYz = (center + Vec3(bboxXMax,bboxYMax,bboxZMin)) * MVPW;
            screenXyZ = (center + Vec3(bboxXMax,bboxYMin,bboxZMax)) * MVPW;
            screenxYZ = (center + Vec3(bboxXMin,bboxYMax,bboxZMax)) * MVPW;
            screenXYZ = (center + Vec3(bboxXMax,bboxYMax,bboxZMax)) * MVPW;
        }
    }
    // If no info label :
    else
    {
         //Projection of the bounding box of the label :
        screenxyz = (center + Vec3(bbox.xMin(),bbox.yMin(),bbox.zMin())) * MVPW;
        screenXyz = (center + Vec3(bbox.xMax(),bbox.yMin(),bbox.zMin())) * MVPW;
        screenxYz = (center + Vec3(bbox.xMin(),bbox.yMax(),bbox.zMin())) * MVPW;
        screenxyZ = (center + Vec3(bbox.xMin(),bbox.yMin(),bbox.zMax())) * MVPW;
        screenXYz = (center + Vec3(bbox.xMax(),bbox.yMax(),bbox.zMin())) * MVPW;
        screenXyZ = (center + Vec3(bbox.xMax(),bbox.yMin(),bbox.zMax())) * MVPW;
        screenxYZ = (center + Vec3(bbox.xMin(),bbox.yMax(),bbox.zMax())) * MVPW;
        screenXYZ = (center + Vec3(bbox.xMax(),bbox.yMax(),bbox.zMax())) * MVPW;
    }

    //std::cout << screenxyz[0] << " " << screenxyz[1] << " " << screenXyz[0] << " " << screenXyz[1] << " " << screenxYz[0] << " " << screenxYz[1];
    //std::cout << " " << screenxyZ[0] << " " << screenxyZ[1] << " " << screenXYz[0] << " " << screenXYz[1] << " " << screenXyZ[0] << " " << screenXyZ[1];
    //std::cout << " " << screenxYZ[0] << " " << screenxyZ[1] << " " << screenXYZ[0] << " " << screenXYZ[1] << std::endl;

    // The bounds of the polytope are determined by the two projected points the most on the left and on the right on the screen.
    /*
    int maxX = myMax(screenxMin[0], screenxMax[0], screenyMin[0], screenyMax[0], screenzMin[0], screenzMax[0]);
    int minX = myMin(screenxMin[0], screenxMax[0], screenyMin[0], screenyMax[0], screenzMin[0], screenzMax[0]);
    int maxY = myMax(screenxMin[1], screenxMax[1], screenyMin[1], screenyMax[1], screenzMin[1], screenzMax[1]);
    int minY = myMin(screenxMin[1], screenxMax[1], screenyMin[1], screenyMax[1], screenzMin[1], screenzMax[1]);
    */

    Vec3 mostLeft = screenxyz[0] < screenXyz[0] ? screenxyz : screenXyz;
    mostLeft = mostLeft[0] < screenxYz[0] ? mostLeft : screenxYz;
    mostLeft = mostLeft[0] < screenxyZ[0] ? mostLeft : screenxyZ;
    mostLeft = mostLeft[0] < screenXYz[0] ? mostLeft : screenXYz;
    mostLeft = mostLeft[0] < screenXyZ[0] ? mostLeft : screenXyZ;
    mostLeft = mostLeft[0] < screenxYZ[0] ? mostLeft : screenxYZ;
    mostLeft = mostLeft[0] < screenXYZ[0] ? mostLeft : screenXYZ;

    Vec3 mostRight = screenxyz[0] > screenXyz[0] ? screenxyz : screenXyz;
    mostRight = mostRight[0] > screenxYz[0] ? mostRight : screenxYz;
    mostRight = mostRight[0] > screenxyZ[0] ? mostRight : screenxyZ;
    mostRight = mostRight[0] > screenXYz[0] ? mostRight : screenXYz;
    mostRight = mostRight[0] > screenXyZ[0] ? mostRight : screenXyZ;
    mostRight = mostRight[0] > screenxYZ[0] ? mostRight : screenxYZ;
    mostRight = mostRight[0] > screenXYZ[0] ? mostRight : screenXYZ;

    std::cout << "xMostLeft = " << mostLeft[0] << " yMostLeft = " << mostLeft[1] << " xMostRight = " << mostRight[0] << " yMostRight = " << mostRight[1] << std::endl;


    //Save the current matrixTransform
    Matrix matrixTransform= dynamic_cast<MatrixTransform*>(node)->getMatrix();

    //Find the intersection between the camera and the node :
    ref_ptr<osgUtil::PolytopeIntersector> intersector = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, mostLeft[0]-Xminimum, (mostLeft[1] < mostRight[1] ? mostLeft[1] : mostRight[1])-Yminimum, mostRight[0]-Xmaximum, (mostLeft[1] > mostRight[1] ? mostLeft[1] : mostRight[1])-Ymaximum);
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
            return false;
        }
    }
    return true;
}

 /* Maximum de 6 entiers
  * Arguments :
  * - a : int
  * - b : int
  * - c : int
  * - d : int
  * - e : int
  * - f : int
  */
int LGAnimation::myMax(int a, int b, int c, int d, int e, int f){
    int max1 = a > b ? a : b;
    int max2 = c > d ? c : d;
    int max3 = e > f ?  e : f;
    int result = max1 > max2 ? max1 : max2;
    result = result > max3 ? result : max3;
    return result;
}

 /* Minimum de 6 entiers
  * Arguments :
  * - a : int
  * - b : int
  * - c : int
  * - d : int
  * - e : int
  * - f : int
  */
int LGAnimation::myMin(int a, int b, int c, int d, int e, int f){
    int min1 = a < b ? a : b;
    int min2 = c < d ? c : d;
    int min3 = e < f ?  e : f;
    int result = min1 < min2 ? min1 : min2;
    result = result < min3 ? result : min3;
    return result;
}
