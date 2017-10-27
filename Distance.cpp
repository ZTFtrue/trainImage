//
// Created by ztftrue on 17-8-18.
//

#include "Distance.h"
#include <math.h>
//Distance::Distance() {
//	// TODO Auto-generated constructor stub
//
//}
Distance::Distance(int x, int y) {
    // TODO Auto-generated constructor stub
    centerX = x;
    centerY = y;
}
Distance::~Distance() {

}

int Distance::getDistance(int x, int y) {
    return static_cast<int>(sqrt(pow((x - centerX) , 2) + pow((y - centerY) , 2)));
}

Distance::Distance() {

}
