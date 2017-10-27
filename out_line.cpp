//
// Created by ztftrue on 17-10-10.
//

#include "out_line.h"

RNG rng(12345);

struct DistanceHu { //顺序结构体
    int distance;
    Point point;
};

bool SortByDistance(const DistanceHu &v1, const DistanceHu &v2) {
    return v1.distance < v2.distance; //升序排列
}


bool SortDistance(const int &v1, const int &v2) {
    return v1 < v2;
}

double start, finish;


out_line::out_line() {

}

out_line::~out_line() {

}
/**
 * 训练轮廓 这里不是我最初想的，以后会弃用的
 * @param size
 * @param contour
 * @param outLineName
 * @return
 */
bool out_line::trainOut(Size size, vector<Point> contour, string outLineName) {
    vector<DistanceHu> vecHu;
    vector<vector<Point>> contours;
    contours.push_back(contour);
    Moments mu = moments(contour, false);
    //计算轮廓的质心   中心
    Point2f mc = Point2f(static_cast<float>(mu.m10 / mu.m00),
                         static_cast<float>(mu.m01 / mu.m00));
    DistanceHu test;
    test.distance = 0;
    test.point = mc;//地一个是质新
    vecHu.push_back(test);
    Distance *dis = new Distance(static_cast<int>(mc.x), static_cast<int>(mc.y));
    Mat drawingW = Mat::zeros(size, CV_8UC3);
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
                          rng.uniform(0, 255));
    drawContours(drawingW, contours, 0, color, 2, 8, 0, 0,
                 Point());
    vector<KeyPoint> kpts1;
    Mat desc1;
    Ptr<AKAZE> akaze = AKAZE::create();
    akaze->detectAndCompute(drawingW, noArray(), kpts1, desc1);
    if (kpts1.size() > 2) {
        mkdir(reinterpret_cast<const char *>(kpts1.size()), S_IRWXU);
        for (int j = 0; j < kpts1.size(); ++j) {
            DistanceHu distanceHu;
            distanceHu.distance = dis->getDistance(static_cast<int>(kpts1[j].pt.x), static_cast<int>(kpts1[j].pt.y));
            distanceHu.point = kpts1[j].pt;
            vecHu.push_back(distanceHu);
        }
        delete dis;
        std::sort(vecHu.begin(), vecHu.end(), SortByDistance);
        mkdir("outline", S_IRWXU);
        string a = "outline/";
        mkdir(a.data(), S_IRWXU);
        if (vecHu[vecHu.size() - 1].distance != 0) {
            a = a + to_string(vecHu[1].distance / vecHu[vecHu.size() - 1].distance);
        } else {
            a = a + "0";
        }
        mkdir(a.data(), S_IRWXU);
        if (vecHu[2].distance != 0) {
            a = a + "/" + to_string(vecHu[1].distance / vecHu[2].distance);
        } else {
            a = a + "0";
        }
        mkdir(a.data(), S_IRWXU);

        if (vecHu[vecHu.size() - 1].distance != 0) {
            a = a + "/" + to_string(vecHu[3].distance / vecHu[vecHu.size() - 1].distance);
        } else {
            a = a + "/0";
        }
        mkdir(a.data(), S_IRWXU);

        int i = static_cast<int>(vecHu.size() / 2);


        if (vecHu[i].distance != 0) {
            a = a + "/" + to_string(vecHu[1].distance / vecHu[i].distance);
        } else {
            a = a + "/0";
        }
        mkdir(a.data(), S_IRWXU);
        if (vecHu[vecHu.size() - 1].distance != 0) {
            a = a + "/" + to_string(vecHu[i].distance / vecHu[vecHu.size() - 1].distance);
        } else {
            a = a + "/0";
        }

        Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array
        Json::Value object;
        Json::Value arrayObj;   // 构建对象
        for (std::vector<DistanceHu>::iterator it = vecHu.begin(); it != vecHu.end(); it++) {
            object["x"] = it->point.x;
            object["y"] = it->point.y;
            object["distance"] = it->distance;
            arrayObj.append(object);
        }
        root["data"] = arrayObj;
        root["name"] = outLineName;
        a = a + "/" + to_string(kpts1.size());
        mkdir(a.data(), S_IRWXU);
        std::string out = root.toStyledString();
        std::ofstream ofs;
        string c;
        stringstream ss;
        int k = 0;
        for (;;) {
            ss.str("");
            ss.clear();
            ss << k;
            c = a + "/" + ss.str() + ".json";
            if (!fopen(c.data(), "r")) {
                break;
            }
            k = k + 1;
        }
        ofs.open(c.data());
        ofs << out;
        ofs.close();
    }
    vecHu.clear();
    cout << "one" << endl;

    return false;
}
/**
 * 识别轮廓，用的是特征点的方法，并不理想
 * @param size
 * @param contour
 * @return
 */
string out_line::reOut(Size size, vector<Point> contour) {

    vector<DistanceHu> vecHu;
    vector<vector<Point>> contours;
    contours.push_back(contour);
    Moments mu = moments(contour, false);
    //计算轮廓的质心   中心
    Point2f mc = Point2f(static_cast<float>(mu.m10 / mu.m00),
                         static_cast<float>(mu.m01 / mu.m00));
    DistanceHu test;
    test.distance = 0;
    test.point = mc;//地一个是质新
    vecHu.push_back(test);

    Distance *dis = new Distance(static_cast<int>(mc.x), static_cast<int>(mc.y));
    Mat drawingW = Mat::zeros(size, CV_8UC3);
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
                          rng.uniform(0, 255));
    drawContours(drawingW, contours, 0, color, 2, 8, 0, 0,
                 Point());
    vector<KeyPoint> kpts1;
    Mat desc1;
    Ptr<BRISK> akaze = BRISK::create();
    start = clock();//取开始时间
    printf("Hello, World!\n");
    akaze->detectAndCompute(drawingW, noArray(), kpts1, desc1);
    finish = clock();//取结束时间
    printf("%f seconds\n", (finish - start) / CLOCKS_PER_SEC);//以秒为单位显示之

    if (kpts1.size() > 2) {
        mkdir(reinterpret_cast<const char *>(kpts1.size()), S_IRWXU);
        for (int j = 0; j < kpts1.size(); ++j) {
            DistanceHu distanceHu;
            distanceHu.distance = dis->getDistance(static_cast<int>(kpts1[j].pt.x), static_cast<int>(kpts1[j].pt.y));
            distanceHu.point = kpts1[j].pt;
            vecHu.push_back(distanceHu);
        }
        delete dis;
        std::sort(vecHu.begin(), vecHu.end(), SortByDistance);
        string a = "outline/";
        if (vecHu[vecHu.size() - 1].distance != 0) {
            a = a + to_string(vecHu[1].distance / vecHu[vecHu.size() - 1].distance);
        } else {
            a = a + "/0";
        }
        if (vecHu[2].distance != 0) {
            a = a + "/" + to_string(vecHu[1].distance / vecHu[2].distance);
        } else {
            a = a + "/0";
        }

        if (vecHu[vecHu.size() - 1].distance != 0) {
            a = a + "/" + to_string(vecHu[3].distance / vecHu[vecHu.size() - 1].distance);
        } else {
            a = a + "/0";
        }

        int i = static_cast<int>(vecHu.size() / 2);

        if (vecHu[i].distance != 0) {
            a = a + "/" + to_string(vecHu[1].distance / vecHu[i].distance);
        } else {
            a = a + "/0";
        }

        if (vecHu[vecHu.size() - 1].distance != 0) {
            a = a + "/" + to_string(vecHu[i].distance / vecHu[vecHu.size() - 1].distance);
        } else {
            a = a + "/0";
        }

        a = a + "/" + to_string(kpts1.size());

        Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array
        Json::Value object;
        Json::Value arrayObj;   // 构建对象
        std::ifstream ifs;
        string c;
        stringstream ss;
        vector<int> vectorDistance;
        int k = static_cast<int>(kpts1.size() - 10);
        for (;;) {
            ss.str("");
            ss.clear();
            ss << k;
            c = a + "/" + ss.str() + ".json";
            if (fopen(c.data(), "r")) {
                //开始识别
                ifs.open(c.data());
                ifs >> root;
                arrayObj = root["data"];
                string name = root["name"].asString();
                for (int j = 1; j < arrayObj.size(); ++j) {
                    object = arrayObj[i];
                    int x = object["x"].asInt();
                    int y = object["y"].asInt();
                    int distance = object["distance"].asInt();
                    vectorDistance.push_back(distance / vecHu[j].distance);
                }
                ifs.close();
                std::sort(vectorDistance.begin(), vectorDistance.end(), SortDistance);
                if (vectorDistance[0] / vectorDistance[vectorDistance.size() - 1] > 1.03) {
                    return root["name"].asString();
                }
            }
            k = k + 1;
            if (k > kpts1.size() + 11) {
                return "";
            }
        }

    } else {
        return "circle";
    }
}


