//
//  DiskMultiMap.cpp
//  pj4
//
//  Created by Binyi Wu on 3/5/16.
//  Copyright © 2016 Binyi Wu. All rights reserved.
//

#include "DiskMultiMap.h"
#include "BinaryFile.h"
#include <iostream>
#include <functional>
using namespace std;

DiskMultiMap::DiskMultiMap(){}
DiskMultiMap::~DiskMultiMap(){}

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{
    if (b.isOpen())
        b.close();
    
    b.createNew(m_free);
    b.write(freememory(0,0,0),0);   //head structrue for freed space 0 used, 0 in total
    b.close();
    
    if (!b.createNew(filename))
        return false;
    m_fn = filename;
    m_bucket = numBuckets;
    b.write(freememory(0,0,0),0);   //head structure for DiskMultiMap 0 used, 0 in total
    for (int i=0; i<numBuckets; i++)
    {
        string a;
        Bucket bk(a);
        bk.next = 0;
        bk.nextbucket = 0;
        bk.used = false;
        b.write(bk,b.fileLength());
        
    }
    return true;
}

bool DiskMultiMap::openExisting(const std::string &filename)
{
    if (!b.openExisting(filename))
        return false;
    return true;
}

void DiskMultiMap::close()
{
    if (b.isOpen())
        b.close();
}
bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context)
{
    if (key.size() > 120 || value.size() > 120 || context.size() > 120)
        return false;
    
    
}
BinaryFile::Offset DiskMultiMap::hasher(string key)
{
    
}
