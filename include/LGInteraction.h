/* Fichier : LGInteraction.h
 * Description : Header de la classe LGINteraction
 * Cette classe g�re l'ensemble des interaction entre les �tiquettes et l'utilisateur une fois que le programme est lanc� :
 * - ctrl+ clic gauche : s�lection d'une �tiquette. Les �tiquettes d�j� s�lectionn�es le restent.
 * - 'a' : s�lectionner toutes les �tiquettes
 * - 'q' ou 'Q' ou Gauche : augmenter la valeur X des �tiquettes
 * - 'd' ou 'D' ou Droite : diminuer la valeur de X des �tiquettes
 * - 'z' ou 'Z' ou Haut : augmenter la valeur de Y des �tiquettes
 * - 's' ou 'S' ou Bas : diminuer la valeur de Y des �tiquettes
 * - '+' : augmenter la taille des �tiquettes
 * - '-' : diminuer la taille des �tiquettes
 * - 'h' ou 'H' : cacher les �tiquettes
 * - 'l' ou 'L' : savoir � quelles drawables les �tiquettes sont rattach�es
 * - 'm' ou 'M' : ne plus savoir quelles drawables rattach�s aux �tiquettes
 * - 'i' ou 'I' : afficher l'�tiquette d'informations suppl�mentaires, si elle existe
 * Pour l'utiliser, il faut cr�er une instance et l'ajouter en tant que EventHandler � l'objet Viewer du programme principal :
 *      ref_ptr<LGInteraction> interaction = new LGInteraction(listLabels);
 *      viewer.addEventHandler(interaction.get());
 * Avec listLabels un vecteurs de pointeurs vers les �tiquettes.
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
        /* Fonction qui s'active automatiquement lorsqu'un �venement est d�tect�
         * Arguments :
         * - ea : osgGA::GUIEventAdapter
         * - aa : osgGA::GUIActionAdapter
         * Retour : renvoie toujours false. La valeur de retour indique si l'�venement a �t� trait�.
         * Si true est renvo�, OSG consid�re que l'�venement n'est plus n�cessaire � aucun handler et sera ignor� par les autres handlers, dont le "camera manipulator"
         * (cf. Beginner's guide p. 237)
         */
        virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

        /* Constructeur
         * Argument :
         * - l : vector<osgText::Text> : la liste des �tiquettes cr��es dans le programme principal.
         */
        LGInteraction(std::vector<osgText::Text*> l);

    protected:

        /* Liste des �tiquettes s�lectionn�es � un instant donn�
         */
        std::vector<osgText::Text*> selectedLabels;

        /* Liste des �tiquettes cr��es dans le programme principal
         */
        std::vector<osgText::Text*> listLabels;
};

#endif // LGInteraction_H_INCLUDED
