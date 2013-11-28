/* Fichier : LGInteraction.cpp
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

 #include "../include/LGInteraction.h"

#include <iostream>

using namespace osg;

/* Constructeur
 * Argument :
 * - l : vector<osgText::Text> : la liste des etiquettes creees dans le programme principal.
 */
LGInteraction::LGInteraction(std::vector<lgLabel*> l) : listLabels(l) {};


  /* Fonction qui s'active automatiquement lorsqu'un evenement est detecte
   * Arguments
   * - ea : osgGA::GUIEventAdapter
   * - aa : osgGA::GUIActionAdapter
   * Retour : renvoie toujours false. La valeur de retour indique si l'evenement a ete traite
   * Si true est renvoye, OSG considere que l'evenement n'est plus necessaire a aucun handler et sera ignore par les autres handlers, dont le "camera manipulator"
   * (cf. Beginner's guide p. 237)
   */
bool LGInteraction::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    // Ctrl + left click : label picking
    if (ea.getEventType()==osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL )
    {
        ref_ptr<osgViewer::Viewer> viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

        if ( viewer )
        {
            // To know on what the user clicked :
            ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
            osgUtil::IntersectionVisitor iv( intersector.get() );
            iv.setTraversalMask( ~0x1 );

            viewer->getCamera()->accept( iv );
            if ( intersector->containsIntersections() )
            {
                osgUtil::LineSegmentIntersector::Intersection result = *(intersector->getIntersections().begin());

                // If it is a label :
                if (result.drawable->isSameKindAs(new lgLabel()))
                {
                    //Add the label to the list of selected labels
                    selectedLabels.push_back(dynamic_cast<lgLabel*>(result.drawable.get()));

                    // Disable depth testing so geometry is draw regardless of depth values of geometry already draw.
                    ref_ptr<StateSet> stateSet = selectedLabels.at(selectedLabels.size()-1)->getParent(0)->getParent(0)->getOrCreateStateSet();
                    stateSet->setMode(GL_DEPTH_TEST,StateAttribute::OFF);
                    stateSet->setRenderingHint( StateSet::TRANSPARENT_BIN );
                    // Make sure this geometry is draw last. RenderBins are handled in numerical order so set bin number to 11
                    stateSet->setRenderBinDetails(11, "DepthSortedBin");

                    //Bounding box :
                    selectedLabels.at(selectedLabels.size()-1)->setDrawMode(selectedLabels.at(selectedLabels.size()-1)->getDrawMode() | osgText::Text::BOUNDINGBOX);
                    selectedLabels.at(selectedLabels.size()-1)->setPreviousDrawMode(selectedLabels.at(selectedLabels.size()-1)->getDrawMode());
                }
                else
                {
                    //Reset the stateset of the previous selected objects
                    //Clear the list of selected labels
                    for (unsigned i(0); i<selectedLabels.size();i++)
                    {
                        selectedLabels.at(i)->getParent(0)->getParent(0)->setStateSet(new StateSet());
                        selectedLabels.at(i)->setPreviousDrawMode(selectedLabels.at(i)->getPreviousDrawMode() - osgText::Text::BOUNDINGBOX);
                        selectedLabels.at(i)->setDrawMode( selectedLabels.at(i)->getDefaultDrawMode());

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
                        selectedLabels.at(i)->setPreviousDrawMode(selectedLabels.at(i)->getPreviousDrawMode() - osgText::Text::BOUNDINGBOX);
                        selectedLabels.at(i)->setDrawMode( selectedLabels.at(i)->getDefaultDrawMode());
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
            ref_ptr<StateSet> stateSet = selectedLabels.at(selectedLabels.size()-1)->getParent(0)->getParent(0)->getOrCreateStateSet();
            stateSet->setMode(GL_DEPTH_TEST,StateAttribute::OFF);
            stateSet->setRenderingHint( StateSet::TRANSPARENT_BIN );
            // Make sure this geometry is draw last. RenderBins are handled in numerical order so set bin number to 11
            stateSet->setRenderBinDetails(11, "DepthSortedBin");

            //Bounding box :
            selectedLabels.at(i)->setDrawMode(selectedLabels.at(i)->getPreviousDrawMode() | osgText::Text::BOUNDINGBOX);
            selectedLabels.at(i)->setPreviousDrawMode(selectedLabels.at(i)->getDrawMode());
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
                    dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->setMatrix(dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->getMatrix()*Matrix::translate(-5,0,0));
                }
                break;
            // 'd' or 'D' or Right : increase the X value of the labels
            case 'd': case 'D':  case osgGA::GUIEventAdapter::KEY_Right :
               for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->setMatrix(dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->getMatrix()*Matrix::translate(5,0,0));
                }
                break;
            // 'z' or 'Z' or Up : increase the Y cvalue of the labels
            case 'z': case 'Z': case osgGA::GUIEventAdapter::KEY_Up :
               for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->setMatrix(dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->getMatrix()*Matrix::translate(0,5,0));
                }
                break;
            // 's' or 'S' or Down : decrease the Y value of the labels
            case 's': case 'S': case osgGA::GUIEventAdapter::KEY_Down :
               for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->setMatrix(dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->getMatrix()*Matrix::translate(0,-5,0));
                }
                break;
            // '+' : increase size of the labels
            case osgGA::GUIEventAdapter::KEY_Plus : case osgGA::GUIEventAdapter::KEY_KP_Add :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    selectedLabels.at(i)->setCharacterSize(selectedLabels.at(i)->getCharacterHeight()+1);
                }
                break;
            // '-' : decrease size of the lables
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
                    ///TO DO : ne marche plus
                    if (selectedLabels.at(i)->getDrawMode() != 0)
                    {
                        selectedLabels.at(i)->setDrawMode(0);
                        selectedLabels.at(i)->setPreviousDrawMode(0);
                    }
                    else
                    {
                        selectedLabels.at(i)->setDrawMode(selectedLabels.at(i)->getDefaultDrawMode() | osgText::Text::BOUNDINGBOX);
                        selectedLabels.at(i)->setPreviousDrawMode(selectedLabels.at(i)->getDrawMode());
                    }
                }
                break;
            // 'l' or 'L' : see the drawables corresponding to the labels
            case 'l' : case 'L' :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    ///TO DO : the fog has to be applied on the Node linked to the label. The LGLabel class is needed here.
                    StateSet * stateset = selectedLabels.at(i)->getParent(0)->getParent(0)->getParent(0)->getOrCreateStateSet();
                    stateset->setMode(GL_FOG, StateAttribute::ON);
                }
                break;
            // 'm' or 'M' : don't see the drawables corresponding to the labels
            case 'm' : case 'M' :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    ///TO DO : the fog has to be applied on the Node linked to the label. The LGLabel class is needed here.
                    StateSet * stateset = selectedLabels.at(i)->getParent(0)->getParent(0)->getParent(0)->getOrCreateStateSet();
                    stateset->setMode(GL_FOG, StateAttribute::OFF);
                }
                break;
            // 'i' : show info label if it exists
            case 'i' : case 'I' :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    // hasInfoLabel()
                  if (dynamic_cast<Group*>(selectedLabels.at(i)->getParent(0)->getParent(0))->getNumChildren()>1)
                  {
                      ref_ptr<osgText::Text> infoLabel = dynamic_cast<osgText::Text*>(dynamic_cast<Geode*>(dynamic_cast<Group*>(selectedLabels.at(i)->getParent(0)->getParent(0))->getChild(1))->getDrawable(0));
                      if (infoLabel->getDrawMode()==0)
                      {
                          infoLabel->setDrawMode(osgText::Text::TEXT);
                          selectedLabels.at(i)->setPosition(selectedLabels.at(i)->getPosition()+Vec3(0,0,infoLabel->getBound().zMax()));
                      }
                      else
                      {
                           infoLabel->setDrawMode(0);
                           selectedLabels.at(i)->setPosition(selectedLabels.at(i)->getPosition()-Vec3(0,0,infoLabel->getBound().zMax()));
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
                    // To know on what the user clicked :
                    ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
                    osgUtil::IntersectionVisitor iv( intersector.get() );
                    iv.setTraversalMask( ~0x1 );

                    viewer->getCamera()->accept( iv );
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
                                           mouseLabel->setCharacterSize(characterSize-2);
                                        }
                                    }
                                    mouseLabel = dynamic_cast<lgLabel*>(result.drawable.get());
                                    int characterSize = static_cast<lgLabel*>(mouseLabel)->getCharacterHeight();
                                    mouseLabel->setCharacterSize(characterSize+2);
                                }
                            }
                        }
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
