/* Fichier : myMain.cpp
 * Description : Fichier de test pour les classes LGInteraction et LGAnimation
 *      Deux �tiquettes sont cr��es, ainsi qu'un �tiquette d'information rattach�e � la 2e �tiquette.
 *      Un fichier citygml est charg�, mais les �tiquettes ne sont pas reli�es � un noeud de cette structure.
 *      Le code mis en commentaire pr�cise des options de modification des �tiquettes
 * Auteur : Thomas Brunel
 */

#include <iostream>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/Camera>

#include "../include/LGAnimation.h"
#include "../include/LGInteraction.h"
#include "../include/myLGAnimation.h"

using namespace std;
using namespace osg;

int mainSecond()
{
    // Creation of the tree nodes :
    osgViewer::Viewer viewer;
    ref_ptr<Group> root = new Group;
    ref_ptr<MatrixTransform> mtLabel1 = new MatrixTransform;
    ref_ptr<Geode> geodeLabel1 = new Geode;
    ref_ptr<osgText::Text> label1 = new osgText::Text;
    ref_ptr<MatrixTransform> mtLabel2 = new MatrixTransform;
    ref_ptr<Geode> geodeLabel2 = new Geode;
    ref_ptr<osgText::Text> label2 = new osgText::Text;
    ref_ptr<Geode> geodeInfoLabel2 = new Geode;
    ref_ptr<osgText::Text> infoLabel2 = new osgText::Text;

    //loading citygml model :
    ref_ptr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options("");
    ref_ptr<MatrixTransform> citygmlModel = dynamic_cast<MatrixTransform*>(osgDB::readNodeFile("/home/tbrunel/Documents/Cartes3D-CityGML/Frankfurt_Street_Setting_LOD3/Frankfurt_Street_Setting_LOD3.citygml", options));
    if (! citygmlModel )
    {
        std::cout << "could not load model" << std::endl;
        return -1;
    }

    // Linking the nodes :
    root->addChild(mtLabel1.get());
    mtLabel1->addChild(geodeLabel1.get());
    geodeLabel1->addDrawable(label1.get());
    root->addChild(mtLabel2.get());
    mtLabel2->addChild(geodeLabel2.get());
    mtLabel2->addChild(geodeInfoLabel2.get());
    geodeLabel2->addDrawable(label2.get());
    geodeInfoLabel2->addDrawable(infoLabel2.get());
    root->addChild(citygmlModel.get());

    //List of the labels :
     vector<osgText::Text*> listLabels = vector<osgText::Text*>();

    //Label 1 setup :
    mtLabel1->setMatrix(Matrixd::translate(0,-12,5));
    mtLabel1->setUpdateCallback( new myLGAnimation(&viewer)); //link the LGAnimation to the label
    label1->setText("Label 1", osgText::String::ENCODING_UTF8 );
    label1->setAxisAlignment(osgText::Text::SCREEN);
    label1->setAlignment(osgText::Text::CENTER_BOTTOM); // To center the label on the node
    label1->setColor(Vec4(192.0f/255.0f,0,0,1.0f) );
    label1->setFontResolution(64,64);
    label1->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_CENTER);
    //label1->setCharacterSizeMode(osgText::TextBase::SCREEN_COORDS); // If we want to have labels with constant size on screen
    //label1->setCharacterSizeMode(osgText::TextBase::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT);  //If we want the label to have a constant size when far and a variable size when close :
    //label1->setAutoRotateToScreen(false);  // To make the label stay "on" the map. Remove the LGAnimations
    label1->setCharacterSize(3);
    //label1->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX); // To add a pseudo bounding box (on the text, not on the object label)
    //label1->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX | osgText::Text::FILLEDBOUNDINGBOX);   // To add a backgrouund !
    label1->setBoundingBoxColor(Vec4(0,0,1,1));  // To change the color of the boundingbox :
    listLabels.push_back(label1);

    //Label 2 setup :
    mtLabel2->setMatrix(Matrixd::translate(25,10,5));
    mtLabel2->setUpdateCallback( new myLGAnimation(&viewer));
    mtLabel2->addChild(geodeLabel2.get());
    label2->setText("Label 2", osgText::String::ENCODING_UTF8 );
    label2->setAxisAlignment(osgText::Text::SCREEN);
    label2->setAlignment(osgText::Text::CENTER_BOTTOM);
    label2->setCharacterSize(3);
    label2->setColor(Vec4(0,0,192.0f/255.0f,1.0f) );
    label2->setFontResolution(64,64);
   	label2->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_CENTER);
    label2->setColorGradientMode(osgText::Text::OVERALL);
    label2->setDrawMode(osgText::Text::TEXT /*| osgText::Text::ALIGNMENT*/);
    listLabels.push_back(label2);

    //Info label of label 2:
    infoLabel2->setText("More info \n about label 2", osgText::String::ENCODING_UTF8 );
    infoLabel2->setAxisAlignment(osgText::Text::SCREEN);
    infoLabel2->setAlignment(osgText::Text::CENTER_BOTTOM);
    infoLabel2->setCharacterSize(2);
    infoLabel2->setDrawMode(0); // Hidden by default
    label2->setText("label 2 \n ...");
    geodeInfoLabel2->setNodeMask( 0x1 );

    // Citygml model setup
    citygmlModel->setMatrix(Matrixd::translate(-20,-20,0));

    // Create LGInteraction
    ref_ptr<LGInteraction> interaction = new LGInteraction(listLabels);

    // Vieewer setup :
    viewer.addEventHandler(interaction.get());
    viewer.setSceneData( root.get() );
    viewer.setCameraManipulator(new osgGA::TrackballManipulator());
    viewer.realize();

    //Run
    while( !viewer.done() )
    {
        viewer.frame();
    }
}
