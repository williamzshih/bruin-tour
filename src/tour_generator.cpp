#include "tour_generator.h"
#include "base_classes.h"
#include "tourcmd.h"
#include "geopoint.h"
#include "geotools.h"
#include <vector>
#include <string>
using namespace std;

TourGenerator::TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router) : m_geodb(geodb), m_router(router) {}

TourGenerator::~TourGenerator() {}

std::vector<TourCommand> TourGenerator::generate_tour(const Stops& stops) const
{
    vector<TourCommand> commands;
    
    for (int i = 0; i < stops.size(); i++)
    {
        string currentPoIName;
        string currentPoICommentary;
        stops.get_poi_data(i, currentPoIName, currentPoICommentary);
        
        // commentary for the current point of interest
        TourCommand commentary;
        commentary.init_commentary(currentPoIName, currentPoICommentary);
        commands.push_back(commentary);
        
        string nextPoIName;
        string nextPoICommentary;
        
        if ( ! stops.get_poi_data(i + 1, nextPoIName, nextPoICommentary))
            return commands;    // no next point of interest
        
        // there is another point of interest following the current point of interest
        // find GeoPoints associated with the current and next point of interest
        GeoPoint currentPoI;
        GeoPoint nextPoI;
        
        if ( ! m_geodb.get_poi_location(currentPoIName, currentPoI))
            return vector<TourCommand>();   // current point of interest not found in the map data
        
        if ( ! m_geodb.get_poi_location(nextPoIName, nextPoI))
            return vector<TourCommand>();   // next point of interest not found in the map data
        
        // the GeoPoints associated with the current and next point of interest were successfully found
        vector<GeoPoint> route = m_router.route(currentPoI, nextPoI);
        
        if (route.empty())
            return vector<TourCommand>();   // no route is possible
        
        // a route from the current point of interest to the next point of interest is possible
        for (int j = 0; j < route.size() - 1; j++)
        {
            string firstSegmentName = m_geodb.get_street_name(route[j], route[j + 1]);
            double firstSegmentDistance = distance_earth_miles(route[j], route[j + 1]);
            double firstSegmentAngle = angle_of_line(route[j], route[j + 1]);
            string firstSegmentDirection;
            
            if (firstSegmentAngle >= 0 && firstSegmentAngle < 22.5)
            {
                firstSegmentDirection = "east";
            }
            else if (firstSegmentAngle >= 22.5 && firstSegmentAngle < 67.5)
            {
                firstSegmentDirection = "northeast";
            }
            else if (firstSegmentAngle >= 67.5 && firstSegmentAngle < 112.5)
            {
                firstSegmentDirection = "north";
            }
            else if (firstSegmentAngle >= 112.5 && firstSegmentAngle < 157.5)
            {
                firstSegmentDirection = "northwest";
            }
            else if (firstSegmentAngle >= 157.5 && firstSegmentAngle < 202.5)
            {
                firstSegmentDirection = "west";
            }
            else if (firstSegmentAngle >= 202.5 && firstSegmentAngle < 247.5)
            {
                firstSegmentDirection = "southwest";
            }
            else if (firstSegmentAngle >= 247.5 && firstSegmentAngle < 292.5)
            {
                firstSegmentDirection = "south";
            }
            else if (firstSegmentAngle >= 292.5 && firstSegmentAngle < 337.5)
            {
                firstSegmentDirection = "southeast";
            }
            else if (firstSegmentAngle >= 337.5)
            {
                firstSegmentDirection = "east";
            }
            
            // proceed from route[j] to route[j + 1]
            TourCommand proceed;
            proceed.init_proceed(firstSegmentDirection, firstSegmentName, firstSegmentDistance, route[j], route[j + 1]);
            commands.push_back(proceed);
            
            // there is a GeoPoint route[j + 2] directly after route[j + 1] on the path
            if (j + 2 < route.size())
            {
                string secondSegmentName = m_geodb.get_street_name(route[j + 1], route[j + 2]);
                double turningAngle = angle_of_turn(route[j], route[j + 1], route[j + 2]);
                
                // segment street names differ and there is some turn
                if (firstSegmentName != secondSegmentName && turningAngle >= 1 && turningAngle <= 359)
                {
                    string turningDirection;
                    
                    if (turningAngle >= 1 && turningAngle < 180)
                    {
                        turningDirection = "left";
                    }
                    else if (turningAngle >= 180 && turningAngle <= 359)
                    {
                        turningDirection = "right";
                    }
                    
                    TourCommand turn;
                    turn.init_turn(turningDirection, secondSegmentName);
                    commands.push_back(turn);
                }
            }
        }
    }
    
    return commands;
}
