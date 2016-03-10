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
DiskMultiMap::~DiskMultiMap()
{
    if (m_file.isOpen())
        close();
}

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{
    if (m_file.isOpen())
        m_file.close();
    
    if (!m_file.createNew(filename))
        return false;
    m_fn = filename;
    m_bucket = numBuckets;
    firstfreeNode = 0;
    m_file.write(head(0,0),0);   //head structure for DiskMultiMap  0 used, 0 in total
    for (int i=0; i<numBuckets; i++)
    {
        Bucket bk;
        bk.next = 0;
        bk.used = false;
        m_file.write(bk,m_file.fileLength());
        
    }
    m_file.close();
    return true;
}

bool DiskMultiMap::openExisting(const std::string &filename)
{
    if (!m_file.openExisting(filename))
        return false;
    return true;
}

void DiskMultiMap::close()
{
    if (m_file.isOpen())
        m_file.close();
}
bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context)
{
    if (key.size() > 120 || value.size() > 120 || context.size() > 120)
        return false;
    if (!m_file.openExisting(m_fn))
        return false;
    BinaryFile::Offset prev = 0;
    BinaryFile::Offset Offset = keyhash(key);
    BinaryFile::Offset start = 0;
    int k = m_file.fileLength();
    head h(0,0);
    m_file.read(h,0);
    if (firstfreeNode!=0)
    {
        start = firstfreeNode;
        DiskNode nt;
        if (!m_file.read(nt,firstfreeNode))
            cerr<<"Error! firstfreeNode must have a next value"<<endl;
        firstfreeNode = nt.next;
        
        m_file.write(head(h.numinuse+1,h.numintotal),0);
    }
    else
    {    start = k;
        m_file.write(head(h.numinuse+1,h.numintotal+1),0);
    }
    
    Bucket bk;
    
    m_file.read(bk,Offset);
    if (bk.used == false)
    {
        bk.next = start;
        bk.used = true;
        m_file.write(bk,Offset);
        m_file.write(DiskNode(key,value,context,0),start);
    }
    else
    {   prev = bk.next;
        bk.next = start;
        m_file.write(bk,Offset);
        m_file.write(DiskNode(key,value,context,prev),start);
    }
    
    
//for test purpose///////////////
    DiskNode d;
    m_file.read(d,start);
    m_file.read(bk,Offset);
//need to comment out it/////////////

    m_file.close();
    return true;
}

BinaryFile::Offset DiskMultiMap::keyhash(string key)
{
    hash<string> hasher;
    unsigned int rkey = hasher(key);
    Bucket bk;
    head a = head(0,0);
    rkey = (((rkey % (m_bucket))*sizeof(bk))+sizeof(a));
    return rkey;
}
int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context)
{
    if (!m_file.openExisting(m_fn))
        return -1;
    int num = 0;
    BinaryFile::Offset Offset = keyhash(key);
    Bucket b;
    if (m_file.read(b,Offset) && b.used == false)
    {   m_file.close();
        return 0;
    }
    
    DiskNode current;
    m_file.read(current,b.next);
    BinaryFile::Offset os = b.next;
    BinaryFile::Offset prev = Offset;
    for(;;)
    {
        if (strcmp(current.key,key.c_str())!=0 || strcmp(current.value,value.c_str())!=0 || strcmp(current.context,context.c_str())!=0)
        {
            if (current.next == 0)
            {   m_file.close();
                return num;
            }
            prev = os;
            os = current.next;
            m_file.read(current,current.next);
            continue;
        }
        
        else
        {   num++;
            DiskNode pv;
            m_file.read(pv,prev);
            m_file.write(DiskNode(pv.key,pv.value,pv.context,current.next),prev);
            head h(0,0);
            m_file.read(h,0);
            if (firstfreeNode == 0)
            {   firstfreeNode = os;
                m_file.write(DiskNode(key,value,context,0),os);
                m_file.write(head(h.numinuse-1,h.numintotal),0);
            }
            else
            {
                m_file.write(DiskNode(key,value,context,firstfreeNode),os);
                m_file.write(head(h.numinuse-1,h.numintotal),0);
                firstfreeNode = os;
            }
            if (current.next == 0)
                break;
            else
                m_file.read(current,pv.next);
        }
    }
    
    m_file.close();
    return num;
}
DiskMultiMap::Iterator DiskMultiMap::search(const std::string &key)
{
    if (!m_file.openExisting(m_fn))
        cerr<<"Error, no such file"<<endl;
    BinaryFile::Offset offset = keyhash(key);
    Bucket b;
    m_file.read(b,offset);
    offset = b.next;
    if (b.used == false || b.next == 0)
    {
        Iterator it;
        m_file.close();
        return it;
    }
    else
    {   DiskNode d;
        m_file.read(d,offset);
        while(key!=d.key && d.next!=0)
        {
            offset = d.next;
            m_file.read(d,d.next);
        }
    }
        Iterator it = Iterator(this,offset);
    m_file.close();
    return it;
    
}
///////////////////////////////////////////////
////////ITERATOR//////////////////////////////
////////IMPLEMENTATION////////////////////////
//////////////////////////////////////////////
DiskMultiMap::Iterator::Iterator()
{
    m_valid = false;
    p = nullptr;
}
DiskMultiMap::Iterator::Iterator(DiskMultiMap* d, BinaryFile::Offset o)
{
    m_valid = true;
    p = d;
    offset = o;
}
bool DiskMultiMap::Iterator::isValid() const
{
    return m_valid;
}
MultiMapTuple DiskMultiMap::Iterator::operator*()
{
    MultiMapTuple m;
    if (!this->m_valid)
        return m;
    else
    {
       if (!p->openExisting(p->m_fn))
       {    p->close();
           return m;
       }
        else
        {
            DiskNode d;
            p->m_file.read(d,this->offset);
            m.key = d.key;
            m.value= d.value;
            m.context = d.context;
            
        }
    }
    p->close();
    return m;
    
}
DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++()
{
    DiskNode d;
    if (!p->openExisting(p->m_fn))
        cerr<<"didnt open";
    p->m_file.read(d,this->offset);
    if (d.next == 0)
        m_valid = false;
    else
        offset = d.next;
    p->close();
    return *this;
        
}
void DiskMultiMap::printlist(string key)
{
    if (!openExisting(m_fn))
        return;
    BinaryFile::Offset offset = keyhash(key);
    Bucket b;
    m_file.read(b,offset);
    DiskNode d;
    m_file.read(d,b.next);
    while (d.next!=0)
    {
        cout<<d.key<<" "<<d.value<<" "<<d.context<<endl;
        m_file.read(d,d.next);
    }
    cout<<d.key<<" "<<endl;
    m_file.close();
}

