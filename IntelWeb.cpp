//
//  IntelWeb.cpp
//  pj4
//
//  Created by Binyi Wu on 3/6/16.
//  Copyright © 2016 Binyi Wu. All rights reserved.
//
#include <iostream>
#include "IntelWeb.h"
#include "MultiMapTuple.h"
#include <queue>
#include <cstdlib>
using namespace std;

IntelWeb::IntelWeb(){}
IntelWeb::~IntelWeb() { close(); }

bool IntelWeb::createNew(const std::string& filePrefix, unsigned int maxDataItems)
{
    close();
    if (!m_d.createNew(filePrefix+"logdata", maxDataItems/0.75))
        return false;
    m_name = filePrefix + "logdata";
    return true;
}
void IntelWeb::close()
{
    m_d.close();
}
bool ingest(const std::string& telemetryFile)
{
    
}
unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators,
                   unsigned int minPrevalenceToBeGood,
                   std::vector<std::string>& badEntitiesFound,
                   std::vector<InteractionTuple>& interactions
                   )
{
    int num = 0;
    queue<DiskMultiMap::Iterator> q;
    if (!m_d.openExisting(m_name))
        return 0;
    for (int i=0; i<indicators.size();i++)
    {
        string key = indicators[i];
        DiskMultiMap::Iterator it = m_d.search(key);
        q.push(it);
    }
    while (!q.empty())
    {
        DiskMultiMap::Iterator it = q.front();
        q.pop();
        
        MultiMapTuple m = *it;
        
        auto malicious = find(badEntitiesFound.begin(),badEntitiesFound.end(),m.value);
        if (malicious == badEntitiesFound.end())
        {
            badEntitiesFound.push_back(m.value);
            DiskMultiMap::Iterator it = m_d.search(m.value);
            q.push(it);
            num++;
        }
        InteractionTuple ite = InteractionTuple(m.context, m.key, m.value); //note this difference
        auto inter = find(interactions.begin(),interactions.end(),ite);
        if (inter == interactions.end())
            interactions.push_back(ite);
        
        if ((++it).isValid())
        {
            q.push(it);
        }
    }
    sort(badEntitiesFound.begin(),badEntitiesFound.end());
    sort(interactions.begin(),interactions.end());
    return num;
}
