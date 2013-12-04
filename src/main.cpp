#include <osg/Group>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgSim/MultiSwitch>
#include "../include/lgNodeVisitor.h"
#include "../include/lgNodeOverseer.h"
#include "../include/lgLabel.h"
#include "../include/test.h"
#include "../include/LGAnimation.h"
#include "../include/LGInteraction.h"
#include "../include/myLGAnimation.h"
#include "../include/myLGAnimation2.h"
#include <osgGA/TrackballManipulator>

#include <osgText/Font>
#include <osgText/Text>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <osg/BlendFunc>

using namespace std;
using namespace osg;

lgLabel* addTextLabel(Node* g, std::string nom_id, std::string nom, Vec3 recoPos, osgViewer::Viewer* viewer, lgType type);

/// I don't use a makefile, but only Code::Blocks.
/// To add the library dependencies : right clic on project, build options,
/// linker settings, add every .so file in the /lib directory of OpenSceneGraph
/// Then, to use the program, the building is done by codeblocks, but launch it with a console.
/// The program is in /bin/Debug of the project

int main()
{
   // Declare a group for the root of our tree and a group for the model
   Group* root = new Group();
   Group* model = NULL;
   osgViewer::Viewer viewer;

   // Load the model
   // If you want to see the names of the nodes, write "names" instead of "" in the next line.
    osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options("");
    //  erreur lors de l'input d'une variable string

    cout << "Entrer le path du fichier .citygml :" << endl;
    string pathFile;
    cin >> pathFile;
    //string pathFile = "/home/paulyves/OpenSceneGraph-Data/Munich_v_1_0_0.citygml";
    model = dynamic_cast<osg::Group*> (osgDB::readNodeFile(pathFile, options));

   // quit if we didn't successfully load the models
   if (! model )
   {
      std::cout << "could not load model" << std::endl;
      return -1;
   }

    test monTest=test();
    lgNodeVisitor findNode=monTest.test_lgNodeVisitor_initialisation(model);

    Vec3 positionCalc = findNode.recommendedCoordinates();
    std::cout << "Le point recommandé est : x:" << positionCalc.x() << " y=" << positionCalc.y() << " z=" << positionCalc.z() << std::endl;

    // Add this group node to the root
    root->addChild(model);
    cout << findNode.getNodeList().size() << endl;
    Node* rootModel = findNode.getFirst();

    // Add the label
    // You can change the name of the label, it is the 3rd argument of the next line
    vector<lgLabel*> listLabels = vector<lgLabel*>();
    ref_ptr<lgLabel> label1 = addTextLabel(rootModel, rootModel->getName(), rootModel->getName(), positionCalc, &viewer, EXTERNAL);

    //Hide the label if it is too far
    label1->setHidingDistance(1000);
    label1->setSeeInTransparency(true);

    //second label
    cout << "Entrer l'id du noeud à étiquetter :" << endl;
    string idNode2;
    cin >> idNode2;
    //string idNode2 = "ID_311-TheMagdalena";
    lgNodeVisitor findNode2(idNode2);
    model->accept(findNode2);
    Node* secondNode = findNode2.getFirst();
    findNode2.feedFoundPointList(*(findNode2.getFirst()));
    Vec3 calcPos2 = findNode2.recommendedCoordinates();
    std::cout << "Le point recommandé est : x:" << calcPos2.x() << " y=" << calcPos2.y() << " z=" << calcPos2.z() << std::endl;
    ref_ptr<lgLabel> label2 = addTextLabel(secondNode, secondNode->getName(), secondNode->getName(), calcPos2, &viewer, EXTERNAL);

    // Create LGInteraction
    listLabels.push_back(label1.get());
    listLabels.push_back(label2.get());
    ref_ptr<LGInteraction> interaction = new LGInteraction(listLabels);
    viewer.addEventHandler(interaction.get());

    viewer.setSceneData(root);
    viewer.setCameraManipulator(new osgGA::TrackballManipulator());
    viewer.realize();
    ref_ptr<osgViewer::Viewer> pView = &viewer;
    float distWin = label1->distance2d(pView, label2);
    cout << "distance 2d " << distWin << endl;
    float dist3d = label1->distance(label2);
    cout<<"distance 3d " << dist3d<<endl;
    Vec3 vecDist = label1->distanceVec(label2);
    cout<<"distance vec "<<vecDist.x()<<" "<<vecDist.y()<<" "<<vecDist.z()<<endl;
    float calcDist = sqrt(pow(vecDist.x(),2.0)+pow(vecDist.y(),2.0)+pow(vecDist.z(),2.0));
    cout<<"distance vec "<<calcDist<<endl;
    float a2dBox = label1->distance2dBox(pView,label2);
    osg::Vec4 bounds = label1->compute2dBox(pView);
    cout<<"bounds "<<bounds.x()<<" "<<bounds.y()<<" "<<bounds.z()<<" "<<bounds.w()<<endl;

    while (!viewer.done()) {
        float a2dBox = label1->distance2dBox(pView,label2);
        //cout<<"distance 2d "<<a2dBox<<endl;
        viewer.frame();
    }
}

lgLabel* addTextLabel(Node* g, std::string name_id, std::string name, Vec3 recoPos, osgViewer::Viewer* viewer, lgType type)
{
   if (!g)
   {
       std::cout << "Error while creating the label" << std::endl;
      return NULL;
   }
   lgLabel* label = new lgLabel();

   ref_ptr<Node> linkToNode = g;

   if (type == EXTERNAL)
   {
       ref_ptr<myLGAnimation2> animation = new myLGAnimation2(viewer);
       label->setLinkNode(linkToNode, recoPos);
       label->setLabelType(type, animation);

   }
   else if (type == INTERNAL_TOP)
   {
       ref_ptr<LGAnimation> animation = new LGAnimation(viewer);
       label->setLinkNode(linkToNode, recoPos);
       label->setLabelType(type, animation);

   }
   else //INTERNAL_FACE
   {
       ref_ptr<LGAnimation> animation = new LGAnimation(viewer);
       label->setLinkNode(linkToNode, recoPos);
       label->setLabelType(type, animation);
   }
   test monTest=test();
   monTest.test_label_setLinkNode(linkToNode,label);

   label->setCharacterSize(5);
   // TODO : change the path to the font
   //label->setFont("/home/tbrunel/Telechargements/OSG_data/OSG_data/fonts/arial.ttf");
   label->setText(name, osgText::String::ENCODING_UTF8 );
   label->setColor( Vec4(192.0f/255.0f,0,0,1.0f) );
   label->setDrawMode(osgText::Text::TEXT |
                             osgText::Text::ALIGNMENT);
    //Record the draw mode
   label->setPreviousDrawMode(label->getDrawMode());
   label->setDefaultDrawMode(label->getDrawMode());
   label->setFontResolution(64,64);

   //testing that we can have absolute position
   cout << "label position" << endl;
   cout << label->getPosition().x() << endl;
   cout << label->getPosition().y() << endl;
   cout << label->getPosition().z() << endl;

   cout << "label absolute position" << endl;
   cout << label->getAbsolutePosition().x() << endl;
   cout << label->getAbsolutePosition().y() << endl;
   cout << label->getAbsolutePosition().z() << endl;

   //Testing the distance between the label and the camera :
   cout << "Distance label-camera" << endl;
   cout << label->distanceCamera(viewer);

   return label;
}
