/* Fichier : myLGAnimation.h
 * Description : Fichier cr�ant une sous classe de LGAnimation, pour red�finir la m�thode operator()
 * Auteur : Thomas Brunel
 */

#ifndef MYLGANIMATION_H_INCLUDED
#define MYLGANIMATION_H_INCLUDED

#include "LGAnimation.h"

class myLGAnimation : public LGAnimation
{
    public :

    /* Constructeur : appel a celui de LGAnimation
     */
     myLGAnimation(osgViewer::Viewer* viewer) : LGAnimation(viewer) {};

     /* Redefinition de la methode operator()
      */
     virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;
};
#endif // MYLGANIMATION_H_INCLUDED
