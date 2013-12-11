/** File : test.h
 * Description : Test file for our library
 */

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
        test(){};
        virtual ~test(){};

        /**
         * This test is used to set a label (with idNode as input) and
         * see if we can recommand a position
         * @param model the model containing the citygml as OSG
         * @return findNode : the nodeVisitor fed with the vertex of child drawables and recommended position
         */
        lgNodeVisitor test_lgNodeVisitor_initialisation(osg::Group* model);

        /**
         * Function used to check that we got the proper linkNode
         * @param theLinkNode the Node father of the label
         * @param theLabel the label we want to link
         */
        void test_label_setLinkNode(osg::ref_ptr<osg::Node> theLinkNode, lgLabel* theLabel);

        /**
         * Test if translation of the label works
         * @param theLabel : the label we want to translate
         */
        void test_label_translateLabel(lgLabel* theLabel);

        /**
         * Test if the 2D bounding box computed for the label is correct
         * @param viewer : osgViewer::Viewer* : the viewer object of the main file
         * @param theLabel : lgLabel* : label
         */
        void test_label_compute2dBox(osgViewer::Viewer* viewer, lgLabel* theLabel);

        /**
         * Test if the 2D center calculated for the label is correct
         * @param viewer : osgViewer::Viewer* : the viewer object of the main file
         * @param theLabel : lgLabel* : label
         */
        void test_label_compute2dCenter(osgViewer::Viewer* viewer, lgLabel* theLabel);
};

#endif // TEST_H
