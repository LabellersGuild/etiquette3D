/* Fichier : LGAnimation.h
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

#ifndef LGAnimation_H_INCLUDED
#define LGAnimation_H_INCLUDED

#include <iostream>
#include <osg/NodeCallback>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgUtil/LineSegmentIntersector>
#include <osgText/TextBase>
#include <osgText/Text>

class LGAnimation : public osg::NodeCallback
{
    public:
        /* Constructeur
         * Arguments : viewer : osgViewer::Viewer* : l'objet viewer du programme principal
         */
        LGAnimation(osgViewer::Viewer* viewer) : view(viewer) {};

        /* Fonction qui gere le deplacement dynamique des etiquettes
         * Arguments :
         * - node : osg::Node* : le noeuds rattache a l'objet LGAnimation : normalement, la matrice de transformation reliee a l'etiquette
         * - nv : osg::NodeVisitor
         */
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;

        /* Precise si l'etiquette est ses environs sont caches par d'autres drawables
         * Pour savoir si l'etiquette est cachee, ne pas preciser les valeurs des 4 derniers arguments (0 par defaut)
         * Arguments :
         * - node : osg::Node* : meme argument que operator()
         * - nv : osg::NodeVisitor* : meme argument que operator()
         * - int Xminimum : 0 par defaut. Augmente la fenetre de verification selon l'axe X de la fenetre
         * - int Yminimum : 0 par defaut. Augmente la fenetre de verification selon l'axe Y de la fenetre
         * - int Xmaximum : 0 par defaut. Augmente la fenetre de verification selon l'axe X de la fenetre
         * - int Ymaximum : 0 par defaut. Augmente la fenetre de verification selon l'axe Y de la fenetre
         * La fonction renvoie NULL si aucun élément n'est devant l'étiquette, et la position de l'intersection entre le faisceau camera-label et le drawable.
         */
        osgUtil::PolytopeIntersector::Intersection* intersectCameraLabel(osg::Node* node, osg::NodeVisitor* nv, int Xminimum=0, int Yminimum=0, int Xmaximum=0, int Ymaximum=0);

    protected :

        /* L'objet viewer du programme principal
        */
        osg::ref_ptr<osgViewer::Viewer> view;

        /* Objet qui enregistre l'intersection entre le faisceau de la caméra à l'étiquette. Renvoyé par intersectCameraLabel
         */
        osgUtil::PolytopeIntersector::Intersection intersection;
};

#endif // LGAnimation_H_INCLUDED
