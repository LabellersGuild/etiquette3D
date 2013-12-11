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
#include "../include/testLGAnimation.h"
#include <osgGA/TrackballManipulator>

#include <osgText/Font>
#include <osgText/Text>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <osg/BlendFunc>

using namespace std;
using namespace osg;

lgLabel* addTextLabel(ref_ptr<Node> linkToNode, string nom_id, string nom, Vec3 recoPos, ref_ptr<osgViewer::Viewer> viewer, lgType type);
void addMoreLabel(lgNodeVisitor visitor, vector<lgLabel*> listLabels, Group* model, osgViewer::Viewer* viewer);
/// I don't use a makefile, but only Code::Blocks.
/// To add the library dependencies : right clic on project, build options,
/// linker settings, add every .so file in the /lib directory of OpenSceneGraph
/// Then, to use the program, the building is done by codeblocks, but launch it with a console.
/// The program is in /bin/Debug of the project

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
    string pathFile = "/home/paulyves/OpenSceneGraph-Data/Munich_v_1_0_0.citygml";
    model = dynamic_cast<osg::Group*> (osgDB::readNodeFile(pathFile, options));

   // quit if we didn't successfully load the model
   if (! model )
   {
      std::cout << "could not load model" << std::endl;
      return -1;
   }


    // Add the model group node to the root
    root->addChild(model);


    // Find a node
    test myTest=test();
    lgNodeVisitor findNode=myTest.test_lgNodeVisitor_initialisation(model);
    Vec3 positionCalc = findNode.recommendedCoordinates();

    cout << findNode.getNodeList().size() << endl;
    ref_ptr<Node> rootModel = findNode.getFirst();

    // Add the label
    // You can change the name of the label, it is the 3rd argument of the next line

    vector<lgLabel*> listLabels = vector<lgLabel*>();
    ref_ptr<lgLabel> label1 = addTextLabel(rootModel, rootModel->getName(), rootModel->getName(), positionCalc, &viewer, INTERNAL_FACE);

    //Tests of the hiding distance, and setSeeInTransparency
    label1->setHidingDistance(1000);

    //Test of setSeeInTransparency :
    //label1->setSeeInTransparency(true);

    //second label
    lgNodeVisitor findNode2 = myTest.test_lgNodeVisitor_initialisation(model);
    ref_ptr<Node> rootModel2 = findNode2.getFirst();
    Vec3 positionCalc2 = findNode2.recommendedCoordinates();
    ref_ptr<lgLabel> label2 = addTextLabel(rootModel2, rootModel2->getName(), rootModel2->getName(), positionCalc2, &viewer, EXTERNAL);
    listLabels.push_back(label2.get());

    //Test of setTransparency :
    label2->setTransparency(0.8);

    //Indo label
    label1->setInfoLabel("Castle constructed in the XIV th century");

    //more label
    //addMoreLabel(findNode,listLabels,model,&viewer);

    // Create LGInteraction

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


    //Translate label test:
    myTest.test_label_translateLabel(label1);
    std::clock_t start;
    double timer;
    float fps;
    while (!viewer.done()) {
        //a2dBox = label1->distance2dBox(pView,label2);

        //cout<<"distance 2d "<<a2dBox<<endl;

        //Some tests :
        //myTest.test_label_compute2dBox(&viewer, label1);
        //myTest.test_label_compute2dCenter(&viewer, label1);

        //fps calc
        timer = (clock() - start) / (double) CLOCKS_PER_SEC;
        start = clock();
        fps = 1/timer;
        cout<<"fps :"<<fps<<endl;
        //cout<<"distance 2d "<<a2dBox<<endl;
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

   if (type == EXTERNAL)
   {
       // Test of the LGAnimation : use of testLGAnimation class
       //ref_ptr<testLGAnimation> animation = new testLGAnimation(viewer);
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
   test myTest=test();
   myTest.test_label_setLinkNode(linkToNode,label);

   label->setCharacterSize(5);
   label->setText(name, osgText::String::ENCODING_UTF8 );

   label->setColor( Vec4(192.0f/255.0f,0,0,1.0f) );
   label->setDrawMode(osgText::Text::TEXT | osgText::Text::ALIGNMENT);

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
   cout << "Distance label-camera : " << label->distanceCamera(viewer) << endl;

   return label;
}

/**
 * Use this function if you are lazy and just want to add labels, remember to change the idNode(s)
 * @param visitor a lgNodeVisitor
 * @param listLabels the list of Labels for further interactions
 * @param model the model of the citygml file translated in OSG
 * @param viewer the viewer
 */
void addMoreLabel(lgNodeVisitor visitor, vector<lgLabel*> listLabels, Group* model, osgViewer::Viewer* viewer){
    string idNode = "ID_276003000000992_26";
    visitor.setNameToFind(idNode);
    model->accept(visitor);
    Node* aNode = visitor.getFirst();
    visitor.feedFoundPointList(*(aNode));
    listLabels.push_back(addTextLabel(visitor.getFirst(),aNode->getName(),aNode->getName(),visitor.recommendedCoordinates(), viewer, EXTERNAL));

    idNode = "ID_276003000000992_27";
    visitor.setNameToFind(idNode);
    model->accept(visitor);
    aNode = visitor.getFirst();
    visitor.feedFoundPointList(*(aNode));
    listLabels.push_back(addTextLabel(visitor.getFirst(),aNode->getName(),aNode->getName(),visitor.recommendedCoordinates(), viewer, EXTERNAL));

    idNode = "ID_276003000005856_87";
    visitor.setNameToFind(idNode);
    model->accept(visitor);
    aNode = visitor.getFirst();
    visitor.feedFoundPointList(*(aNode));
    listLabels.push_back(addTextLabel(visitor.getFirst(),aNode->getName(),aNode->getName(),visitor.recommendedCoordinates(), viewer, EXTERNAL));

    idNode = "ID_276003000005856_45";
    visitor.setNameToFind(idNode);
    model->accept(visitor);
    aNode = visitor.getFirst();
    visitor.feedFoundPointList(*(aNode));
    listLabels.push_back(addTextLabel(visitor.getFirst(),aNode->getName(),aNode->getName(),visitor.recommendedCoordinates(), viewer, EXTERNAL));

    idNode = "ID_276003000005856_71";
    visitor.setNameToFind(idNode);
    model->accept(visitor);
    aNode = visitor.getFirst();
    visitor.feedFoundPointList(*(aNode));
    listLabels.push_back(addTextLabel(visitor.getFirst(),aNode->getName(),aNode->getName(),visitor.recommendedCoordinates(), viewer, EXTERNAL));

    idNode = "ID_276003000001194";
    visitor.setNameToFind(idNode);
    model->accept(visitor);
    aNode = visitor.getFirst();
    visitor.feedFoundPointList(*(aNode));
    listLabels.push_back(addTextLabel(visitor.getFirst(),aNode->getName(),aNode->getName(),visitor.recommendedCoordinates(), viewer, EXTERNAL));


    string nodeName="ID_276003000001194_";
    char numstr[21];
    for (int j=1;j<19;j++){
        sprintf(numstr, "%d", j);
        idNode=nodeName+numstr;
        visitor.setNameToFind(idNode);
        model->accept(visitor);
        aNode = visitor.getFirst();
        visitor.feedFoundPointList(*(aNode));
        listLabels.push_back(addTextLabel(visitor.getFirst(),aNode->getName(),aNode->getName(),visitor.recommendedCoordinates(), viewer, EXTERNAL));
    }
}
