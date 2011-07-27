//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#include "./stdafx.h"
#include "GameRunTimeLib.h"
#include "EventBase.h"

////////////////////////////////////////////////////////////////////////////////////////
//      CSubscriberBase
/////////////////////////////////////////////////////////////////////////////////////////

CSubscriberBase::SSubscriberSubscriptionInfo::SSubscriberSubscriptionInfo()
{
    pPublisherBase=NULL;
}


bool CSubscriberBase::SSubscriberSubscriptionInfo::operator <(const SSubscriberSubscriptionInfo &other) const
{
    if (pPublisherBase<other.pPublisherBase) {
        return true;
    }
    if (pPublisherBase>other.pPublisherBase) {
        return false;
    }
    return strcmp(sEventInterface.c_str(),other.sEventInterface.c_str())<0;
}

CSubscriberBase::CSubscriberBase()
{
}

CSubscriberBase::~CSubscriberBase()
{
    UnsubscribeAll();
}


bool CSubscriberBase::SubscribeTo(std::string sEventInterface,IPublisher *pPublisher)
{
    if (pPublisher->Subscribe(sEventInterface,this))
    {
        SSubscriberSubscriptionInfo info;
        info.sEventInterface=sEventInterface;
        info.pPublisherBase=pPublisher;
        m_sClientSubscriptions.insert(info);
        return true;
    }
    return false;
}

void CSubscriberBase::UnsubscribeFrom(std::string sEventInterface,IPublisher *pPublisher,bool bPublisherCommand)
{
    SSubscriberSubscriptionInfo info;
    info.sEventInterface=sEventInterface;
    info.pPublisherBase=pPublisher;
    std::set<SSubscriberSubscriptionInfo>::iterator i=m_sClientSubscriptions.find(info);
    if (i!=m_sClientSubscriptions.end())
    {
        if (!bPublisherCommand)
        {
            pPublisher->Unsubscribe(sEventInterface,this);
        }
        m_sClientSubscriptions.erase(i);
    }
}

void CSubscriberBase::UnsubscribeFrom(IPublisher *pPublisher)
{
    std::set<SSubscriberSubscriptionInfo>::iterator i;
    for (i=m_sClientSubscriptions.begin();i!=m_sClientSubscriptions.end();)
    {
        SSubscriberSubscriptionInfo info=*i;
        if (info.pPublisherBase==pPublisher)
        {
            info.pPublisherBase->Unsubscribe(info.sEventInterface,this);
            m_sClientSubscriptions.erase(i++);
        }
        else
        {
            i++;
        }
    }
}
void CSubscriberBase::UnsubscribeInterface(std::string sEventInterface)
{
    std::set<SSubscriberSubscriptionInfo>::iterator i;
    for (i=m_sClientSubscriptions.begin();i!=m_sClientSubscriptions.end();)
    {
        SSubscriberSubscriptionInfo info=*i;
        if (info.sEventInterface==sEventInterface)
        {
            info.pPublisherBase->Unsubscribe(info.sEventInterface,this);
            m_sClientSubscriptions.erase(i++);
        }
        else
        {
            i++;
        }
    }
}
void CSubscriberBase::UnsubscribeAll()
{
    std::set<SSubscriberSubscriptionInfo>::iterator i;
    for (i=m_sClientSubscriptions.begin();i!=m_sClientSubscriptions.end();i++)
    {
        SSubscriberSubscriptionInfo info=*i;
        info.pPublisherBase->Unsubscribe(info.sEventInterface,this);
    }
    m_sClientSubscriptions.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////
//      CPublisherBase
/////////////////////////////////////////////////////////////////////////////////////////

CPublisherBase::SPublisherSubscriptionInfo::SPublisherSubscriptionInfo()
{
    pSubscriberBase=NULL;
}

bool CPublisherBase::SPublisherSubscriptionInfo::operator <(const SPublisherSubscriptionInfo &other) const
{
    if (pSubscriberBase<other.pSubscriberBase) {
        return true;
    }
    if (pSubscriberBase>other.pSubscriberBase) {
        return false;
    }
    return strcmp(sEventInterface.c_str(),other.sEventInterface.c_str())<0;
}

CPublisherBase::CPublisherBase()
{
    m_bNotifyingFlag=false;
}

CPublisherBase::~CPublisherBase()
{
    std::set<SPublisherSubscriptionInfo>::iterator i;
    for (i=m_sServerSubscriptions.begin();i!=m_sServerSubscriptions.end();i++)
    {
        SPublisherSubscriptionInfo info=*i;
        info.pSubscriberBase->UnsubscribeFrom(info.sEventInterface,this,true);
    }
    m_sServerSubscriptions.clear();
}

bool CPublisherBase::Subscribe(std::string sEventInterface,ISubscriber *pSubscriber)
{
    SPublisherSubscriptionInfo info;
    info.sEventInterface=sEventInterface;
    info.pSubscriberBase=pSubscriber;
    if (m_bNotifyingFlag)
    {
        m_sPendingUnsubscriptions.erase(info);
        m_sPendingSubscriptions.insert(info);
    }
    else
    {
        m_sServerSubscriptions.insert(info);
    }
    return true;
}

void CPublisherBase::Unsubscribe(std::string sEventInterface,ISubscriber *pSubscriber)
{
    SPublisherSubscriptionInfo info;
    info.sEventInterface=sEventInterface;
    info.pSubscriberBase=pSubscriber;

    if (m_bNotifyingFlag)
    {
        m_sPendingSubscriptions.erase(info);
        m_sPendingUnsubscriptions.insert(info);
    }
    else
    {
        m_sServerSubscriptions.erase(info);
    }
}

void CPublisherBase::SetNotifyingFlag(bool bNotifying)
{
    bool bWasNotifying=m_bNotifyingFlag;
    m_bNotifyingFlag=bNotifying;
    if (bWasNotifying && !bNotifying)
    {
        std::set<SPublisherSubscriptionInfo>::iterator i;
        for (i=m_sPendingSubscriptions.begin();i!=m_sPendingSubscriptions.end();i++)
        {
            SPublisherSubscriptionInfo info=*i;
            m_sServerSubscriptions.insert(info);
        }
        for (i=m_sPendingUnsubscriptions.begin();i!=m_sPendingUnsubscriptions.end();i++)
        {
            SPublisherSubscriptionInfo info=*i;
            m_sServerSubscriptions.erase(info);
        }
        m_sPendingSubscriptions.clear();
        m_sPendingUnsubscriptions.clear();
    }
}
