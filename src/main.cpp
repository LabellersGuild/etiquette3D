/** File : main.cpp
 * Description : Test file for our library
 */

#include <osg/Group>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgSim/MultiSwitch>
#include "../include/lgNodeVisitor.h"
#include "../include/lgNodeOverseer.h"
#include "../include/lgLabel.h"
#include "../include/test.h"
#include "../include/lgAnimation.h"
#include "../include/lgInteraction.h"
#include "../include/myLGAnimation.h"
#include "../include/myLGAnimation2.h"
#include "../include/InternalLabelAnimation.h"
#include "../include/testLGAnimation.h"
#include "../include/Evaluator.h"
#include "../include/LabelVisitor.h"
#include "../include/InExSwitch.h"
#include <osgGA/TrackballManipulator>

#include <osgText/Font>
#include <osgText/Text>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <osg/BlendFunc>
#include <osg/ShapeDrawable>

using namespace std;
using namespace osg;

lgLabel* addTextLabel(ref_ptr<Node> linkToNode, string nom_id, string nom, Vec3 recoPos, ref_ptr<osgViewer::Viewer> viewer, lgType type);
void addMoreLabel(lgNodeVisitor visitor, vector<lgLabel*> listLabels, Group* model, osgViewer::Viewer* viewer);
ref_ptr<lgLabel> addBuildingLabel(string id, vector<lgLabel*> listLabels, Group* model, osgViewer::Viewer* viewer);

int main()
{
   // Declare a group for the root of our tree and a group for the model
   ref_ptr<Group> root = new Group();
   ref_ptr<Group> model = NULL;

   osgViewer::Viewer viewer;

   // Load the model
   // If you want to see the names of the nodes, write "names" instead of "" in the next line. Do
   // Does not work for every citygml file.
    osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options("");

//    cout << "Entrer le path du fichier .citygml :" << endl;
//    string pathFile;
//    cin >> pathFile;
    string pathFile = "/home/paulyves/OpenSceneGraph-Data/waldbruecke_v0.4.0.citygml";
    
    model = dynamic_cast<Group*> (osgDB::readNodeFile(pathFile, options));

   // quit if we didn't successfully load the model
   if (! model )
   {
      cout << "could not load model" << endl;
      return -1;
   }

    //List of every label
    vector<lgLabel*> listLabels = vector<lgLabel*>();

    // Add the model group node to the root
    root->addChild(model);

    //create building labels
    ref_ptr<lgLabel> label1 = addBuildingLabel("b_12529I1", listLabels, model, &viewer);
    ref_ptr<lgLabel> label2 =addBuildingLabel("b_12529I2", listLabels, model, &viewer);
    ref_ptr<lgLabel> label3 =addBuildingLabel("b_12530I1", listLabels, model, &viewer);
    ref_ptr<lgLabel> label4 =addBuildingLabel("b_12530I2", listLabels, model, &viewer);
    ref_ptr<lgLabel> label5 =addBuildingLabel("b_12531I1", listLabels, model, &viewer);
    ref_ptr<lgLabel> label6 =addBuildingLabel("b_12531I2", listLabels, model, &viewer);
    ref_ptr<lgLabel> label7 =addBuildingLabel("b_12532I1", listLabels, model, &viewer);
    ref_ptr<lgLabel> label8 =addBuildingLabel("b_12532I2", listLabels, model, &viewer);
    ref_ptr<lgLabel> label9 =addBuildingLabel("b_12835I1", listLabels, model, &viewer);
    ref_ptr<lgLabel> label10 =addBuildingLabel("b_12586-1I1", listLabels, model, &viewer);
    
    label1->addArrow();
    label2->addArrow();
    label3->addArrow();
    label4->addArrow();
    label5->addArrow();
    label6->addArrow();
    label7->addArrow();
    label8->addArrow();
    label9->addArrow();
    label10->addArrow();
    // Create LGInteraction
    ref_ptr<lgInteraction> interaction = new lgInteraction(listLabels);
    viewer.addEventHandler(interaction.get());

    viewer.setSceneData(root);
    viewer.setCameraManipulator(new osgGA::TrackballManipulator());
    viewer.realize();

    ref_ptr<osgViewer::Viewer> pView = &viewer;


    LabelVisitor recenseur;
    model->accept(recenseur);
    
    Evaluator* testeur = new Evaluator();
    testeur->setLabelList(recenseur.getLabelList());
    
    label1->setAxisAlignment(osgText::Text::XY_PLANE);
    label2->setHidingDistance(800);
    
    //initializing evaluation
    float percentVisible = testeur->lisibility_checkAlignement(true);
    testeur->visibilityFilterCalculator();
    testeur->realTime_init();
    clock_t start;
    double timer;
    float fps;
    while (!viewer.done())
    {
        testeur->realTime_iter();
        testeur->computeLabelCollision(pView, recenseur.getLabelList());
        testeur->visibilityOutOfCamera(pView);
        testeur->computeLabelObjectDistance(pView, recenseur.getLabelList());

        viewer.frame();
    }
}

lgLabel* addTextLabel(ref_ptr<Node> linkToNode, string name_id, string name, Vec3 recoPos, ref_ptr<osgViewer::Viewer> viewer, lgType type)
{
   if (!linkToNode)
   {
      cout << "Error while creating the label" << endl;
      return NULL;
   }

   ref_ptr<lgLabel> label = new lgLabel();

  label->setLinkNode(linkToNode, recoPos);

   if (type == EXTERNAL)
   {
       label->setLabelType(type);
       ref_ptr<myLGAnimation2> animation = new myLGAnimation2(viewer);
       animation->getMapLabel()
        ->insert(std::pair<
                osg::ref_ptr<Drawable>,
                std::vector<osg::ref_ptr<lgLabel> >
                >(label, vector<ref_ptr<lgLabel> >()));
       label->getUpdatedMatrix()->setUpdateCallback(animation);

   }
   else if (type == INTERNAL_TOP)
   {
       label->setLabelType(type);
       ref_ptr<lgAnimation> animation = new lgAnimation(viewer);
       label->getUpdatedMatrix()->setUpdateCallback(animation);

   }
   else if ( type == INTERNAL_FACE)
   {
      label->setLabelType(INTERNAL_FACE);
      ref_ptr<InternalLabelAnimation> internalAnimation = new InternalLabelAnimation(viewer, label);
      ref_ptr<myLGAnimation2> myLGAnimation2Animation = new myLGAnimation2(viewer);

       myLGAnimation2Animation->getMapLabel()
        ->insert(std::pair<
                osg::ref_ptr<Drawable>,
                std::vector<osg::ref_ptr<lgLabel> >
                >(label, vector<ref_ptr<lgLabel> >()));

       label->getUpdatedMatrix()->setUpdateCallback(myLGAnimation2Animation);
      ref_ptr<InExSwitch> animation = new InExSwitch(viewer, internalAnimation, myLGAnimation2Animation);
      label->getUpdatedMatrix()->setUpdateCallback(animation);
   }

   test myTest=test();
   myTest.test_label_setLinkNode(linkToNode,label);

   label->setCharacterSize(5);
   label->setText(name, osgText::String::ENCODING_UTF8 );

   label->setColor( Vec4(0.2,0.2,1,1.0f) );
   label->setDrawMode(5); //To see the background of the label
   label->setBoundingBoxColor(Vec4(255,255,255,0.5)); //Color of the background of the label

   //Record the draw mode
   label->setPreviousDrawMode(label->getDrawMode());
   label->setDefaultDrawMode(label->getDrawMode());
   label->setFontResolution(64,64);
   // To change the font:
   //label->setFont("/pathToTheFont/font.ttf");
//   label->setFont("/home/tbrunel/Téléchargements/OSG_data/OSG_data/fonts/arial.ttf");

   return label;
}


ref_ptr<lgLabel> addBuildingLabel(string id, vector<lgLabel*> listLabels, Group* model, osgViewer::Viewer* viewer)
{
  lgNodeVisitor visitor(id);
  visitor.setNameToFind(id);
  model->accept(visitor);
  Node* aNode = visitor.getFirst();
  visitor.feedFoundPointList(*(aNode));
  ref_ptr<lgLabel> label = addTextLabel(aNode,aNode->getName(),aNode->getName(),visitor.recommendedCoordinates(), viewer,INTERNAL_FACE);
  listLabels.push_back(label);
  return label;
}

