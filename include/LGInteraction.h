/* Fichier : LGInteraction.h
 * Description : Header de la classe LGINteraction
 * Cette classe gere l'ensemble des interaction entre les etiquettes et l'utilisateur une fois que le programme est lance :
 * - ctrl+ clic gauche : selection d'une etiquette. Les etiquettes deja selectionnees le restent.
 * - 'a' : selectionner toutes les etiquettes
 * - 'q' ou 'Q' ou Gauche : augmenter la valeur X des etiquettes
 * - 'd' ou 'D' ou Droite : diminuer la valeur de X des etiquettes
 * - 'z' ou 'Z' ou Haut : augmenter la valeur de Y des etiquettes
 * - 's' ou 'S' ou Bas : diminuer la valeur de Y des etiquettes
 * - '+' : augmenter la taille des etiquettes
 * - '-' : diminuer la taille des etiquettes
 * - 'h' ou 'H' : cacher les etiquettes
 * - 'l' ou 'L' : savoir a quels drawables les etiquettes sont rattachees
 * - 'm' ou 'M' : ne plus savoir quels drawables sont rattaches aux etiquettes
 * - 'i' ou 'I' : afficher l'etiquette d'informations supplementaires, si elle existe
 * Pour l'utiliser, il faut creer une instance et l'ajouter en tant que EventHandler a l'objet Viewer du programme principal :
 *      ref_ptr<LGInteraction> interaction = new LGInteraction(listLabels);
 *      viewer.addEventHandler(interaction.get());
 * Avec listLabels un vecteurs de pointeurs vers les etiquettes.
 * Auteur : Thomas Brunel
 */

#ifndef LGInteraction_H_INCLUDED
#define LGInteraction_H_INCLUDED

#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgUtil/LineSegmentIntersector>
#include <osgText/Text>
#include "lgLabel.h"

class LGInteraction : public osgGA::GUIEventHandler
{
    public:
        /* Fonction qui s'active automatiquement lorsqu'un evenement est detecte
         * Arguments :
         * - ea : osgGA::GUIEventAdapter
         * - aa : osgGA::GUIActionAdapter
         * Retour : renvoie toujours false. La valeur de retour indique si l'evenement a ete traite.
         * Si true est renvoye, OSG considere que l'evenement n'est plus necessaire a aucun handler et sera ignore par les autres handlers, dont le "camera manipulator"
         * (cf. Beginner's guide p. 237)
         */
        virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

        /* Constructeur
         * Argument :
         * - l : vector<osgText::Text> : la liste des etiquettes creees dans le programme principal.
         */
        LGInteraction(std::vector<osgText::Text*> l);

    protected:

        /* Liste des etiquettes selectionnees a un instant donne
         */
        std::vector<osgText::Text*> selectedLabels;

        /* Liste des etiquettes creees dans le programme principal
         */
        std::vector<osgText::Text*> listLabels;

        /* Label qui est en dessous de la souris
         */
        osg::ref_ptr<lgLabel> mouseLabel;
};

#endif // LGInteraction_H_INCLUDED
