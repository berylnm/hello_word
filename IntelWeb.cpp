//
//  IntelWeb.cpp
//  pj4
//
//  Created by Binyi Wu on 3/6/16.
//  Copyright © 2016 Binyi Wu. All rights reserved.
//

#include <iostream>
#include "IntelWeb.h"
#include <queue>
#include "InteractionTuple.h"
#include <fstream>  // needed in addition to <iostream> for file I/O
#include <sstream>  // needed in addition to <iostream> for string stream I/O
#include <string>
#include <cstring>
#include <cstdlib>
using namespace std;

IntelWeb::IntelWeb(){}
IntelWeb::~IntelWeb() { close(); }

bool IntelWeb::createNew(const std::string& filePrefix, unsigned int maxDataItems)
{
    close();
    if (!m_d.createNew(filePrefix+"keyfirst", maxDataItems/0.75))
        return false;
    m_name1 = filePrefix + "keyfirst";
    close();
    if (!m_d2.createNew(filePrefix+"valuefirst",maxDataItems/0.75))
        return false;
    m_name2 = filePrefix + "valuefirst";
    return true;
}
bool IntelWeb::openExisting(const std::string& filePrefix)
{
    close();
    if (!m_d.openExisting(filePrefix+"keyfirst") || !m_d2.openExisting(filePrefix+"valuefirst"))
        return false;
    else
      m_name1=filePrefix+"keyfirst";
      m_name2= filePrefix+"valuefirst";
    return true;
}

void IntelWeb::close()
{
    m_d.close();
    m_d2.close();
}
bool IntelWeb::ingest(const std::string& telemetryFile)
{
    ifstream inf(telemetryFile);
    if (!inf)
    {
        cout << "Cannot open expenses file!" << endl;
        return 1;
    }
    string line;
    string key,value,context;
    while (getline(inf,line))
    {
        istringstream iss(line);
        if ( ! (iss >> key >> value >> context) )
        {
            cerr << "Ignoring badly-formatted input line: " << line << endl;
            continue;
        }
        if (!m_d.openExisting(m_name1))
            cerr<<"not open"<<endl;
        m_d.insert(value,context,key);
        m_d.close();
        if (!m_d2.openExisting(m_name2))
            cerr<<"not open"<<endl;
        m_d2.insert(context,value,key);
        close();
    }
    return true;
}
unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators,
                   unsigned int minPrevalenceToBeGood,
                   std::vector<std::string>& badEntitiesFound,
                   std::vector<InteractionTuple>& interactions
                   )
{
    
    int num = 0;
    queue<DiskMultiMap::Iterator> q;
    if (!m_d.openExisting(m_name1))
        return 0;
    for (int i=0; i<indicators.size();i++)
    {
        string key = indicators[i];
        DiskMultiMap::Iterator it = m_d.search(key);
        while(it.isValid())
        {
            if ((*it).key == key)
                break;
            else
                ++it;
        }
        if (it.isValid())
            q.push(it);
    }
   
    while (!q.empty())
    {
        DiskMultiMap::Iterator it = q.front();
        q.pop();
        
        MultiMapTuple m = *it;
        
        InteractionTuple ite = InteractionTuple(m.context, m.key, m.value); //note this difference
        auto inter = find(interactions.begin(),interactions.end(),ite);
        if (inter == interactions.end())
            interactions.push_back(ite);
        else
        {
            auto malicious = find(badEntitiesFound.begin(),badEntitiesFound.end(),m.value);
            if (malicious == badEntitiesFound.end() && getprev(m.value)<minPrevalenceToBeGood)
            {   
                badEntitiesFound.push_back(m.value);
                DiskMultiMap::Iterator it = m_d.search(m.value);
                q.push(it);
                num++;
            }
        }
        
        if ((++it).isValid())
        {
            q.push(it);
        }
    }
    
    if (!m_d2.openExisting(m_name2))
        return 0;
    for (int i=0; i<indicators.size();i++)
    {
        string key = indicators[i];
        DiskMultiMap::Iterator it = m_d2.search(key);
        q.push(it);
    }
   
    while (!q.empty())
    {
        DiskMultiMap::Iterator it = q.front();
        q.pop();
        
        MultiMapTuple m = *it;
        
        InteractionTuple ite = InteractionTuple(m.context, m.value, m.key); //note this difference
        auto inter = find(interactions.begin(),interactions.end(),ite);
        if (inter == interactions.end())
            interactions.push_back(ite);
        else
        {
            auto malicious = find(badEntitiesFound.begin(),badEntitiesFound.end(),m.value);
            if (malicious == badEntitiesFound.end()&&malicious == badEntitiesFound.end() && getprev(m.value)<minPrevalenceToBeGood)
            {
                badEntitiesFound.push_back(m.value);
                DiskMultiMap::Iterator it = m_d.search(m.value);
                q.push(it);
                num++;
            }
        }
        
        if ((++it).isValid())
        {
            q.push(it);
        }
    }
    
    cout<<"Is there a problem?"<<endl;

       //sort(badEntitiesFound.begin(),badEntitiesFound.end());
    //sort(interactions.begin(),interactions.end());
    
    return num;
}
bool IntelWeb::purge(const std::string& entity)
{   int sign = 0;
    if (!m_d.openExisting(m_name1))
        return false;
    auto it = m_d.search(entity);
    
    while(it.isValid())
    {
        sign = 1;
        m_d.erase((*it).key,(*it).value,(*it).context);
        ++it;
    }
    
    if (!m_d2.openExisting(m_name2))
        return false;
    it = m_d2.search(entity);
    while(it.isValid())
    {
        sign = 1;
        m_d2.erase((*it).key,(*it).value,(*it).context);
        ++it;
    }
    return sign;

}
int IntelWeb::getprev(string key)
{
    int n = 0;
    if (!m_d.openExisting(m_name1))
        cerr<<"not open"<<endl;
    auto it = m_d.search(key);
    while (it.isValid())
    {
        if ((*it).key == key)
            n++;
        ++it;
    }
    m_d.close();
    if (!m_d2.openExisting(m_name2))
        cerr<<"not open"<<endl;
    auto it2 = m_d2.search(key);
    while (it2.isValid())
    {
        if ((*it).key == key)
            n++;
        ++it;
    }

    close();
    return n;
    
}


