#include "router.h"
#include "base_classes.h"
#include "geopoint.h"
#include "geotools.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include <unordered_set>
#include <list>
#include <cmath>
using namespace std;

Router::Router(const GeoDatabaseBase& geo_db) : m_geodb(geo_db) {}

Router::~Router() {}

// Use the A* search algorithm to find an optimal path from pt1 (start) to pt2 (end)
std::vector<GeoPoint> Router::route(const GeoPoint& pt1, const GeoPoint& pt2) const
{
    // Minimize f(n) = g(n) + h(n)
    // f(n) is the total cost of using a path with GeoPoint n
    // g(n) is the cost of moving from the start GeoPoint to GeoPoint n
    // h(n) is a heuristic function estimating the cost of moving from GeoPoint n to the end GeoPoint
    
    // openSet contains (pairs of) GeoPoints along with their fScores
    // This uses a min heap priority queue to easily access the GeoPoint with the smallest fScore
    // At first, only the start GeoPoint's fScore is known
    priority_queue<pair<GeoPoint, double>, vector<pair<GeoPoint, double>>, greater<>> openSet;
    openSet.push(pair(pt1, heuristic(pt1, pt2)));
    
    // geoPoints contains the GeoPoints found in openSet. This is so GeoPoints other than openSet.top() can be accessed
    unordered_set<string> geoPoints;
    geoPoints.insert(pt1.to_string());

    // previousMap maps a given GeoPoint to the GeoPoint immediately before it on a path
    unordered_map<string, GeoPoint> previousMap;

    // gScoreMap maps a given GeoPoint to its gScore
    // DoubleWrapper is an object that, when default constructed, contains the maximum value a double can hold
    unordered_map<string, DoubleWrapper> gScoreMap;
    gScoreMap[pt1.to_string()] = DoubleWrapper(0);

    // fScoreMap maps a given GeoPoint to its fScore
    unordered_map<string, DoubleWrapper> fScoreMap;
    fScoreMap[pt1.to_string()] = DoubleWrapper(heuristic(pt1, pt2));

    while ( ! openSet.empty())
    {
        auto current = openSet.top();   // current is a pair with the GeoPoint with the lowest fScore
        if (isAtEnd(current.first, pt2))
            return reconstructPath(previousMap, current.first);
        
        openSet.pop();
        geoPoints.erase(current.first.to_string());
        for (const auto& neighbor : m_geodb.get_connected_points(current.first))
        {
            // tentativeGScore is the cost of moving from the start GeoPoint to the neighbor GeoPoint through the current GeoPoint
            double tentativeGScore = gScoreMap[current.first.to_string()].m_d + distance_earth_miles(current.first, neighbor);
            if (tentativeGScore < gScoreMap[neighbor.to_string()].m_d)
            {
                // This path to the neighbor GeoPoint is better than any previous path
                previousMap[neighbor.to_string()] = current.first;
                gScoreMap[neighbor.to_string()] = DoubleWrapper(tentativeGScore);
                fScoreMap[neighbor.to_string()] = DoubleWrapper(tentativeGScore + heuristic(neighbor, pt2));
                if (geoPoints.find(neighbor.to_string()) == geoPoints.end())    // not found
                {
                    openSet.push(pair(neighbor, fScoreMap[neighbor.to_string()].m_d));
                    geoPoints.insert(neighbor.to_string());
                }
            }
        }
    }
        
    // openSet is empty but a path to the end GeoPoint was never found, so return an empty vector
    return std::vector<GeoPoint>();
}

DoubleWrapper::DoubleWrapper(double d) : m_d(d) {}

bool operator>(const std::pair<GeoPoint, double>& lhs, const std::pair<GeoPoint, double>& rhs)
{
    return (lhs.second > rhs.second);
}

std::vector<GeoPoint> reconstructPath(std::unordered_map<std::string, GeoPoint>& previousMap, const GeoPoint& end)
{
    GeoPoint current = end;
    list<GeoPoint> pathList;    // using a list allows for efficient prepending
    pathList.push_front(current);
    
    while (previousMap.find(current.to_string()) != previousMap.end())  // found
    {
        current = previousMap[current.to_string()];
        pathList.push_front(current);
    }

    return vector<GeoPoint>(pathList.begin(), pathList.end());  // construct a vector of GeoPoints from a list of GeoPoints
}

double heuristic(const GeoPoint& current, const GeoPoint& end)
{
    return (abs(current.latitude - end.latitude) + abs(current.longitude - end.longitude));
}

bool isAtEnd(const GeoPoint& current, const GeoPoint& end)
{
    return (current.sLatitude == end.sLatitude && current.sLongitude == end.sLongitude);
}
