#include <iostream>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/Camera>

#include "../include/MovingCallback.h"
#include "../include/PickHandler.h"

using namespace std;
using namespace osg;

int main()
{
    osgViewer::Viewer viewer;
    ref_ptr<Group> root = new Group;

    //Terrain
    /*
    ref_ptr<ShapeDrawable> floor = new ShapeDrawable;
    floor->setShape( new Box(Vec3(0.0f, 0.0f, -5.0f),50,50,1) );
    ref_ptr<Geode> geodeFloor = new Geode;
    geodeFloor->addDrawable(floor);
    geodeFloor->setDataVariance( Object::STATIC );
    root->addChild(geodeFloor);
    */
    /*
    //Sphere 1 :
    ref_ptr<ShapeDrawable> sphere = new ShapeDrawable;
    sphere->setShape( new Sphere(Vec3(0.0f, 0.0f, 0.0f),1.0f) );
    sphere->setColor( Vec4(0.0f, 0.5f, 1.0f, 0.5f) );
    ref_ptr<MatrixTransform> mt = new MatrixTransform;
    mt->setDataVariance( Object::DYNAMIC ); // Tell OSG that this node will move
    mt->setMatrix(Matrix::translate(10,0,5));
    ref_ptr<Geode> geodeSphere = new Geode;
    geodeSphere->addDrawable(sphere);
    mt->addChild(geodeSphere);
    root->addChild(mt);
    // Attach the MovingCallback to the Sphere
    mt->setUpdateCallback( new MovingCallback(&viewer));
    */

    /*
    //Sphere 2 :
    ref_ptr<ShapeDrawable> sphere2 = new ShapeDrawable;
    sphere2->setShape( new Sphere(Vec3(0.0f, 0.0f, 0.0f),4.0f) );
    sphere2->setColor( Vec4(1.0f, 1.0f, 0.0f, 0.5f) );
    ref_ptr<MatrixTransform> mt2 = new MatrixTransform;
    mt2->setMatrix(Matrix::translate(-10,0,5));
    ref_ptr<Geode> geodeSphere2 = new Geode;
    mt2->addChild(geodeSphere2);
    geodeSphere2->addDrawable(sphere2);
    mt2->setDataVariance( Object::STATIC );
    root->addChild(mt2);
    */

        //HUD for test validation
        Geode* HUDGeode = new Geode();
        Projection* HUDProjectionMatrix = new Projection;
        HUDProjectionMatrix->setMatrix(Matrix::ortho2D(0,1920,0,1080));
        MatrixTransform* HUDModelViewMatrix = new MatrixTransform;
        HUDModelViewMatrix->setReferenceFrame(Transform::ABSOLUTE_RF);
        HUDModelViewMatrix->setMatrix(Matrix::identity());
        // To see the HUD :
        //root->addChild(HUDProjectionMatrix);
        HUDProjectionMatrix->addChild(HUDModelViewMatrix);
        HUDModelViewMatrix->addChild( HUDGeode );
        Geometry* HUDBackgroundGeometry = new Geometry();

       Vec3Array* HUDBackgroundVertices = new Vec3Array;
       HUDBackgroundVertices->push_back( Vec3(   20,  20,-1) ); // -1 : c'est devant, mais du coup, c'est par dessus
       HUDBackgroundVertices->push_back( Vec3(800,  20,-1) );
       HUDBackgroundVertices->push_back( Vec3(800,600,-1) );
       HUDBackgroundVertices->push_back( Vec3(   20,600,-1) );

       DrawElementsUInt* HUDBackgroundIndices =
          new DrawElementsUInt(PrimitiveSet::POLYGON, 0);
       HUDBackgroundIndices->push_back(0);
       HUDBackgroundIndices->push_back(1);
       HUDBackgroundIndices->push_back(2);
       HUDBackgroundIndices->push_back(3);

       Vec4Array* HUDcolors = new Vec4Array;
       HUDcolors->push_back(Vec4(0.8f,0.8f,0.8f,0.5f)); // dernière valeur indique la transparence du fond

       Vec3Array* HUDnormals = new Vec3Array;
       HUDnormals->push_back(Vec3(0.0f,0.0f,1.0f));
       HUDBackgroundGeometry->setNormalArray(HUDnormals);
       HUDBackgroundGeometry->setNormalBinding(Geometry::BIND_OVERALL);

       HUDBackgroundGeometry->addPrimitiveSet(HUDBackgroundIndices);
       HUDBackgroundGeometry->setVertexArray(HUDBackgroundVertices);
       HUDBackgroundGeometry->setColorArray(HUDcolors);
       HUDBackgroundGeometry->setColorBinding(Geometry::BIND_OVERALL);

        HUDGeode->addDrawable(HUDBackgroundGeometry);

       StateSet* HUDStateSet = new StateSet();
       HUDGeode->setStateSet(HUDStateSet);
       HUDStateSet->setRenderBinDetails(11, "DepthSortedBin");
       HUDStateSet->setMode(GL_BLEND,StateAttribute::ON);
       HUDStateSet->setMode(GL_DEPTH_TEST,StateAttribute::OFF);
       HUDStateSet->setRenderingHint( StateSet::TRANSPARENT_BIN );

    //Label 1
    ref_ptr<MatrixTransform> mtLabel = new MatrixTransform();
    mtLabel->setMatrix(Matrixd::translate(0,-12,5));
    root->addChild(mtLabel.get());
    ref_ptr<Geode> labelGeode = new Geode();
    mtLabel->addChild(labelGeode.get());
    ref_ptr<osgText::Text> textOne = new osgText::Text();
    labelGeode->addDrawable(textOne.get());
    textOne->setText("I am a label", osgText::String::ENCODING_UTF8 );
    textOne->setAxisAlignment(osgText::Text::SCREEN);
    // To center the label on the node : // The problem is that the movingCallback is not working properly anymore.
    textOne->setAlignment(osgText::Text::CENTER_BOTTOM);
    textOne->setColor(Vec4(192.0f/255.0f,0,0,1.0f) );
    textOne->setFontResolution(64,64);
    textOne->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_CENTER);
    // If we want to have labels with constant size on screen :
    //textOne->setCharacterSizeMode(osgText::TextBase::SCREEN_COORDS);
    //If we want the label to have a constant size when far and a variable size when close :
    //textOne->setCharacterSizeMode(osgText::TextBase::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT);
    // To make the label stay "on" the map : + alors remove the movingcallback.
    //textOne->setAutoRotateToScreen(false);
    textOne->setCharacterSize(3);
    // To add a pseudo bounding box (on the text, not on the object label) :
    textOne->setDrawMode(3);
    // To add a backgrouund !
    //textOne->setDrawMode(5);
    // To change the color of the boundingbox :
    textOne->setBoundingBoxColor(Vec4(0,0,1,1));
    mtLabel->setUpdateCallback( new MovingCallback(&viewer, HUDBackgroundVertices));

    //Label 2
    ref_ptr<MatrixTransform> mtLabel2 = new MatrixTransform();
    mtLabel2->setMatrix(Matrixd::translate(0,10,5));
    root->addChild(mtLabel2.get());
    ref_ptr<Geode> labelGeode2 = new Geode();
    mtLabel2->addChild(labelGeode2.get());
    ref_ptr<osgText::Text> textTwo = new osgText::Text();
    labelGeode2->addDrawable(textTwo.get());
    textTwo->setText("I am a label too", osgText::String::ENCODING_UTF8 );
    textTwo->setAxisAlignment(osgText::Text::SCREEN);
    textTwo->setAlignment(osgText::Text::CENTER_BOTTOM);
    textTwo->setCharacterSize(3);
    textTwo->setColor(Vec4(0,0,192.0f/255.0f,1.0f) );
    textTwo->setFontResolution(64,64);
   	textTwo->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_CENTER);
    textTwo->setColorGradientMode(osgText::Text::OVERALL);
    mtLabel2->setUpdateCallback( new MovingCallback(&viewer, HUDBackgroundVertices));

    // Citygml model
    ref_ptr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options("");
    ref_ptr<MatrixTransform> citygmlModel = dynamic_cast<MatrixTransform*>(osgDB::readNodeFile("/home/tbrunel/Documents/Cartes3D-CityGML/Frankfurt_Street_Setting_LOD3/Frankfurt_Street_Setting_LOD3.citygml", options));
    if (! citygmlModel ){
      std::cout << "could not load model" << std::endl;
      return -1;
    }
    citygmlModel->setMatrix(Matrixd::translate(-20,-20,0));
    root->addChild(citygmlModel.get());

    // Create picker
     ref_ptr<PickHandler> picker = new PickHandler(mtLabel.get());
    viewer.addEventHandler( picker.get() );

    viewer.setSceneData( root.get() );
    viewer.setCameraManipulator(new osgGA::TrackballManipulator());
    viewer.realize();

    while( !viewer.done() )
    {
        viewer.frame();
    }
}
