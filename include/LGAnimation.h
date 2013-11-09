/* Fichier : LGAnimation.h
 * Description : Header de la classe LGAnimation
 *      Cette classe facilite le placement dynamique des étiquettes
 *      Pour l'utiliser, il faut d'abord créer une sous classe qui redéfinit la méthode operator().
 *      Dans cette méthode, on peut utiliser les autres méthodes de la classe, et il faut finir par la ligne "traverse(node, nv);" qui permet à OSG de continuer la traversée de l'arbre.
 *      Ensuite, il faut créer une instance de la sous classe et la rattacher à la matrice de transformation d'une étiquette :
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

        /* Fonction qui gère le déplacement dynamique des étiquettes
         * Arguments :
         * - node : osg::Node* : le noeuds rattaché à l'objet LGAnimation : normalement, la matrice de transformation reliée à l'étiquette
         * - nv : osg::NodeVisitor
         */
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;

        /* Fonction qui permet de déplacer l'étiquette.
         * Arguments :
         * - x : déplacement selon l'axe X
         * - y : déplacement selon l'axe Y
         * - z : déplacement selon l'axe Z
         */
        void translateLabel(osg::Node* node, int x, int y, int z);

        /* Précise si l'étiquette est ses environs sont cachés par d'autres drawables
         * Pour savoir si l'étiquette est cachée, ne pas préciser les valeurs des 4 derniers arguments (0 par défaut)
         * Arguments :
         * - node : osg::Node* : même argument que operator()
         * - nv : osg::NodeVisitor* : même argument que operator()
         * - int Xminimum : 0 par défaut. Augmente la fenêtre de vérification selon l'axe X à de la fenêtre
         * - int Yminimum : 0 par défaut. Augmente la fenêtre de vérification selon l'axe Y de la fenêtre
         * - int Xmaximum : 0 par défaut. Augmente la fenêtre de vérification selon l'axe X à de la fenêtre
         * - int Ymaximum : 0 par défaut. Augmente la fenêtre de vérification selon l'axe Y de la fenêtre
         */
        bool isFree(osg::Node* node, osg::NodeVisitor* nv, int Xminimum, int Yminimum, int Xmaximum, int Ymaximum);

    protected :

        /* L'objet viewer du programme principal
        */
        osg::ref_ptr<osgViewer::Viewer> view;

    private :

        /* Maximum de 6 entiers
         * Arguments :
         * - a : int
         * - b : int
         * - c : int
         * - d : int
         * - e : int
         * - f : int
         */
        int myMax(int a, int b, int c, int d, int e, int f);

        /* Minimum de 6 entiers
         * Arguments :
         * - a : int
         * - b : int
         * - c : int
         * - d : int
         * - e : int
         * - f : int
         */
        int myMin(int a, int b, int c, int d, int e, int f);
};

#endif // LGAnimation_H_INCLUDED
