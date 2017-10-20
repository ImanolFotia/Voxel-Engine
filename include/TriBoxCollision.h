#pragma once

#include <glm/glm.hpp>

#define X 0

#define Y 1

#define Z 2
/*======================== X-tests ========================*/

#define AXISTEST_X01(a, b, fa, fb)			   \
                                p0 = a*v0[Y] - b*v0[Z];\
p2 = a*v2[Y] - b*v2[Z];\
if(p0<p2) {min=p0;max=p2;} else {min=p2;max=p0;} \
rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];\
if(min>rad || max<-rad) return 0;


#define AXISTEST_X2(a, b, fa, fb)			   \
p0 = a*v0[Y] - b*v0[Z];\
p1 = a*v1[Y] - b*v1[Z];\
if(p0<p1) {min=p0;max=p1;} else {min=p1;max=p0;} \
rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];\
if(min>rad || max<-rad) return 0;



/*======================== Y-tests ========================*/

#define AXISTEST_Y02(a, b, fa, fb)			   \
p0 = -a*v0[X] + b*v0[Z];\
p2 = -a*v2[X] + b*v2[Z];\
if(p0<p2) {min=p0;max=p2;} else {min=p2;max=p0;} \
rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];\
if(min>rad || max<-rad) return 0;


#define AXISTEST_Y1(a, b, fa, fb)			   \
p0 = -a*v0[X] + b*v0[Z];\
p1 = -a*v1[X] + b*v1[Z];\
if(p0<p1) {    min=p0;max=p1;} else {    min=p1;    max=p0;} \
rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];\
if(min>rad || max<-rad) return 0;



/*======================== Z-tests ========================*/



#define AXISTEST_Z12(a, b, fa, fb)			   \
p1 = a*v1[X] - b*v1[Y];\
p2 = a*v2[X] - b*v2[Y];\
if(p2<p1) {    min=p2;    max=p1;} else {    min=p1;    max=p2;} \
rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];\
if(min>rad || max<-rad) return 0;



#define AXISTEST_Z0(a, b, fa, fb)			   \
p0 = a*v0[X] - b*v0[Y];\
p1 = a*v1[X] - b*v1[Y];\
if(p0<p1) {    min=p0;    max=p1;} else {    min=p1;    max=p0;} \
rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];\
if(min>rad || max<-rad) return 0;

#define FINDMINMAX(x0,x1,x2,min,max) \
  min = max = x0;   \
  if(x1<min) min=x1;\
  if(x1>max) max=x1;\
  if(x2<min) min=x2;\
  if(x2>max) max=x2;

static int planeBoxOverlap(glm::vec3 normal, glm::vec3 vert, glm::vec3 maxbox)	// -NJMP-

{

  int q;

  float vmin[3],vmax[3],v;

  float mNormal[3] = {normal.x, normal.y, normal.z};
  float mVert[3] = {vert.x, vert.y, vert.z};
  float mMaxBox[3] = {maxbox.x, maxbox.y, maxbox.z};

  for(q=X;q<=Z;q++)

  {

    v=mVert[q];					// -NJMP-

    if(mNormal[q]>0.0f)

    {

      vmin[q]=-mMaxBox[q] - v;	// -NJMP-

      vmax[q]= mMaxBox[q] - v;	// -NJMP-

    }

    else

    {

      vmin[q]= mMaxBox[q] - v;	// -NJMP-

      vmax[q]=-mMaxBox[q] - v;	// -NJMP-

    }

  }

  if(glm::dot(glm::vec3(mNormal[0], mNormal[1], mNormal[2]),glm::vec3(vmin[0], vmin[1], vmin[2]))>0.0f) return 0;	// -NJMP-

  if(glm::dot(glm::vec3(mNormal[0], mNormal[1], mNormal[2]), glm::vec3(vmax[0], vmax[1], vmax[2]))>=0.0f) return 1;	// -NJMP-



  return 0;

}

static int triBoxOverlap(glm::vec3 boxcenter,glm::vec3 boxhalfsize,glm::vec3 triverts[3])

{



    /*    use separating axis theorem to test overlap between triangle and box */

    /*    need to test for overlap in these directions: */

    /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */

    /*       we do not even need to test these) */

    /*    2) normal of the triangle */

    /*    3) crossproduct(edge from tri, {x,y,z}-directin) */

    /*       this gives 3x3=9 more tests */

    glm::vec3 v0,v1,v2;

//   float axis[3];

    float min,max,p0,p1,p2,rad,fex,fey,fez;		// -NJMP- "d" local variable removed

    glm::vec3 normal, e0,e1,e2;



    /* This is the fastest branch on Sun */

    /* move everything so that the boxcenter is in (0,0,0) */

    v0 = triverts[0] - boxcenter;

    v1 = triverts[1] - boxcenter;

    v2 = triverts[2] - boxcenter;



    /* compute triangle edges */

    e0 = v1 - v0;      /* tri edge 0 */

    e1 = v2 - v1;      /* tri edge 1 */

    e2 = v0 - v2;      /* tri edge 2 */



    /* Bullet 3:  */

    /*  test the 9 tests first (this was faster) */

    fex = fabsf(e0.x);

    fey = fabsf(e0.y);

    fez = fabsf(e0.z);

    AXISTEST_X01(e0.z, e0.y, fez, fey);

    AXISTEST_Y02(e0.z, e0.x, fez, fex);

    AXISTEST_Z12(e0.y, e0.x, fey, fex);



    fex = fabsf(e1.x);

    fey = fabsf(e1.y);

    fez = fabsf(e1.z);

    AXISTEST_X01(e1.z, e1.y, fez, fey);

    AXISTEST_Y02(e1.z, e1.x, fez, fex);

    AXISTEST_Z0(e1.y, e1.x, fey, fex);



    fex = fabsf(e2.x);

    fey = fabsf(e2.y);

    fez = fabsf(e2.z);

    AXISTEST_X2(e2.z, e2.y, fez, fey);

    AXISTEST_Y1(e2.z, e2.x, fez, fex);

    AXISTEST_Z12(e2.y, e2.x, fey, fex);



    /* Bullet 1: */

    /*  first test overlap in the {x,y,z}-directions */

    /*  find min, max of the triangle each direction, and test for overlap in */

    /*  that direction -- this is equivalent to testing a minimal AABB around */

    /*  the triangle against the AABB */



    /* test in X-direction */

    FINDMINMAX(v0.x,v1.x,v2.x,min,max);

    if(min>boxhalfsize.x || max<-boxhalfsize.x) return 0;



    /* test in Y-direction */

    FINDMINMAX(v0.y,v1.y,v2.y,min,max);

    if(min>boxhalfsize.y || max<-boxhalfsize.y) return 0;



    /* test in Z-direction */

    FINDMINMAX(v0.z,v1.z,v2.z,min,max);

    if(min>boxhalfsize.z || max<-boxhalfsize.z) return 0;



    /* Bullet 2: */

    /*  test if the box intersects the plane of the triangle */

    /*  compute plane equation of triangle: normal*x+d=0 */

    normal = glm::cross(e0,e1);

    // -NJMP- (line removed here)

    if(!planeBoxOverlap(normal,v0,boxhalfsize)) return 0;	// -NJMP-



    return 1;   /* box and triangle overlaps */

}
