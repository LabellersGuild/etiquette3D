#include <osg/Group>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgSim/MultiSwitch>
#include "../include/lgNodeVisitor.h"
#include "../include/lgNodeOverseer.h"
#include "../include/lgNode.h"
#include "../include/lgLabel.h"
#include <osgGA/TrackballManipulator>

#include <osgText/Font>
#include <osgText/Text>
#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

bool addTextLabel(osg::Geode* g, std::string nom_id, std::string nom);


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
    // Add this group node to the root
    root->addChild(model);
    cout << findNode.getNodeList().size() << endl;
    osg::Node* rootModel = findNode.getFirst();
    osg::ref_ptr<osg::Node> rootNode = (osg::Node*) rootModel;
    osg::Group* groupe = (osg::Group*) rootModel;
    rootNode = (osg::Node*) groupe->getChild(0);
    std::cout << "le node trouvé a " << groupe->getNumChildren() << " noeuds fils" <<endl;
    bool fin = false;
    while (fin == false){
        groupe = (osg::Group*) rootNode.get();
        std::cout << "le node fils a " << groupe->getNumChildren() << " noeuds fils" <<endl;
        if(groupe->getNumChildren()<1){
            fin = true;
        } else {
            rootNode = (osg::Node*) groupe->getChild(0); 
        }
    }
    osg::Geode* myGeode = (osg::Geode*) rootNode.get();
    if(myGeode){
        // Add the label
        // To do : you can change the name of the label, it is the 3rd argument of the next line
       addTextLabel(myGeode,rootModel->getName(),rootModel->getName());
    }
   viewer.setSceneData( root );
   viewer.setCameraManipulator(new osgGA::TrackballManipulator());
   viewer.realize();

   while( !viewer.done() )
   {
      viewer.frame();
   }
}

bool addTextLabel(osg::Geode* g, std::string name_id, std::string name)
{
   if (!g)
   {
       std::cout << "Error while creating the label" << std::endl;
      return false;
   }
   osg::Geode* textLabelGeode = new osg::Geode();
   lgLabel* textOne = new lgLabel();
   
   //we get the first drawable of the node
   osg::Geometry* myGeom = (osg::Geometry*) g->getDrawable(0);
   //le vertex array est le tableau des sommets de la géométrie cible
   osg::Vec3Array* arrayVertex = (osg::Vec3Array*) myGeom->getVertexArray();
   
   g->addDrawable(textOne);
   textOne->setCharacterSize(5);
   // TODO : change the path to the font
   //textOne->setFont("/home/tbrunel/T�l�chargements/OSG_data/OSG_data/fonts/arial.ttf");
   textOne->setText(name, osgText::String::ENCODING_UTF8 );
   textOne->setAxisAlignment(osgText::Text::SCREEN);
   textOne->setColor( osg::Vec4(192.0f/255.0f,0,0,1.0f) );
   textOne->setPosition( osg::Vec3(arrayVertex->at(0).x(),arrayVertex->at(0).y(),arrayVertex->at(0).z()+25) );
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
   textOne->setLinkNode(g);
   cout << "textOne absolute position" << endl;
   cout << textOne->getAbsolutePosition().x() << endl;
   cout << textOne->getAbsolutePosition().y() << endl;
   cout << textOne->getAbsolutePosition().z() << endl;
   return true;
}
