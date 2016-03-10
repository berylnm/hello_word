//
//  IntelWeb.hpp
//  pj4
//
//  Created by Binyi Wu on 3/6/16.
//  Copyright © 2016 Binyi Wu. All rights reserved.
//

#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include "DiskMultiMap.h"
#include <string>
#include <cstring>
#include <vector>

class IntelWeb
{
public:
    IntelWeb();
    ~IntelWeb();
    bool createNew(const std::string& filePrefix, unsigned int maxDataItems);
    bool openExisting(const std::string& filePrefix);
    void close();
    bool ingest(const std::string& telemetryFile);
    unsigned int crawl(const std::vector<std::string>& indicators,
                       unsigned int minPrevalenceToBeGood,
                       std::vector<std::string>& badEntitiesFound,
                       std::vector<InteractionTuple>& interactions
                       );
    bool purge(const std::string& entity);
    
private:
    // Your private member declarations will go here
    DiskMultiMap m_d;
    DiskMultiMap m_d2;
    string m_name1;
    string m_name2;
    int getprev(string key);
    
};
inline bool operator==(const InteractionTuple &target,const InteractionTuple &other)
{
 if (target.to == other.to && target.from == other.from && target.context == other.context)
     return true;
    return false;
}
inline bool operator!=(const InteractionTuple &target,const InteractionTuple &other)
{
    if (target.to == other.to && target.from == other.from && target.context == other.context)
        return false;
    return true;

}
inline bool operator<(const InteractionTuple &target,const InteractionTuple &other)
{
    if (target.context+target.from+target.to < other.context+other.from+other.to)
        return true;
    return false;
}
#endif // INTELWEB_H_
