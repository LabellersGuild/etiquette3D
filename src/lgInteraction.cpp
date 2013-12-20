/** File : LGInteraction.cpp
 * Description : source file of the LGInteraction class
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

 #include "../include/lgInteraction.h"

using namespace osg;
using namespace std;

/** Constructor
 * @param l : vector<lgLabel*> : list of lgLabels created.
 */
lgInteraction::lgInteraction(vector<lgLabel*> l) : listLabels(l) {};


  /** Automatically fires when an event is detected.
   * @param ea : osgGA::GUIEventAdapter
   * @param aa : osgGA::GUIActionAdapter
   * @return false : the return value tells if the event is treated.
   * If true is returned, OSG consider that the event is no longer needed to other handlers, and they will ignore it. The camera manipulator is on of these other handlers.
   */
bool lgInteraction::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    // Ctrl + left click : label picking
    if (ea.getEventType()==osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL )
    {
        //viewer
        ref_ptr<osgViewer::Viewer> viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

        if ( viewer )
        {
            // To know what the user clicked on:
            ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
            osgUtil::IntersectionVisitor iv( intersector.get() );
            iv.setTraversalMask( ~0x1 );

            viewer->getCamera()->accept( iv );
            //If there is an object
            if ( intersector->containsIntersections() )
            {
                osgUtil::LineSegmentIntersector::Intersection result = *(intersector->getIntersections().begin());

                // If it is a label :
                if (result.drawable->isSameKindAs(new lgLabel()))
                {
                    //Add the label to the list of selected labels
                    selectedLabels.push_back(dynamic_cast<lgLabel*>(result.drawable.get()));

                    // Disable depth testing so geometry is draw regardless of depth values of geometry.
                    ref_ptr<StateSet> stateSet = selectedLabels.at(selectedLabels.size()-1)->getUpdatedMatrix()->getOrCreateStateSet();
                    stateSet->setMode(GL_DEPTH_TEST,StateAttribute::OFF);
                    stateSet->setRenderingHint( StateSet::TRANSPARENT_BIN );

                    // Make sure this geometry is draw last. RenderBins are handled in numerical order so set bin number to 11
                    stateSet->setRenderBinDetails(11, "DepthSortedBin");

                    //Bounding box
                    if (selectedLabels.at(selectedLabels.size()-1)->getDrawMode() < osgText::Text::BOUNDINGBOX )
                    {
                        selectedLabels.at(selectedLabels.size()-1)->setDrawMode(selectedLabels.at(selectedLabels.size()-1)->getDrawMode() | osgText::Text::BOUNDINGBOX);
                        selectedLabels.at(selectedLabels.size()-1)->setPreviousDrawMode(selectedLabels.at(selectedLabels.size()-1)->getDrawMode());
                    }
                }
                else
                {
                    //Reset the stateset of the previous selected objects
                    //Clear the list of selected labels
                    for (unsigned i(0); i<selectedLabels.size();i++)
                    {
                        selectedLabels.at(i)->getParent(0)->getParent(0)->setStateSet(new StateSet());
                         if (selectedLabels.at(i)->getPreviousDrawMode() <  osgText::Text::BOUNDINGBOX )
                        {
                            selectedLabels.at(i)->setPreviousDrawMode(selectedLabels.at(i)->getPreviousDrawMode() - osgText::Text::BOUNDINGBOX);
                            selectedLabels.at(i)->setDrawMode( selectedLabels.at(i)->getDefaultDrawMode());
                        }
                    }
                    selectedLabels.clear();
                }
            }
            else
            {
                //Reset the stateset of the previous selected objects
                //Clear the list of selected labels
                for (unsigned i(0); i<selectedLabels.size();i++)
                {
                        selectedLabels.at(i)->getParent(0)->getParent(0)->setStateSet(new StateSet());
                        if (selectedLabels.at(i)->getPreviousDrawMode() <  osgText::Text::BOUNDINGBOX )
                        {
                            selectedLabels.at(i)->setPreviousDrawMode(selectedLabels.at(i)->getPreviousDrawMode() - osgText::Text::BOUNDINGBOX);
                            selectedLabels.at(i)->setDrawMode( selectedLabels.at(i)->getDefaultDrawMode());
                        }
                }
                selectedLabels.clear();
            }
        }
    }
    // 'a' : select every label
    else if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN && (ea.getKey() =='a' || ea.getKey()=='A'))
    {
        selectedLabels.clear();
        for (unsigned i(0);i<listLabels.size();i++)
        {
            selectedLabels.push_back(listLabels.at(i));

            // Disable depth testing so geometry is draw regardless of depth values of geometry already draw.
            ref_ptr<StateSet> stateSet = selectedLabels.at(selectedLabels.size()-1)->getUpdatedMatrix()->getOrCreateStateSet();
            stateSet->setMode(GL_DEPTH_TEST,StateAttribute::OFF);
            stateSet->setRenderingHint( StateSet::TRANSPARENT_BIN );

            // Make sure this geometry is draw last. RenderBins are handled in numerical order so set bin number to 11
            stateSet->setRenderBinDetails(11, "DepthSortedBin");

            //Bounding box :
            if (selectedLabels.at(i)->getDrawMode() <  osgText::Text::BOUNDINGBOX )
            {
                selectedLabels.at(i)->setDrawMode(selectedLabels.at(i)->getPreviousDrawMode() | osgText::Text::BOUNDINGBOX);
                selectedLabels.at(i)->setPreviousDrawMode(selectedLabels.at(i)->getDrawMode());
            }
        }
    }
    else
    {
         switch ( ea.getEventType() )
        {
        case osgGA::GUIEventAdapter::KEYDOWN:
            switch ( ea.getKey() )
            {
            // 'q' or 'Q' or Left : decrease the X value of the labels
            case 'q': case 'Q': case osgGA::GUIEventAdapter::KEY_Left :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    selectedLabels.at(i)->getUpdatedMatrix()->setMatrix(selectedLabels.at(i)->getUpdatedMatrix()->getMatrix()*Matrix::translate(-5,0,0));
                }
                break;
            // 'd' or 'D' or Right : increase the X value of the labels
            case 'd': case 'D':  case osgGA::GUIEventAdapter::KEY_Right :
               for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    selectedLabels.at(i)->getUpdatedMatrix()->setMatrix(selectedLabels.at(i)->getUpdatedMatrix()->getMatrix()*Matrix::translate(5,0,0));
                }
                break;
            // 'z' or 'Z' or Up : increase the Y value of the labels
            case 'z': case 'Z': case osgGA::GUIEventAdapter::KEY_Up :
               for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    selectedLabels.at(i)->getUpdatedMatrix()->setMatrix(selectedLabels.at(i)->getUpdatedMatrix()->getMatrix()*Matrix::translate(0,5,0));
                }
                break;
            // 's' or 'S' or Down : decrease the Y value of the labels
            case 's': case 'S': case osgGA::GUIEventAdapter::KEY_Down :
               for (unsigned i(0);i<selectedLabels.size();i++)
                {
                   selectedLabels.at(i)->getUpdatedMatrix()->setMatrix(selectedLabels.at(i)->getUpdatedMatrix()->getMatrix()*Matrix::translate(5,-5,0));
                }
                break;
            // '+' : increase the size of the labels
            case osgGA::GUIEventAdapter::KEY_Plus : case osgGA::GUIEventAdapter::KEY_KP_Add :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    selectedLabels.at(i)->setCharacterSize(selectedLabels.at(i)->getCharacterHeight()+1);
                }
                break;
            // '-' : decrease  the size of the lables
            case osgGA::GUIEventAdapter::KEY_Minus : case osgGA::GUIEventAdapter::KEY_KP_Subtract :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    selectedLabels.at(i)->setCharacterSize(selectedLabels.at(i)->getCharacterHeight()-1);
                }
                break;
            // 'h' or 'H' : hide the labels
            case 'h' : case 'H' :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    if (selectedLabels.at(i)->getDrawMode() != 0)
                    {
                        selectedLabels.at(i)->setDrawMode(0);
                        selectedLabels.at(i)->setPreviousDrawMode(0);
                        if (selectedLabels.at(i)->getInfoLabel() !=NULL) selectedLabels.at(i)->getInfoLabel()->setDrawMode(0);
                    }
                    else
                    {
                        selectedLabels.at(i)->setDrawMode(selectedLabels.at(i)->getDefaultDrawMode() | osgText::Text::BOUNDINGBOX);
                        selectedLabels.at(i)->setPreviousDrawMode(selectedLabels.at(i)->getDrawMode());
                        if (selectedLabels.at(i)->getInfoLabel() !=NULL) selectedLabels.at(i)->getInfoLabel()->setDrawMode(osgText::Text::TEXT);
                    }
                }
                break;
            // 'l' or 'L' : see in dark the objects linked to the selected labels
            case 'l' : case 'L' :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    StateSet * stateset = selectedLabels.at(i)->getUpdatedMatrix()->getParent(0)->getOrCreateStateSet();
                    stateset->setMode(GL_FOG, StateAttribute::ON);

                }
                break;
            // 'm' or 'M' : set the objects linked to the selected labels back to normal
            case 'm' : case 'M' :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    StateSet * stateset = selectedLabels.at(i)->getUpdatedMatrix()->getParent(0)->getOrCreateStateSet();
                    stateset->setMode(GL_FOG, StateAttribute::OFF);
                }
                break;
            // 'i'  or 'I' : show the information label if it exists
            case 'i' : case 'I' :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                  if (selectedLabels.at(i)->getInfoLabel() != NULL)
                  {
                      ref_ptr<osgText::Text> infoLabel = selectedLabels.at(i)->getInfoLabel();
                      if (infoLabel->getDrawMode()==0)
                      {
                          infoLabel->setDrawMode(osgText::Text::TEXT);
                          selectedLabels.at(i)->setPositionInit(selectedLabels.at(i)->getPositionInit()+Vec3(0,0,(infoLabel->getBound().zMax()-infoLabel->getBound().zMin())));
                      }
                      else
                      {
                           infoLabel->setDrawMode(0);
                           selectedLabels.at(i)->setPositionInit(selectedLabels.at(i)->getPositionInit()-Vec3(0,0,(infoLabel->getBound().zMax()-infoLabel->getBound().zMin())));
                      }
                  }
                }
                break;
            default:
                break;
            }
            break;
        case osgGA::GUIEventAdapter::MOVE:
            {
                ref_ptr<osgViewer::Viewer> viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

                if ( viewer)
                {
                    // To know where the mouse is:
                    ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
                    osgUtil::IntersectionVisitor iv( intersector.get() );
                    iv.setTraversalMask( ~0x1 );

                    viewer->getCamera()->accept( iv );
                    //If there is an object
                    if ( intersector->containsIntersections() )
                    {
                        osgUtil::LineSegmentIntersector::Intersection result = *(intersector->getIntersections().begin());

                        // If it is a label :
                        if (result.drawable->isSameKindAs(new lgLabel()))
                        {
                            if (dynamic_cast<lgLabel*>(result.drawable.get())->isChangingWhenMouse())
                            {
                                //See if it was already mouseLabel
                                if (mouseLabel != result.drawable)
                                {
                                    if (mouseLabel != NULL)
                                    {
                                        int characterSize = mouseLabel->getCharacterHeight();
                                        if (characterSize >2)
                                        {
                                           //Decrease size of the previous label
                                           mouseLabel->setCharacterSize(characterSize-2);
                                        }
                                    }
                                    //Increase size of the label
                                    mouseLabel = dynamic_cast<lgLabel*>(result.drawable.get());
                                    int characterSize = static_cast<lgLabel*>(mouseLabel)->getCharacterHeight();
                                    mouseLabel->setCharacterSize(characterSize+2);
                                }
                            }
                        }
                        //If it is not a label : decrease size of previous label and set to NULL the current label under the mouse
                        else
                        {
                            if (mouseLabel != NULL)
                            {
                                int characterSize = mouseLabel->getCharacterHeight();
                                if (characterSize >2)
                                {
                                    mouseLabel->setCharacterSize(characterSize-2);
                                }
                                mouseLabel = NULL;
                            }
                        }
                    }
                    //If there is no object : decrease size of previous label and set to NULL the current label under the mouse
                    else
                    {
                          if (mouseLabel != NULL)
                            {
                                 int characterSize = mouseLabel->getCharacterHeight();
                                if (characterSize >2)
                                {
                                    mouseLabel->setCharacterSize(characterSize-2);
                                }
                            }
                            mouseLabel = NULL;
                    }
                }
                break;
            }
        default:
            break;
        }
    }
    return false;
}
