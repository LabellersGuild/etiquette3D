#include <algorithm>
#include <osg/Geode>
#include <osg/Geometry>
#include "../include/lgNodeVisitor.h"

using namespace std;
using namespace osg;

// Default constructor - initialize the name to ""
lgNodeVisitor::lgNodeVisitor() : NodeVisitor(TRAVERSE_ALL_CHILDREN),
                                        searchForName()
{
}

// Constructor that accepts string argument
// Initializes the name of the node
lgNodeVisitor::lgNodeVisitor(const string &name) :
                                   NodeVisitor(TRAVERSE_ALL_CHILDREN),
                                   searchForName(name)
{
}

 //The 'apply' method for 'node' type instances.
 //Compare the 'searchForName' data member against the node's name.
 //If the strings match, add this node to our list
void lgNodeVisitor::apply(Node &searchNode)
{
    // If no node is found, return searchNode
    if (searchForName == "")
    {
        foundNodeList.push_back(&searchNode);
    }
    else
    {
       if (searchNode.getName() == searchForName) {
            foundNodeList.push_back(&searchNode);
        }
        traverse(searchNode);
    }
}

// Set the searchForName to user-defined string
void lgNodeVisitor::setNameToFind(const string &searchName)
{
   searchForName = searchName;
   foundNodeList.clear();
}

Node* lgNodeVisitor::getFirst() {
    return (foundNodeList.at(0));
}

Node* lgNodeVisitor::getLast() {
    int taille = foundNodeList.size();
    return (foundNodeList.at(taille-1));
}

/*
 *This method recursively feed the list of points with the
 * members of the vertex arrays inside the Geode children of the first targetNode
 * The first target node should be the found node's value : *(findNode.getFirst())
 * warning, using this method before calling an instance of this class through the
 * accept method of a Group will probably lead to errors
 */
void lgNodeVisitor::feedFoundPointList(Node& targetNode) {
    if(foundNodeList.size()>0){
        ref_ptr<Group> targetGroup = dynamic_cast<Group*>(&targetNode);
        ref_ptr<Geode> targetGeode = dynamic_cast<Geode*>(&targetNode);
        if (targetGroup && targetGroup->getNumChildren()>0){
            for(unsigned i = 0;i<(targetGroup->getNumChildren()); i++){
                Node & sourceNode = *(targetGroup->getChild(i));
                this->feedFoundPointList(sourceNode);
            }

        }
        if (targetGeode && targetGeode->getNumDrawables()>0) {
            for (unsigned i=0;i<targetGeode->getNumDrawables();i++){
                //we use dynamic_cast to ensure that we will get true Geometry
                //for example, lgLabel won't be counted as Geometry
                ref_ptr<Geometry> aGeom = dynamic_cast<Geometry*>(targetGeode->getDrawable(i));
                if (aGeom){
                    ref_ptr<Vec3Array> arrayVertex = (Vec3Array*) aGeom->getVertexArray();
                    int size = arrayVertex->size();
                    for (int j=0; j<size;j++){
                        foundPointList.push_back(arrayVertex->at(j));
                    }
                }
            }
        }
    }
}

std::vector<Vec3> lgNodeVisitor::getFoundPointList(){
    return foundPointList;
}

/*
 *Calculate a Vec3 made from the foundPointList points that the recommend
 * as the positionInit of a label linked to foundNode
 */
Vec3 lgNodeVisitor::recommendedCoordinates(){
    Vec3 coordinates;
    coordinates.set(0,0,0);
    int listSize = foundPointList.size();
    if(listSize>0){
        int sumX=0, sumY=0, maxZ=0;
        for (int i = 0; i < listSize; i++){
            sumX += foundPointList.at(i).x();
            sumY += foundPointList.at(i).y();
            if(maxZ<foundPointList.at(i).z()){
                maxZ = foundPointList.at(i).z();
            }
        }
        coordinates.set(sumX/listSize, sumY/listSize, maxZ+10);
    }
    return coordinates;
}
