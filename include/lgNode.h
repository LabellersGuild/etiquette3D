/* 
 * File:   lgNode.h
 * Author: paulyves
 *
 * Created on October 22, 2013, 9:55 AM
 */

#ifndef LGNODE_H
#define	LGNODE_H
#include <osg/NodeVisitor>
#include <osg/Node>

#include <iostream>
#include <vector>

class lgNode : public osg::Node {
public:
    lgNode();
    lgNode(const lgNode& orig);
    virtual ~lgNode();
private:

};

#endif	/* LGNODE_H */

