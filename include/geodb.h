#ifndef GEODB_H
#define GEODB_H

#include <string>
#include <vector>
#include "base_classes.h"
#include "geopoint.h"
#include "hashmap.h"

class GeoDatabase: public GeoDatabaseBase
{
public:
    GeoDatabase();
    virtual ~GeoDatabase();
    
    virtual bool load(const std::string& map_data_file);
    virtual bool get_poi_location(const std::string& poi, GeoPoint& point) const;
    virtual std::vector<GeoPoint> get_connected_points(const GeoPoint& pt) const;
    virtual std::string get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const;
private:
    // Types of HashMaps:
    // Point of interest -> GeoPoint
    // GeoPoint -> all connected GeoPoints
    // 2 GeoPoints -> street
    HashMap<GeoPoint> m_poiMap;
    HashMap<std::vector<GeoPoint>> m_connectionsMap;
    HashMap<std::string> m_streetMap;
};

#endif // GEODB_H
