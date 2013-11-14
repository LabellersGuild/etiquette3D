/* Fichier : LGInteraction.h
 * Description : Header de la classe LGINteraction
 * Cette classe gère l'ensemble des interaction entre les étiquettes et l'utilisateur une fois que le programme est lancé :
 * - ctrl+ clic gauche : sélection d'une étiquette. Les étiquettes déjà sélectionnées le restent.
 * - 'a' : sélectionner toutes les étiquettes
 * - 'q' ou 'Q' ou Gauche : augmenter la valeur X des étiquettes
 * - 'd' ou 'D' ou Droite : diminuer la valeur de X des étiquettes
 * - 'z' ou 'Z' ou Haut : augmenter la valeur de Y des étiquettes
 * - 's' ou 'S' ou Bas : diminuer la valeur de Y des étiquettes
 * - '+' : augmenter la taille des étiquettes
 * - '-' : diminuer la taille des étiquettes
 * - 'h' ou 'H' : cacher les étiquettes
 * - 'l' ou 'L' : savoir à quelles drawables les étiquettes sont rattachées
 * - 'm' ou 'M' : ne plus savoir quelles drawables rattachés aux étiquettes
 * - 'i' ou 'I' : afficher l'étiquette d'informations supplémentaires, si elle existe
 * Pour l'utiliser, il faut créer une instance et l'ajouter en tant que EventHandler à l'objet Viewer du programme principal :
 *      ref_ptr<LGInteraction> interaction = new LGInteraction(listLabels);
 *      viewer.addEventHandler(interaction.get());
 * Avec listLabels un vecteurs de pointeurs vers les étiquettes.
 * Auteur : Thomas Brunel
 */

#ifndef LGInteraction_H_INCLUDED
#define LGInteraction_H_INCLUDED

#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgUtil/LineSegmentIntersector>
#include <osgText/Text>

class LGInteraction : public osgGA::GUIEventHandler
{
    public:
        /* Fonction qui s'active automatiquement lorsqu'un évenement est détecté
         * Arguments :
         * - ea : osgGA::GUIEventAdapter
         * - aa : osgGA::GUIActionAdapter
         * Retour : renvoie toujours false. La valeur de retour indique si l'évenement a été traité.
         * Si true est renvoé, OSG considère que l'évenement n'est plus nécessaire à aucun handler et sera ignoré par les autres handlers, dont le "camera manipulator"
         * (cf. Beginner's guide p. 237)
         */
        virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

        /* Constructeur
         * Argument :
         * - l : vector<osgText::Text> : la liste des étiquettes créées dans le programme principal.
         */
        LGInteraction(std::vector<osgText::Text*> l);

    protected:

        /* Liste des étiquettes sélectionnées à un instant donné
         */
        std::vector<osgText::Text*> selectedLabels;

        /* Liste des étiquettes créées dans le programme principal
         */
        std::vector<osgText::Text*> listLabels;
};

#endif // LGInteraction_H_INCLUDED
