#include "../include/LGInteraction.h"

#include <iostream>

using namespace osg;

bool LGInteraction::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    if (ea.getEventType()==osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL )
    {
        osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

        if ( viewer )
        {
            ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
            osgUtil::IntersectionVisitor iv( intersector.get() );

            viewer->getCamera()->accept( iv );
            if ( intersector->containsIntersections() )
            {
                osgUtil::LineSegmentIntersector::Intersection result = *(intersector->getIntersections().begin());

                if (result.drawable->isSameKindAs(new osgText::Text()))
                {
                    //Change the selected object
                    selectedLabels.push_back(dynamic_cast<osgText::Text*>(result.drawable.get()));


                    // Create and set up a state set
                    ref_ptr<StateSet> stateSet = selectedLabels.at(selectedLabels.size()-1)->getParent(0)->getParent(0)->getOrCreateStateSet();

                    // Disable depth testing so geometry is draw regardless of depth values of geometry already draw.
                    stateSet->setMode(GL_DEPTH_TEST,StateAttribute::OFF);
                    stateSet->setRenderingHint( StateSet::TRANSPARENT_BIN );
                    // Make sure this geometry is draw last. RenderBins are handled in numerical order so set bin number to 11
                    stateSet->setRenderBinDetails(11, "DepthSortedBin");


                    //Bounding box :
                    selectedLabels.at(selectedLabels.size()-1)->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);
                }
                else
                {
                    //Reset the stateset of the previous selected objects
                    for (unsigned i(0); i<selectedLabels.size();i++)
                    {
                         selectedLabels.at(i)->getParent(0)->getParent(0)->setStateSet(new StateSet());
                        selectedLabels.at(i)->setDrawMode(osgText::Text::TEXT);
                    }
                    selectedLabels.clear();
                }

            }
            else
            {
                for (unsigned i(0); i<selectedLabels.size();i++)
                {
                    selectedLabels.at(i)->getParent(0)->getParent(0)->setStateSet(new StateSet());
                    selectedLabels.at(i)->setDrawMode(osgText::Text::TEXT);
                }
                selectedLabels.clear();
            }
        }
    }
    else if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN && (ea.getKey() =='a' || ea.getKey()=='A'))
    {
        selectedLabels.clear();
        for (unsigned i(0);i<listLabels.size();i++)
        {
            selectedLabels.push_back(listLabels.at(i));


            // Create and set up a state set
            ref_ptr<StateSet> stateSet = selectedLabels.at(selectedLabels.size()-1)->getParent(0)->getParent(0)->getOrCreateStateSet();

            // Disable depth testing so geometry is draw regardless of depth values of geometry already draw.
            stateSet->setMode(GL_DEPTH_TEST,StateAttribute::OFF);
            stateSet->setRenderingHint( StateSet::TRANSPARENT_BIN );
            // Make sure this geometry is draw last. RenderBins are handled in numerical order so set bin number to 11
            stateSet->setRenderBinDetails(11, "DepthSortedBin");

            //Bounding box :
            selectedLabels.at(i)->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);
        }
    }
    else
    {
         switch ( ea.getEventType() )
        {
        case osgGA::GUIEventAdapter::KEYDOWN:
            switch ( ea.getKey() )
            {
            case 'q': case 'Q': case osgGA::GUIEventAdapter::KEY_Left :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->setMatrix(dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->getMatrix()*Matrix::translate(-5,0,0));
                }
                break;

            case 'd': case 'D':  case osgGA::GUIEventAdapter::KEY_Right :
               for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->setMatrix(dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->getMatrix()*Matrix::translate(5,0,0));
                }
                break;

            case 'z': case 'Z': case osgGA::GUIEventAdapter::KEY_Up :
               for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->setMatrix(dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->getMatrix()*Matrix::translate(0,5,0));
                }
                break;

            case 's': case 'S': case osgGA::GUIEventAdapter::KEY_Down :
               for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->setMatrix(dynamic_cast<MatrixTransform*>(selectedLabels.at(i)->getParent(0)->getParent(0))->getMatrix()*Matrix::translate(0,-5,0));
                }
                break;

            case osgGA::GUIEventAdapter::KEY_Plus : case osgGA::GUIEventAdapter::KEY_KP_Add :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    selectedLabels.at(i)->setCharacterSize(selectedLabels.at(i)->getCharacterHeight()+1);
                }
                break;

            case osgGA::GUIEventAdapter::KEY_Minus : case osgGA::GUIEventAdapter::KEY_KP_Subtract :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    selectedLabels.at(i)->setCharacterSize(selectedLabels.at(i)->getCharacterHeight()-1);
                }
                break;

            case 'h' : case 'H' :
                for (unsigned i(0);i<selectedLabels.size();i++)
                {
                    if (selectedLabels.at(i)->getDrawMode() != 0)
                         selectedLabels.at(i)->setDrawMode(0);
                    else
                        selectedLabels.at(i)->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);
                }
                break;

                default:
                break;
            }
            break;

        default:
            break;
        }
    }
    return false;
}
