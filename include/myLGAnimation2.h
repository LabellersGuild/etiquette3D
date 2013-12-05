/* Fichier : myLGAnimation2.h
 * Description : Fichier créant une sous classe de LGAnimation, pour redéfinir la méthode operator().
 * Autre méthode de placement des étiquettes
 * Auteur : Thomas Brunel
 */


#ifndef MYLGANIMATION2_H
#define MYLGANIMATION2_H

#include "LGAnimation.h"


class myLGAnimation2 : public LGAnimation
{
    public:
        /* Constructeur : appel a celui de LGAnimation
         */
         myLGAnimation2(osgViewer::Viewer* viewer) : LGAnimation(viewer) {};

         /* Redefinition de la methode operator()
          */
         virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) ;
};

#endif // MYLGANIMATION2_H
