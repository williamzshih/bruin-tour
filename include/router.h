#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <unordered_map>
#include <string>
#include <limits>
#include "base_classes.h"
#include "geopoint.h"

class Router: public RouterBase
{
public:
    Router(const GeoDatabaseBase& geo_db);
    virtual ~Router();
    virtual std::vector<GeoPoint> route(const GeoPoint& pt1, const GeoPoint& pt2) const;
private:
    const GeoDatabaseBase& m_geodb;
};

// This struct is needed for a default double value of infinity
struct DoubleWrapper
{
    DoubleWrapper(double d = std::numeric_limits<double>::max());
    double m_d;
};

// for sorting in a priority queue
bool operator>(const std::pair<GeoPoint, double>& lhs, const std::pair<GeoPoint, double>& rhs);

// for backtracking on a path
std::vector<GeoPoint> reconstructPath(std::unordered_map<std::string, GeoPoint>& previousMap, const GeoPoint& end);

// Represents the heuristic function h(n), which estimates the cost to reach the end from GeoPoint n
// uses Manhattan distance
double heuristic(const GeoPoint& current, const GeoPoint& end);

// test if current == end
bool isAtEnd(const GeoPoint& current, const GeoPoint& end);

#endif // ROUTER_H
