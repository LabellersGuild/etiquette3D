#ifndef TEST_H
#define TEST_H

#include <osg/NodeVisitor>
#include <osg/Node>
#include "../include/lgNodeVisitor.h"
#include "../include/lgNodeOverseer.h"
#include "../include/lgLabel.h"
#include "../include/LGAnimation.h"
#include "../include/LGInteraction.h"
#include "../include/myLGAnimation.h"
#include "../include/myLGAnimation2.h"
#include <iostream>
#include <vector>

class test
{
    public:
        test();
        virtual ~test();
        lgNodeVisitor test_lgNodeVisitor_initialisation(osg::Group* model);
        void test_label_setLinkNode(osg::ref_ptr<osg::Node> theLinkNode, lgLabel* theLabel);
        void test_label_translateLabel(lgLabel* theLabel);
        void test_label_compute2dBox(osgViewer::Viewer* viewer, lgLabel* theLabel);
        void test_label_compute2dCenter(osgViewer::Viewer* viewer, lgLabel* theLabel);

    protected:

    private:
};

#endif // TEST_H
