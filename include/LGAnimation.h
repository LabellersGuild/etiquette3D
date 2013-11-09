/* Fichier : LGAnimation.h
 * Description : Header de la classe LGAnimation
 *      Cette classe facilite le placement dynamique des �tiquettes
 *      Pour l'utiliser, il faut d'abord cr�er une sous classe qui red�finit la m�thode operator().
 *      Dans cette m�thode, on peut utiliser les autres m�thodes de la classe, et il faut finir par la ligne "traverse(node, nv);" qui permet � OSG de continuer la travers�e de l'arbre.
 *      Ensuite, il faut cr�er une instance de la sous classe et la rattacher � la matrice de transformation d'une �tiquette :
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

        /* Fonction qui g�re le d�placement dynamique des �tiquettes
         * Arguments :
         * - node : osg::Node* : le noeuds rattach� � l'objet LGAnimation : normalement, la matrice de transformation reli�e � l'�tiquette
         * - nv : osg::NodeVisitor
         */
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;

        /* Fonction qui permet de d�placer l'�tiquette.
         * Arguments :
         * - x : d�placement selon l'axe X
         * - y : d�placement selon l'axe Y
         * - z : d�placement selon l'axe Z
         */
        void translateLabel(osg::Node* node, int x, int y, int z);

        /* Pr�cise si l'�tiquette est ses environs sont cach�s par d'autres drawables
         * Pour savoir si l'�tiquette est cach�e, ne pas pr�ciser les valeurs des 4 derniers arguments (0 par d�faut)
         * Arguments :
         * - node : osg::Node* : m�me argument que operator()
         * - nv : osg::NodeVisitor* : m�me argument que operator()
         * - int Xminimum : 0 par d�faut. Augmente la fen�tre de v�rification selon l'axe X � de la fen�tre
         * - int Yminimum : 0 par d�faut. Augmente la fen�tre de v�rification selon l'axe Y de la fen�tre
         * - int Xmaximum : 0 par d�faut. Augmente la fen�tre de v�rification selon l'axe X � de la fen�tre
         * - int Ymaximum : 0 par d�faut. Augmente la fen�tre de v�rification selon l'axe Y de la fen�tre
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
