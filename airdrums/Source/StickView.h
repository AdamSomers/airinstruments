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

class StickView
{
public:
    StickView();
    ~StickView();
    void setup();
    void update();
    void draw();
    
    GLFrame objectFrame;

private:    
    int id;
    
    static const int numVerts = 36;
    void makePadMesh(M3DVector3f* inVerts, M3DVector3f* inNorms);
    GLBatch     padBatch;
    GLTriangleBatch stickBatch;
    float fade;
    Time fadeStartTime;
};

#endif /* defined(__AirBeats__StickView__) */
