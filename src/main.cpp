#include <osg/Group>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgSim/MultiSwitch>
#include "../include/lgNodeVisitor.h"
#include "../include/lgNodeOverseer.h"
#include "../include/lgLabel.h"
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

lgLabel* addTextLabel(Node* g, std::string nom_id, std::string nom, Vec3 recoPos, osgViewer::Viewer* viewer);

/// I don't use a makefile, but only Code::Blocks.
/// To add the library dependencies : right clic on project, build options,
/// linker settings, add every .so file in the /lib directory of OpenSceneGraph
/// Then, to use the program, the building is done by codeblocks, but a launch it with a console.
    /// The program is in /bin/Debug of the project
int main()
{
   // Declare a group for the root of our tree and a group for the model
   Group* root = new Group();
   Group* model = NULL;
   osgViewer::Viewer viewer;

   // Load the model
   // TODO : change the file name
   // If you want to see the names of the nodes, write "names" instead of "" in the next line.
    osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options("");
    //    erreur lors de l'input d'une variable string

    cout << "Entrer le path du fichier .citygml :" << endl;
    string pathFile;
    cin >> pathFile;
//   string pathFile = "/home/paulyves/OpenSceneGraph-Data/Munich_v_1_0_0.citygml";
    model = dynamic_cast<osg::Group*> (osgDB::readNodeFile(pathFile, options));

   // quit if we didn't successfully load the models
   if (! model )
   {
      std::cout << "could not load model" << std::endl;
      return -1;
   }

    // Find the node with its ID :
    // TODO : you can change the ID you are looking for
    //    erreur lors de l'input d'une variable string

    cout << "Entrer l'id du noeud à étiquetter :" << endl;
    string idNode;
    cin >> idNode;
//    string idNode = "ID_276003000001240";

    lgNodeVisitor findNode(idNode);
    model->accept(findNode);

    findNode.feedFoundPointList(*(findNode.getFirst()));
    std::vector<Vec3> points = findNode.getFoundPointList();
    std::cout << "On a trouvé un liste de " << points.size() << " points" << std::endl;

    Vec3 positionCalc = findNode.recommendedCoordinates();
    std::cout << "Le point recommandé est : x:" << positionCalc.x() << " y=" << positionCalc.y() << " z=" << positionCalc.z() << std::endl;

    // Add this group node to the root
    root->addChild(model);
    cout << findNode.getNodeList().size() << endl;
    Node* rootModel = findNode.getFirst();

    // Add the label
    // You can change the name of the label, it is the 3rd argument of the next line
    vector<lgLabel*> listLabels = vector<lgLabel*>();
    ref_ptr<lgLabel> textOne = addTextLabel(rootModel, rootModel->getName(), rootModel->getName(), positionCalc, &viewer);

    //Hide the label if it is too far
    textOne->setHidingDistance(1000);
    textOne->setSeeInTransparency(true);

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
    ref_ptr<lgLabel> textTwo = addTextLabel(secondNode, secondNode->getName(), secondNode->getName(), calcPos2, &viewer);


    // Create LGInteraction
    listLabels.push_back(textOne.get());
    listLabels.push_back(textTwo.get());
    ref_ptr<LGInteraction> interaction = new LGInteraction(listLabels);
    viewer.addEventHandler(interaction.get());

    viewer.setSceneData(root);
    viewer.setCameraManipulator(new osgGA::TrackballManipulator());
    viewer.realize();
    ref_ptr<osgViewer::Viewer> pView = &viewer;
    float distWin = textOne->distance2d(pView, textTwo);
    cout << "distance 2d " << distWin << endl;
    float dist3d = textOne->distance(textTwo);
    cout<<"distance 3d " << dist3d<<endl;
    Vec3 vecDist = textOne->distanceVec(textTwo);
    cout<<"distance vec "<<vecDist.x()<<" "<<vecDist.y()<<" "<<vecDist.z()<<endl;
    float calcDist = sqrt(pow(vecDist.x(),2.0)+pow(vecDist.y(),2.0)+pow(vecDist.z(),2.0));
    cout<<"distance vec "<<calcDist<<endl;
    float a2dBox = textOne->distance2dBox(pView,textTwo);
    osg::Vec4 bounds = textOne->compute2dBox(pView);
    cout<<"bounds "<<bounds.x()<<" "<<bounds.y()<<" "<<bounds.z()<<" "<<bounds.w()<<endl;
    while (!viewer.done()) {
        float a2dBox = textOne->distance2dBox(pView,textTwo);
        cout<<"distance 2d "<<a2dBox<<endl;
        viewer.frame();
    }
}

lgLabel* addTextLabel(Node* g, std::string name_id, std::string name, Vec3 recoPos, osgViewer::Viewer* viewer)
{
   if (!g)
   {
       std::cout << "Error while creating the label" << std::endl;
      return NULL;
   }
   lgLabel* textOne = new lgLabel();

   ref_ptr<Node> linkToNode = g;
   ref_ptr<myLGAnimation2> animation = new myLGAnimation2(viewer);
   textOne->setLinkNode(linkToNode, recoPos, animation);
   textOne->setCharacterSize(5);
   // TODO : change the path to the font
   //textOne->setFont("/home/tbrunel/T�l�chargements/OSG_data/OSG_data/fonts/arial.ttf");
   textOne->setText(name, osgText::String::ENCODING_UTF8 );
   textOne->setAxisAlignment(osgText::Text::SCREEN);
   textOne->setColor( Vec4(192.0f/255.0f,0,0,1.0f) );
   textOne->setDrawMode(osgText::Text::TEXT |
                             osgText::Text::ALIGNMENT);
    //Record the draw mode
   textOne->setPreviousDrawMode(textOne->getDrawMode());
   textOne->setDefaultDrawMode(textOne->getDrawMode());
   textOne->setAlignment(osgText::Text::CENTER_TOP);
   textOne->setFontResolution(64,64);
   textOne->setLabelType(external);

   //testing that we can have absolute position
   cout << "textOne position" << endl;
   cout << textOne->getPosition().x() << endl;
   cout << textOne->getPosition().y() << endl;
   cout << textOne->getPosition().z() << endl;

   cout << "textOne absolute position" << endl;
   cout << textOne->getAbsolutePosition().x() << endl;
   cout << textOne->getAbsolutePosition().y() << endl;
   cout << textOne->getAbsolutePosition().z() << endl;

   //Testing the distance between the label and the camera :
   cout << "Distance label-camera" << endl;
   cout << textOne->distanceCamera(viewer);

   return textOne;
}
