#ifndef LGInteraction_H_INCLUDED
#define LGInteraction_H_INCLUDED

#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgUtil/LineSegmentIntersector>
#include <osgText/Text>

class LGInteraction : public osgGA::GUIEventHandler
{
    public:
        virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
        LGInteraction(std::vector<osgText::Text*> l) : listLabels(l) {};
    protected:
        std::vector<osgText::Text*> selectedLabels;
        std::vector<osgText::Text*> listLabels;
};

#endif // LGInteraction_H_INCLUDED
