/* Fichier : LGAnimation.cpp
 * Description : Header de la classe LGAnimation
 *      Cette classe facilite le placement dynamique des etiquettes
 *      Pour l'utiliser, il faut d'abord creer une sous classe qui redefinit la methode operator().
 *      Dans cette methode, on peut utiliser les autres methodes de la classe, et il faut
 *      commencer par la ligne " LGAnimation::operator()(node, nv);"
 *      et finir par la ligne "traverse(node, nv);" qui permet a OSG de continuer la traversee de l'arbre.
 *      Ensuite, il faut creer une instance de la sous classe et la rattacher a la matrice de transformation d'une etiquette :
 *          matriceDeTransformation->setUpdateCallback( new LGAnimationSousClasse(&viewer));
 * Auteur : Thomas Brunel
 */

 #include "../include/LGAnimation.h"
#include "../include/lgLabel.h"

using namespace osg;

/* Fonction qui gere le deplacement dynamique des etiquettes
  * Arguments :
  * - node : Node* : le noeuds rattache a l'objet LGAnimation : normalement, la matrice de transformation reliee a l'etiquette
  * - nv : NodeVisitor
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
 * - node : Node* : meme argument que operator()
 * - nv : NodeVisitor* : meme argument que operator()
 * - int Xminimum : 0 par defaut. Augmente la fenetre de verification selon l'axe X de la fenetre
 * - int Yminimum : 0 par defaut. Augmente la fenetre de verification selon l'axe Y de la fenetre
 * - int Xmaximum : 0 par defaut. Augmente la fenetre de verification selon l'axe X de la fenetre
 * - int Ymaximum : 0 par defaut. Augmente la fenetre de verification selon l'axe Y de la fenetre
 * La fonction renvoie NULL si aucun élément n'est devant l'étiquette, et la position de l'intersection entre le faisceau camera-label et le drawable.
*/
osgUtil::PolytopeIntersector::Intersection* LGAnimation::intersectCameraLabel(Node* node, NodeVisitor* nv, int Xminimum, int Yminimum, int Xmaximum, int Ymaximum)
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
        intersection = *(intersector->getIntersections().begin());
        if (intersection.drawable->getParent(0)->getParent(0) != node)
        {
            return &intersection;
        }
        return NULL;
    }
    return NULL;
}
