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

    //2D bBox :
    Vec4 bbox2d = label->compute2dBox(view);

    //Save the current matrixTransform
    Matrix matrixTransform= dynamic_cast<MatrixTransform*>(node)->getMatrix();

    //Find the intersection between the camera and the node :
    ref_ptr<osgUtil::PolytopeIntersector> intersector = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, bbox2d[0]-Xminimum, bbox2d[1]-Yminimum, bbox2d[2]-Xmaximum, bbox2d[3]-Ymaximum);
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
