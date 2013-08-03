//
//  StickView.h
//  AirBeats
//
//  Created by Adam Somers on 7/24/13.
//
//

#ifndef __AirBeats__StickView__
#define __AirBeats__StickView__

#include "GfxTools.h"
#include "Pointer3d.h"
#include "ShadowView.h"

class StickView : public Pointer3d
{
public:
    StickView();
    ~StickView();
    void setup();
    void update();
    void draw();

    void setOrigin(float x, float y, float z);
    
    int pointableId;
    int handId;

private:
    void calcCollisionPoint(M3DVector3f collisionPoint);
    float calcStickDistance();
    int id;
    
    static const int numVerts = 36;
    void makePadMesh(M3DVector3f* inVerts, M3DVector3f* inNorms);
    GLBatch     padBatch;
    GLTriangleBatch stickBatch;
    float fade;
    Time fadeStartTime;
    ShadowView shadow;
};

#endif /* defined(__AirBeats__StickView__) */
