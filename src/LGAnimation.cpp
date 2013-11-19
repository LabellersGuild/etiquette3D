/* Fichier : LGAnimation.h
 * Description : Header de la classe LGAnimation
 *      Cette classe facilite le placement dynamique des étiquettes
 *      Pour l'utiliser, il faut d'abord créer une sous classe qui redéfinit la méthode operator().
 *      Dans cette méthode, on peut utiliser les autres méthodes de la classe, et il faut finir par la ligne "traverse(node, nv);" qui permet à OSG de continuer la traversée de l'arbre.
 *      Ensuite, il faut créer une instance de la sous classe et la rattacher à la matrice de transformation d'une étiquette :
 *          matriceDeTransformation->setUpdateCallback( new LGAnimationSousClasse(&viewer));
 * Auteur : Thomas Brunel
 */

 #include "../include/LGAnimation.h"

using namespace osg;

/* Fonction qui gère le déplacement dynamique des étiquettes
  * Arguments :
  * - node : osg::Node* : le noeuds rattaché à l'objet LGAnimation : normalement, la matrice de transformation reliée à l'étiquette
  * - nv : osg::NodeVisitor
  */
void LGAnimation::operator()(Node* node, NodeVisitor* nv)
{
}

/* Précise si l'étiquette et ses environs sont cachés par d'autres drawables
 * Pour savoir si l'étiquette est cachée, ne pas préciser les valeurs des 4 derniers arguments (0 par défaut)
 * Arguments :
 * - node : osg::Node* : même argument que operator()
 * - nv : osg::NodeVisitor* : même argument que operator()
 * - int Xminimum : 0 par défaut. Augmente la fenêtre de vérification selon l'axe X à de la fenêtre
 * - int Yminimum : 0 par défaut. Augmente la fenêtre de vérification selon l'axe Y de la fenêtre
 * - int Xmaximum : 0 par défaut. Augmente la fenêtre de vérification selon l'axe X à de la fenêtre
 * - int Ymaximum : 0 par défaut. Augmente la fenêtre de vérification selon l'axe Y de la fenêtre
*/
bool LGAnimation::isFree(Node* node, NodeVisitor* nv, int Xminimum, int Yminimum, int Xmaximum, int Ymaximum)
{
     // Find the world coordinates of the node :
    Matrix worldMatrix = node->getWorldMatrices()[0];
    Vec3 center = Vec3(worldMatrix(3,0), worldMatrix(3,1), worldMatrix(3,2));

     /// TO DO : Problems with the bounds of the polytope
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
    ref_ptr<osgUtil::PolytopeIntersector> intersector = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, minX-Xminimum, minY-Yminimum, maxX-Xmaximum, maxY-Ymaximum);
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

/**
 * Function to move the label
 * @param node, Node* : the matrix transformation node of the label
 * @param x, int : X axis translation
 * @param y, int : y axis translation
 * @param z, int : z axis translation
*/
void LGAnimation::translateLabel(Node* node, int x, int y, int z)
{
     osg::Matrix matrixTransform= dynamic_cast<osg::MatrixTransform*>(node)->getMatrix();
    dynamic_cast<osg::MatrixTransform*>(node)->setMatrix(matrixTransform * osg::Matrix::translate(x,y,z));
}
