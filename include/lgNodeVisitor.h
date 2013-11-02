#ifndef LGNODE_H_INCLUDED
#define LGNODE_H_INCLUDED

#include <osg/NodeVisitor>
#include <osg/Node>

#include <iostream>
#include <vector>

class lgNodeVisitor : public osg::NodeVisitor {
public:

   lgNodeVisitor();

   lgNodeVisitor(const std::string &name) ;

   virtual void apply(osg::Node &searchNode);

   void setNameToFind(const std::string &name);

   osg::Node* getFirst();
   osg::Node* getLast();

   typedef std::vector<osg::Node*> nodeListType;

   nodeListType& getNodeList() { return foundNodeList; }

private:

   std::string searchForName;
   nodeListType foundNodeList;

};

#endif // LGNODE_H_INCLUDED
