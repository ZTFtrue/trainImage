//
// Created by ztftrue on 17-8-18.
// 计算距离
//

#ifndef UNTITLED_DISTANCE_H
#define UNTITLED_DISTANCE_H


class Distance {
private:
    int centerX;
    int centerY;
public:
    Distance();
    Distance(int x ,int y);
    virtual ~Distance();
    int getDistance(int x,int y);
};



#endif //UNTITLED_DISTANCE_H
