#ifndef TOURGENERATOR_H
#define TOURGENERATOR_H

#include <vector>
#include "base_classes.h"
#include "tourcmd.h"

class TourGenerator: public TourGeneratorBase
{
public:
    TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router);
    virtual ~TourGenerator();
    virtual std::vector<TourCommand> generate_tour(const Stops& stops) const;
private:
    const GeoDatabaseBase& m_geodb;
    const RouterBase& m_router;
};

#endif // TOURGENERATOR_H
