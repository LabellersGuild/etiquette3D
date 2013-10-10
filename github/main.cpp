#include <osg/Group>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgSim/MultiSwitch>
#include "lgNode.h"
#include <osgGA/TrackballManipulator>

#include <osgText/Font>
#include <osgText/Text>

#include <fstream>

using namespace std;

bool addTextLabel(osg::Group* g, std::string nom_id, std::string nom);


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
     model = dynamic_cast<osg::Group*>
    (osgDB::readNodeFile("/home/tbrunel/Documents/Cartes3D-CityGML/Castle_Herten_v1.0.0/Castle_Herten_v1.0.0.citygml", options));

   // quit if we didn't successfully load the models
   if (! model )
   {
      std::cout << "could not load model" << std::endl;
      return -1;
   }

    // Find the node with its ID :
    // TODO : you can change the ID you are looking for
  lgNode findNode("ID_251-Schloss");
  model->accept(findNode);

    // Add this group node to the root
   root->addChild(model);

   osg::Node* rootModel = findNode.getFirst();

    // Add the label
    // To do : you can change the name of the label, it is the 3rd argument of the next line
   addTextLabel((osg::Group*)rootModel,rootModel->getName(),rootModel->getName());

   viewer.setSceneData( root );
   viewer.setCameraManipulator(new osgGA::TrackballManipulator());
   viewer.realize();

   while( !viewer.done() )
   {
      viewer.frame();
   }
}

bool addTextLabel(osg::Group* g, std::string name_id, std::string name)
{
   if (!g)
   {
       std::cout << "Error while creating the label" << std::endl;
      return false;
   }
   osg::Geode* textLabelGeode = new osg::Geode();
   osgText::Text* textOne = new osgText::Text();
   g->addChild(textLabelGeode);
   textLabelGeode->addDrawable(textOne);

   textOne->setCharacterSize(5);
   // TODO : change the path to the font
   textOne->setFont("/home/tbrunel/Téléchargements/OSG_data/OSG_data/fonts/arial.ttf");
   textOne->setText(name, osgText::String::ENCODING_UTF8 );
   textOne->setAxisAlignment(osgText::Text::SCREEN);
   textOne->setColor( osg::Vec4(192.0f/255.0f,0,0,1.0f) );
   textOne->setPosition( osg::Vec3(0,0,15) );
   textOne->setDrawMode(osgText::Text::TEXT |
                             osgText::Text::ALIGNMENT |
                                osgText::Text::BOUNDINGBOX);
   textOne->setAlignment(osgText::Text::CENTER_TOP);
   textOne->setFontResolution(64,64);

   return true;
}
