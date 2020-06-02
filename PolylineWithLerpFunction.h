//
//  PolylineWithLerpFunction.h
//  getLerpIndexAtClosestPoint
//
//  Created by richard on 15/09/2016.
//
//

#ifndef __getLerpIndexAtClosestPoint__PolylineWithLerpFunction__
#define __getLerpIndexAtClosestPoint__PolylineWithLerpFunction__

#include "ofMain.h"


class PolylineWithLerpFunction : public ofPolyline
{
public:
    
    float getLerpIndexAtClosestPoint(const ofPoint &target) const;
    
};


#endif /* defined(__getLerpIndexAtClosestPoint__PolylineWithLerpFunction__) */
