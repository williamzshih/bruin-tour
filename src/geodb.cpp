#include "geodb.h"
#include "geopoint.h"
#include "geotools.h"
#include "hashmap.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

GeoDatabase::GeoDatabase() {}

GeoDatabase::~GeoDatabase() {}

bool GeoDatabase::load(const std::string& map_data_file)
{
    ifstream inf(map_data_file);
    
    if ( ! inf) // file not found
        return false;
    
    // Each "block" of lines in the map data file has
    // A line with the name of the street segment
    // A line with the starting and ending GeoPoints of the street segment
    // A line with the number (P) of points of interest reachable from the street segment
    // P lines with the names and locations of the points of interest found on the street segment
    
    // Get the street name the street segment is a part of
    string street;
    while (getline(inf, street))    // read until the end of the file is reached
    {
        // Get the starting and ending GeoPoints of the street segment
        string startLat;
        string startLong;
        string endLat;
        string endLong;
        inf >> startLat >> startLong >> endLat >> endLong;
        
        GeoPoint start(startLat, startLong);
        GeoPoint end(endLat, endLong);
        
        // from start GeoPoint to end GeoPoint is a street name
        m_streetMap.insert(start.to_string() + end.to_string(), street);
        
        // from end GeoPoint to start GeoPoint is a street name
        m_streetMap.insert(end.to_string() + start.to_string(), street);
        
        // start GeoPoint is connected to end GeoPoint
        m_connectionsMap[start.to_string()].push_back(end);
        
        // end GeoPoint is connected to start GeoPoint
        m_connectionsMap[end.to_string()].push_back(start);
        
        // Get the number of points of interest
        int numPoI;
        inf >> numPoI;
        
        GeoPoint mid = midpoint(start, end);
        
        // Create midpoint connections
        if (numPoI >= 1)
        {
            // from start GeoPoint to mid GeoPoint is a street name
            m_streetMap.insert(start.to_string() + mid.to_string(), street);
            
            // from mid GeoPoint to start GeoPoint is a street name
            m_streetMap.insert(mid.to_string() + start.to_string(), street);
            
            // from mid GeoPoint to end GeoPoint ia a street name
            m_streetMap.insert(mid.to_string() + end.to_string(), street);
            
            // from end GeoPoint to mid GeoPoint is a street name
            m_streetMap.insert(end.to_string() + mid.to_string(), street);
            
            // start GeoPoint is connected to mid GeoPoint
            m_connectionsMap[start.to_string()].push_back(mid);
            
            // mid GeoPoint is connected to start GeoPoint
            m_connectionsMap[mid.to_string()].push_back(start);
            
            // mid GeoPoint is connected to end GeoPoint
            m_connectionsMap[mid.to_string()].push_back(end);
            
            // end GeoPoint is connected to mid GeoPoint
            m_connectionsMap[end.to_string()].push_back(mid);
        }

        // Next step is to read in an entire line with getline, so ignore any newline characters
        inf.ignore(10000, '\n');
        
        for (int i = 0; i < numPoI; i++)
        {
            // Get the point of interest name and location input line
            string poiLine;
            getline(inf, poiLine);
            istringstream iss(poiLine);
            
            // Get the point of interest name
            char c;
            string poiName;
            for (;;)
            {
                iss.get(c);
                if (c == '|')
                    break;
                poiName += c;
            }
            
            // Get the point of interest location
            string poiLat;
            string poiLong;
            iss >> poiLat >> poiLong;
            
            GeoPoint poi(poiLat, poiLong);
            
            // Associate a point of interest name to a point of interest GeoPoint
            m_poiMap.insert(poiName, poi);
            
            // from mid GeoPoint to poi GeoPoint is "a path"
            m_streetMap.insert(mid.to_string() + poi.to_string(), "a path");
            
            // from poi GeoPoint to mid GeoPoint is "a path"
            m_streetMap.insert(poi.to_string() + mid.to_string(), "a path");
            
            // mid GeoPoint is connected to poi GeoPoint
            m_connectionsMap[mid.to_string()].push_back(poi);
            
            // poi GeoPoint is connected to mid GeoPoint
            m_connectionsMap[poi.to_string()].push_back(mid);
        }
    }
    
    return true;
}

bool GeoDatabase::get_poi_location(const std::string& poi, GeoPoint& point) const
{
    auto geoPointer = m_poiMap.find(poi);
    
    if (geoPointer == nullptr)  // poi not found
        return false;
    
    point = *geoPointer;    // poi found
    return true;
}

std::vector<GeoPoint> GeoDatabase::get_connected_points(const GeoPoint& pt) const
{
    auto connectionsPointer = m_connectionsMap.find(pt.to_string());
    
    vector<GeoPoint> connections;
    
    if (connectionsPointer == nullptr)  // GeoPoint not found
        return connections; // return empty vector
    
    for (const auto& geoPoint : *connectionsPointer)    // GeoPoint found
        connections.push_back(geoPoint);
    
    return connections;
}

std::string GeoDatabase::get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const
{
    auto streetPointer = m_streetMap.find(pt1.to_string() + pt2.to_string());
    
    if (streetPointer == nullptr)   //  street not found
        return "";
    
    return *streetPointer;  // street found
}
