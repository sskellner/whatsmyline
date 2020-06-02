//
//  PolylineWithLerpFunction.cpp
//  getLerpIndexAtClosestPoint
//
//  Created by richard on 15/09/2016.
//
//

#include "PolylineWithLerpFunction.h"


// This function exists in ofPolyline.cpp
// I just copied it and pasted it here so that the function can use it.
// Not entirely sure how the formula works,..

static ofPoint getClosestPointUtil(const ofPoint& p1, const ofPoint& p2, const ofPoint& p3, float* normalizedPosition) {
    // if p1 is coincident with p2, there is no line
    if(p1 == p2) {
        if(normalizedPosition != nullptr) {
            *normalizedPosition = 0;
        }
        return p1;
    }
    
    float u = (p3.x - p1.x) * (p2.x - p1.x);
    u += (p3.y - p1.y) * (p2.y - p1.y);
    // perfect place for fast inverse sqrt...
    float len = (p2 - p1).length();
    u /= (len * len);
    
    // clamp u
    if(u > 1) {
        u = 1;
    } else if(u < 0) {
        u = 0;
    }
    if(normalizedPosition != nullptr) {
        *normalizedPosition = u;
    }
    return p1.getInterpolated(p2, u);
}



// This function was copied and modified from the 'getClosestPoint()' function in ofPolyline
// See what I commented out to see what was changed. I also included the original function below for comparison.


// 1. Removed integer pointer from arguments
float PolylineWithLerpFunction::getLerpIndexAtClosestPoint(const ofPoint &target/*, unsigned int* nearestIndex*/) const
{
    const ofPolyline & polyline = *this;
    
    if(polyline.size() < 2) {
        //if(nearestIndex != NULL) {
        //    nearestIndex = 0;
        //}
        // no pointer so no need for this!
        return 0;
    }
    
    float distance = 0;
    ofPoint nearestPoint;
    unsigned int nearest = 0;
    float normalizedPosition = 0;
    unsigned int lastPosition = polyline.size() - 1;
    if(polyline.isClosed()) {
        lastPosition++;
    }
    for(int i = 0; i < (int) lastPosition; i++) {
        bool repeatNext = i == (int) (polyline.size() - 1);
        
        const ofPoint& cur = polyline[i];
        const ofPoint& next = repeatNext ? polyline[0] : polyline[i + 1];
        
        float curNormalizedPosition = 0;
        ofPoint curNearestPoint = getClosestPointUtil(cur, next, target, &curNormalizedPosition);
        float curDistance = curNearestPoint.distance(target);
        if(i == 0 || curDistance < distance) {
            distance = curDistance;
            nearest = i;
            nearestPoint = curNearestPoint;
            normalizedPosition = curNormalizedPosition;
        }
    }
    
    //if(nearestIndex != NULL) {
    //    if(normalizedPosition > .5) {
    //        nearest++;
    //        if(nearest == polyline.size()) {
    //            nearest = 0;
    //        }
    //    }
    //    *nearestIndex = nearest;
    //}
    
    // Don't round it based on the normalized position. Instead, add the normalized position to the nearest vertex!
    
    return float(nearest) + normalizedPosition;
}

/*
ofPoint ofPolyline::getClosestPoint(const ofPoint& target, unsigned int* nearestIndex) {
    ofPolyline & polyline = *this;
    
    if(polyline.size() < 2) {
        if(nearestIndex != NULL) {
            nearestIndex = 0;
        }
        return target;
    }
    
    float distance = 0;
    ofPoint nearestPoint;
    unsigned int nearest = 0;
    float normalizedPosition = 0;
    unsigned int lastPosition = polyline.size() - 1;
    if(polyline.isClosed()) {
        lastPosition++;
    }
    for(int i = 0; i < (int) lastPosition; i++) {
        bool repeatNext = i == (int) (polyline.size() - 1);
        
        const ofPoint& cur = polyline[i];
        const ofPoint& next = repeatNext ? polyline[0] : polyline[i + 1];
        
        float curNormalizedPosition = 0;
        ofPoint curNearestPoint = getClosestPointUtil(cur, next, target, &curNormalizedPosition);
        float curDistance = curNearestPoint.distance(target);
        if(i == 0 || curDistance < distance) {
            distance = curDistance;
            nearest = i;
            nearestPoint = curNearestPoint;
            normalizedPosition = curNormalizedPosition;
        }
    }
    
    if(nearestIndex != NULL) {
        if(normalizedPosition > .5) {
            nearest++;
            if(nearest == polyline.size()) {
                nearest = 0;
            }
        }
        *nearestIndex = nearest;
    }
    
    return nearestPoint;
}*/


