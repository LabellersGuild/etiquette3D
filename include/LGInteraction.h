/** File : LGInteraction.h
 * Description : header of the LGInteraction class
 * This class deals with every interaction between the user and the labels when the program is running.
 * - ctrl+ left click : select a label. Already selected labels remain seleced.
 * - 'a' : select every label
 * - 'q' or 'Q' ou Left : increase X value of the selected labels
 * - 'd' or'D' ou Right : decrease X value of the selected labels
 * - 'z' or 'Z' ou Up : increase Y value of the selected labels
 * - 's' or 'S' ou Down : decrease Z value of the selected labels
 * - '+' : inclrease the size of the selected labels
 * - '-' : declrease the size of the selected labels
 * - 'h' or 'H' : hide the selected labels
 * - 'l' or 'L' : see in dark the objects linked to the selected labels
 * - 'm' or 'M' : set the objects linked to the selected labels back to normal
 * - 'i' or 'I' : see the information label, if exists.
 * TO use this class, create an instance and add it as an EventHandler to the viewer object of the main file :
 * ref_ptr<LGInteraction> interaction = new LGInteraction(listLabels);
 * viewer.addEventHandler(interaction.get());
 * With listLabels a vector of pointers on the labels.
 * Author : Thomas Brunel
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
       /** Constructor
        * @param l : vector<lgLabel*> : list of lgLabels created.
        */
        LGInteraction(std::vector<lgLabel*> l);

       /** Automatically fires when an event is detected.
        * @param ea : osgGA::GUIEventAdapter
        * @param aa : osgGA::GUIActionAdapter
        * @return false : the return value tells if the event is treated.
        * If true is returned, OSG consider that the event is no longer needed to other handlers, and they will ignore it. The camera manipulator is on of these other handlers.
        */
        virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

    protected:

        /** List of the currently selected labels
         */
        std::vector<lgLabel*> selectedLabels;

        /** List of the labels created in the main file
         */
        std::vector<lgLabel*> listLabels;

        /** Label under the mouse
         */
        osg::ref_ptr<lgLabel> mouseLabel;
};

#endif // LGInteraction_H_INCLUDED
