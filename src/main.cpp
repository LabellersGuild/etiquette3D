#include <osg/Group>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgSim/MultiSwitch>
#include "../include/lgNodeVisitor.h"
#include "../include/lgNodeOverseer.h"
#include "../include/lgNode.h"
#include "../include/lgLabel.h"
#include "../include/LGAnimation.h"
#include "../include/LGInteraction.h"
#include "../include/myLGAnimation.h"
#include <osgGA/TrackballManipulator>

#include <osgText/Font>
#include <osgText/Text>
#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

lgLabel* addTextLabel(osg::Node* g, std::string nom_id, std::string nom, osg::Vec3 recoPos, osgViewer::Viewer* viewer);


/// I don't use a makefile, but only Code::Blocks.
/// To add the library dependencies : right clic on project, build options,
/// linker settings, add every .so file in the /lib directory of OpenSceneGraph
/// Then, to use the program, the building is done by codeblocks, but a launch it with a console.
    /// The program is in /bin/Debug of the project
int main()
{
   // Declare a group for the root of our tree and a group for the model
   osg::Group* root = new osg::Group();
   osg::Group* model = NULL;
   osgViewer::Viewer viewer;

   // Load the model
   // TODO : change the file name
   // If you want to see the names of the nodes, write "names" instead of "" in the next line.
    osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options("");
    //    erreur lors de l'input d'une variable string
    cout << "Entrer le path du fichier .citygml :" << endl;
    string pathFile;
    cin >> pathFile;
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
    lgNodeVisitor findNode(idNode);
    model->accept(findNode);

    findNode.feedFoundPointList(*(findNode.getFirst()));
    std::vector<osg::Vec3> points = findNode.getFoundPointList();
    std::cout << "On a trouvé un liste de " << points.size() << " points" << std::endl;

    osg::Vec3 positionCalc = findNode.recommendedCoordinates();
    std::cout << "Le point recommandé est : x:" << positionCalc.x() << " y=" << positionCalc.y() << " z=" << positionCalc.z() << std::endl;

    // Add this group node to the root
    root->addChild(model);
    cout << findNode.getNodeList().size() << endl;
    osg::Node* rootModel = findNode.getFirst();
    // Add the label
    // To do : you can change the name of the label, it is the 3rd argument of the next line
    vector<osgText::Text*> listLabels = vector<osgText::Text*>();
    lgLabel* textOne = addTextLabel(rootModel, rootModel->getName(), rootModel->getName(), positionCalc, &viewer);

    // Create LGInteraction
    listLabels.push_back(textOne);
    osg::ref_ptr<LGInteraction> interaction = new LGInteraction(listLabels);
    viewer.addEventHandler(interaction.get());

    viewer.setSceneData(root);
    viewer.setCameraManipulator(new osgGA::TrackballManipulator());
    viewer.realize();

    while (!viewer.done()) {
        viewer.frame();
    }
}

lgLabel* addTextLabel(osg::Node* g, std::string name_id, std::string name, osg::Vec3 recoPos, osgViewer::Viewer* viewer)
{
   if (!g)
   {
       std::cout << "Error while creating the label" << std::endl;
      return NULL;
   }
   lgLabel* textOne = new lgLabel();

   osg::ref_ptr<osg::Node> linkToNode = g;
   textOne->setLinkNode(linkToNode, viewer);
   textOne->setPositionInit(recoPos);
   textOne->setCharacterSize(5);
   // TODO : change the path to the font
   //textOne->setFont("/home/tbrunel/T�l�chargements/OSG_data/OSG_data/fonts/arial.ttf");
   textOne->setText(name, osgText::String::ENCODING_UTF8 );
   textOne->setAxisAlignment(osgText::Text::SCREEN);
   textOne->setColor( osg::Vec4(192.0f/255.0f,0,0,1.0f) );
   textOne->setDrawMode(osgText::Text::TEXT |
                             osgText::Text::ALIGNMENT |
                                osgText::Text::BOUNDINGBOX);
   textOne->setAlignment(osgText::Text::CENTER_TOP);
   textOne->setFontResolution(64,64);

   //testing that we can have absolute position
   cout << "textOne position" << endl;
   cout << textOne->getPosition().x() << endl;
   cout << textOne->getPosition().y() << endl;
   cout << textOne->getPosition().z() << endl;

   cout << "textOne absolute position" << endl;
   cout << textOne->getAbsolutePosition().x() << endl;
   cout << textOne->getAbsolutePosition().y() << endl;
   cout << textOne->getAbsolutePosition().z() << endl;
   return textOne;
}
