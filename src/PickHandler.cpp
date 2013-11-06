#include "../include/PickHandler.h"

using namespace osg;

bool PickHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    if ( ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE || ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON || !(ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL) ) return false;
    osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

    if ( viewer )
    {
        ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
        osgUtil::IntersectionVisitor iv( intersector.get() );

        viewer->getCamera()->accept( iv );
        if ( intersector->containsIntersections() )
        {
            osgUtil::LineSegmentIntersector::Intersection result = *(intersector->getIntersections().begin());

            if (result.drawable->isSameKindAs(new osgText::Text())){

                //Reset the stateset of the previous selected object
                model->setStateSet(new StateSet());
                dynamic_cast<osgText::Text*>(dynamic_cast<Geode*>(model->getChild(0))->getDrawable(0))->setDrawMode(1);

                //Change the selected object
                model = (dynamic_cast<MatrixTransform*> (result.drawable->getParent(0)->getParent(0)));

                // Create and set up a state set
                StateSet* stateSet = new StateSet();
                model->setStateSet(stateSet);
                // Disable depth testing so geometry is draw regardless of depth values of geometry already draw.
                stateSet->setMode(GL_DEPTH_TEST,StateAttribute::OFF);
                stateSet->setRenderingHint( StateSet::TRANSPARENT_BIN );

                //Bounding box :
                osgText::Text* label = dynamic_cast<osgText::Text*>(result.drawable.get());
                label->setDrawMode(3);

                // Need to make sure this geometry is draw last. RenderBins are handled in numerical order so set bin number to 11
                stateSet->setRenderBinDetails(11, "DepthSortedBin");
            }
        }
    }
    return false;
}
