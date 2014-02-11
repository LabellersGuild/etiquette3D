/** File: InternalLabelAnimation.cpp
 * Description : file that creates a sub-class of LGAnimation for internal labels.
 * Author : Thomas
 */

#include "../include/InternalLabelAnimation.h"
#include "../include/lgLabel.h"
#define PI 3.14159265359
#define GOOD 0.0
#define TOOFAR 10000.0

using namespace osg;
using namespace std;

/** ConstructorID_311-TheMagdalena
 * Warning : this animation has to be applied on internal labels (INTERNAL_TOP or INTERNAL_FACE)
 */
InternalLabelAnimation::InternalLabelAnimation(osgViewer::Viewer* viewer, ref_ptr<lgLabel> label) :lgAnimation(viewer)
{
    if( label->getLabelType() != INTERNAL_TOP && label->getLabelType() != INTERNAL_FACE)
    {
       cout << "Wrong type : "<< label->getLabelType() <<endl;
       return;
    }

    //label
    this->label = label;

     //Bounding box of the object
    this->bbox = label->computeObjectBBox(label->getUpdatedMatrix()->getParent(0),BoundingBox(0,0,0,0,0,0) );

    //Corners of the boundingbx, local position in lCorners:
    lCorners[0]=Vec3(bbox.xMin(),bbox.yMin(),bbox.zMin());
    lCorners[1]=Vec3(bbox.xMin(),bbox.yMin(),bbox.zMax());
    lCorners[2]=Vec3(bbox.xMin(),bbox.yMax(),bbox.zMin());
    lCorners[3]=Vec3(bbox.xMin(),bbox.yMax(),bbox.zMax());
    lCorners[4]=Vec3(bbox.xMax(),bbox.yMin(),bbox.zMin());
    lCorners[5]=Vec3(bbox.xMax(),bbox.yMin(),bbox.zMax());
    lCorners[6]=Vec3(bbox.xMax(),bbox.yMax(),bbox.zMin());
    lCorners[7]=Vec3(bbox.xMax(),bbox.yMax(),bbox.zMax());

    //Find the absolute coordinates of the corners
    Vec4 extendedlCorners[8];
    for(int i=0;i<8;i++)
        extendedlCorners[i]=Vec4(lCorners[i].x(),lCorners[i].y(),lCorners[i].z(),1);

    MatrixList matricesList = label->getUpdatedMatrix()->getParent(0)->getWorldMatrices();

    for (unsigned i=0;i<matricesList.size();i++)
    {
        for (int j=0;j<8;j++)
             extendedlCorners[j] = extendedlCorners[j]*matricesList[i];
    }

    //Absolute position in aCornerslocalEndFreePosition
    for (int i=0;i<8;i++)
        aCorners[i]= Vec3(extendedlCorners[i].x(), extendedlCorners[i].y(), extendedlCorners[i].z());

    //Planes of each face
    Plane planesFaces[5];
    planesFaces[0]= Plane(aCorners[3], aCorners[7], aCorners[6]);
    planesFaces[1]= Plane(aCorners[7], aCorners[5], aCorners[4]);
    planesFaces[2]= Plane(aCorners[5], aCorners[1], aCorners[0]);
    planesFaces[3]= Plane(aCorners[1], aCorners[3], aCorners[2]);
    planesFaces[4]= Plane(aCorners[1], aCorners[5], aCorners[7]);

    //Normals
    for (int i=0;i<5;i++)
        normals[i]=planesFaces[i].getNormal();

    //corners linked to each face,
    //face 1
    faceCorners[0][0] = 3;
    faceCorners[0][1] = 7;
    faceCorners[0][2] = 6;
    faceCorners[0][3] = 2;
    //face 2
    faceCorners[1][0] = 7;
    faceCorners[1][1] = 5;
    faceCorners[1][2] = 4;
    faceCorners[1][3] = 6;
    //face 3
    faceCorners[2][0] = 5;
    faceCorners[2][1] = 1;
    faceCorners[2][2] = 0;
    faceCorners[2][3] = 4;
    //face 4
    faceCorners[3][0] = 1;
    faceCorners[3][1] = 3;
    faceCorners[3][2] = 2;
    faceCorners[3][3] = 0;
    //face 5 (top one))
    faceCorners[4][0] = 7;
    faceCorners[4][1] = 3;
    faceCorners[4][2] = 1;
    faceCorners[4][3] = 5;
}

 /** Override of operator()
  */
void InternalLabelAnimation::operator()(Node* node, NodeVisitor* nv)
{
    lgAnimation::operator()(node, nv);

    enoughSpace(node, nv);

     //top of the bounding box
    if (lCorners[cornersSorted[chosenFace][0]].z() == lCorners[cornersSorted[chosenFace][1]].z() && lCorners[cornersSorted[chosenFace][1]].z() == lCorners[cornersSorted[chosenFace][2]].z() && lCorners[cornersSorted[chosenFace][2]].z() == lCorners[cornersSorted[chosenFace][3]].z())
    {
         //Change position of the label
          label->getUpdatedMatrix()->setMatrix(Matrixd::translate((lCorners[cornersSorted[chosenFace][0]]+lCorners[cornersSorted[chosenFace][1]]+lCorners[cornersSorted[chosenFace][2]]+lCorners[cornersSorted[chosenFace][3]])/4));

          label->setAxisAlignment(osgText::TextBase::XY_PLANE);

          float alpha=atan(cameraFaceVectors[chosenFace].y()/cameraFaceVectors[chosenFace].x())-PI/2;
          if (cameraFaceVectors[chosenFace].x()>0 && cameraFaceVectors[chosenFace].y()>0)
          {
              alpha -=PI;
          }
          if (cameraFaceVectors[chosenFace].x()>0 && cameraFaceVectors[chosenFace].y()<0)
          {
               alpha -=PI;
          }
          Matrix updatedMatrix = label->getUpdatedMatrix()->getMatrix();
          label->getUpdatedMatrix()->setMatrix(Matrixd::rotate(alpha,0,0,1)*updatedMatrix);
    }
    //Face
    else
    {
        //Change position of the label
      label->getUpdatedMatrix()->setMatrix(Matrixd::translate((localBeginningFreePosition+localEndFreePosition)/2));

      //Alignment of the label according to the face used.
      if (lCorners[cornersSorted[chosenFace][0]].x() == lCorners[cornersSorted[chosenFace][1]].x() && lCorners[cornersSorted[chosenFace][1]].x() == lCorners[cornersSorted[chosenFace][2]].x() && lCorners[cornersSorted[chosenFace][2]].x() == lCorners[cornersSorted[chosenFace][3]].x())
      {
          if (lCorners[cornersSorted[chosenFace][0]].x() == bbox.xMin())
              label->setAxisAlignment(osgText::TextBase::REVERSED_YZ_PLANE);
          else
              label->setAxisAlignment(osgText::TextBase::YZ_PLANE);
      }
      else if (lCorners[cornersSorted[chosenFace][0]].y() ==lCorners[cornersSorted[chosenFace][1]].y() && lCorners[cornersSorted[chosenFace][1]].y() == lCorners[cornersSorted[chosenFace][2]].y() && lCorners[cornersSorted[chosenFace][2]].y() == lCorners[cornersSorted[chosenFace][3]].y())
      {
          if ( lCorners[cornersSorted[chosenFace][0]].y() == bbox.yMin())
              label->setAxisAlignment(osgText::TextBase::XZ_PLANE);
          else
              label->setAxisAlignment(osgText::TextBase::REVERSED_XZ_PLANE);
      }
      else
      {
          cerr << "error with the faces definition of the bounding box an object linked to a label. The faces are not on (XY), (YZ) ot (XY) planes." << endl;
      }
    }
    //Allow OSG to continue the node traversal
    traverse(node, nv);
}

bool InternalLabelAnimation::enoughSpace(Node* node, NodeVisitor* nv)
{
    // True if there is enough space
    bool space=false;

    // We want the label to be :
    // - on a face with the best orientation to the camera
    // - where there is space
    // - high on a face (the higher the label, the more we are likely to see it)
    //Position of the camera in the absolute frame of reference
    Matrix matrixCamera = view->getCamera()->getInverseViewMatrix();
    Vec3 positionCamera= Vec3(matrixCamera(3,0), matrixCamera(3,1), matrixCamera(3,2));

    //Vectors Camera-faces :
    cameraFaceVectors.clear();
    for (int i=0;i<5;i++)
    {
      cameraFaceVectors.push_back(Vec3(positionCamera-(aCorners[faceCorners[i][0]]+aCorners[faceCorners[i][1]]+aCorners[faceCorners[i][2]]+aCorners[faceCorners[i][3]])/4));
    }
    //Orientations
    vector<double> orientations;
    for(int i=0;i<5;i++)
    {
      orientations.push_back(acos((cameraFaceVectors[i]*normals[i])/(cameraFaceVectors[i].length()*normals[i].length())));
    }

    //Sorted Orientations
    vector<double> sortedOrientations;

    //Sort the corners of the faces
    for(int i=0;i<5;i++ )
    {
        double minOrientation=3.16; //rad
        int index=0;
        for (int j=0;j<5;j++)
        {
            if (orientations[j]<=minOrientation)
            {
                minOrientation = orientations[j];
                index=j;
            }
        }
        for (int j=0;j<4;j++)
            cornersSorted[i][j]=faceCorners[index][j];

        sortedOrientations.push_back(orientations[index]);
        orientations[index]=3.17;
    }

    //we divide the face into small portions that are represented by points.
    const int iMax = 10; //Number of points per line
    const int jMax = 10; //Number of points per column

    int freePoints=0;
    int maxFreePoints=0;
    Vec3 beginningFreePosition;
    Vec3 endFreePosition;
    int candidatei=0;
    int candidatej=0;

    //Calculate the occlusions
    // Find the line which have the more consecutive free points. If there are several lines with this maximum, take the first one. The higher the label, the better
    // We could use different criteria.
    for (int k=0;k<5;k++)
    {
        //top of the bounding box
        if (lCorners[cornersSorted[k][0]].z() == lCorners[cornersSorted[k][1]].z() && lCorners[cornersSorted[k][1]].z() == lCorners[cornersSorted[k][2]].z() && lCorners[cornersSorted[k][2]].z() == lCorners[cornersSorted[k][3]].z())
        {
            if (sortedOrientations[k]<PI/3.0 && cameraFaceVectors[k].length() < TOOFAR)
            {
              space = true;
              chosenFace = k;
            }
        }
        else
        {
            for (int i=0;i<iMax;i++)
            {
                freePoints=0;
                for (int j=0;j<jMax;j++)
                {
                    Vec3 currentPoint = ((aCorners[cornersSorted[k][0]]) * (iMax-i)*(jMax-j) + (aCorners[cornersSorted[k][1]]) * (iMax-i)*j + (aCorners[cornersSorted[k][2]]) * i*(jMax-j) + (aCorners[cornersSorted[k][3]])*i*j)/(iMax*jMax);
                    osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(positionCamera,currentPoint);
                    osgUtil::IntersectionVisitor iv( intersector.get() );
                    iv.setTraversalMask( ~0x1 );
                    view->getCamera()->accept( iv );
                    //If there is nothing, or the label
                    if ( !(intersector->containsIntersections()) || (intersector->containsIntersections() && (*(intersector->getIntersections().begin())).drawable == label))
                    {
                        freePoints++;
                        //cornersSorted
                        if(j==jMax-1 && maxFreePoints<freePoints)
                        {
                            maxFreePoints=freePoints;
                            candidatei=i;
                            candidatej=j;
                        }
                    }
                    else
                    {
                        if (freePoints>0 && maxFreePoints<freePoints)
                        {
                            maxFreePoints=freePoints;
                            candidatei=i;
                            candidatej=j-1;
                        }
                        freePoints=0;
                        if((jMax-j)<=maxFreePoints){
                            break;
                        }
                    }
                }
                //beginningFreePosition = ((aCorners[cornersSorted[k][0]])*(iMax-candidatei)*(jMax-candidatej+maxFreePoints-1) + (aCorners[cornersSorted[k][1]])*(iMax-candidatei)*(candidatej-maxFreePoints+1) + (aCorners[cornersSorted[k][2]])*candidatei*(jMax-candidatej+maxFreePoints-1) + (aCorners[cornersSorted[k][3]])*candidatei*(candidatej-maxFreePoints+1))/(iMax*jMax);
                //endFreePosition =  ((aCorners[cornersSorted[k][0]]) * (iMax-candidatei)*(jMax-candidatej) + (aCorners[cornersSorted[k][1]]) * (iMax-candidatei)*candidatej + (aCorners[cornersSorted[k][2]]) * candidatei*(jMax-candidatej) + (aCorners[cornersSorted[k][3]])*candidatei*candidatej)/(iMax*jMax);

                localBeginningFreePosition = ((lCorners[cornersSorted[k][0]])*(iMax-candidatei)*(jMax-candidatej+maxFreePoints-1) + (lCorners[cornersSorted[k][1]])*(iMax-candidatei)*(candidatej-maxFreePoints+1) + (lCorners[cornersSorted[k][2]])*candidatei*(jMax-candidatej+maxFreePoints-1) + (lCorners[cornersSorted[k][3]])*candidatei*(candidatej-maxFreePoints+1))/(iMax*jMax);
                localEndFreePosition = ((lCorners[cornersSorted[k][0]]) * (iMax-candidatei)*(jMax-candidatej) + (lCorners[cornersSorted[k][1]]) * (iMax-candidatei)*candidatej + (lCorners[cornersSorted[k][2]]) * candidatei*(jMax-candidatej) + (lCorners[cornersSorted[k][3]])*candidatei*candidatej)/(iMax*jMax);

                  // Adapt the size of the label box according to the space we have
                  double space3D = sqrt(pow(localBeginningFreePosition.x()-localEndFreePosition.x(),2)+pow(localBeginningFreePosition.y()-localEndFreePosition.y(),2));

                  double spaceLabel = label->getCharacterHeight()*label->getText().size()*0.6;

                  if (spaceLabel<space3D && sortedOrientations[k]<PI/3.0 && cameraFaceVectors[k].length() < TOOFAR)
                  {
                    space = true;
                    chosenFace = k;
                  }
            }
        }
        if (space == true)
        {
          break;
        }
    }
    return space;
}

/**
 * operator used for InExSwitch
 */
void InternalLabelAnimation::operatorForSwitch(Node* node, NodeVisitor* nv)
{
     //top of the bounding box
    if (lCorners[cornersSorted[chosenFace][0]].z() == lCorners[cornersSorted[chosenFace][1]].z() && lCorners[cornersSorted[chosenFace][1]].z() == lCorners[cornersSorted[chosenFace][2]].z() && lCorners[cornersSorted[chosenFace][2]].z() == lCorners[cornersSorted[chosenFace][3]].z())
    {
         //Change position of the label
          label->getUpdatedMatrix()->setMatrix(Matrixd::translate((lCorners[cornersSorted[chosenFace][0]]+lCorners[cornersSorted[chosenFace][1]]+lCorners[cornersSorted[chosenFace][2]]+lCorners[cornersSorted[chosenFace][3]])/4));

          label->setAxisAlignment(osgText::TextBase::XY_PLANE);

          float alpha=atan(cameraFaceVectors[chosenFace].y()/cameraFaceVectors[chosenFace].x())-PI/2;
          if (cameraFaceVectors[chosenFace].x()>0 && cameraFaceVectors[chosenFace].y()>0)
          {
              alpha -=PI;
          }
          if (cameraFaceVectors[chosenFace].x()>0 && cameraFaceVectors[chosenFace].y()<0)
          {
               alpha -=PI;
          }
          Matrix updatedMatrix = label->getUpdatedMatrix()->getMatrix();
          label->getUpdatedMatrix()->setMatrix(Matrixd::rotate(alpha,0,0,1)*updatedMatrix);

    }
    //Face
    else
    {
        //Change position of the label
      label->getUpdatedMatrix()->setMatrix(Matrixd::translate((localBeginningFreePosition+localEndFreePosition)/2));

      //Alignment of the label according to the face used.
      if (lCorners[cornersSorted[chosenFace][0]].x() == lCorners[cornersSorted[chosenFace][1]].x() && lCorners[cornersSorted[chosenFace][1]].x() == lCorners[cornersSorted[chosenFace][2]].x() && lCorners[cornersSorted[chosenFace][2]].x() == lCorners[cornersSorted[chosenFace][3]].x())
      {
          if (lCorners[cornersSorted[chosenFace][0]].x() == bbox.xMin())
              label->setAxisAlignment(osgText::TextBase::REVERSED_YZ_PLANE);
          else
              label->setAxisAlignment(osgText::TextBase::YZ_PLANE);
      }
      else if (lCorners[cornersSorted[chosenFace][0]].y() ==lCorners[cornersSorted[chosenFace][1]].y() && lCorners[cornersSorted[chosenFace][1]].y() == lCorners[cornersSorted[chosenFace][2]].y() && lCorners[cornersSorted[chosenFace][2]].y() == lCorners[cornersSorted[chosenFace][3]].y())
      {
          if ( lCorners[cornersSorted[chosenFace][0]].y() == bbox.yMin())
              label->setAxisAlignment(osgText::TextBase::XZ_PLANE);
          else
              label->setAxisAlignment(osgText::TextBase::REVERSED_XZ_PLANE);
      }
      else
      {
          cerr << "error with the faces definition of the bounding box an object linked to a label. The faces are not on (XY), (YZ) ot (XY) planes." << endl;
      }
    }
}
