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


#pragma once


class CSubscriberBase;
class CPublisherBase;

class CSubscriberBase: virtual public ISubscriber
{
    struct SSubscriberSubscriptionInfo
    {
        std::string             sEventInterface;
        IPublisher              *pPublisherBase;

        bool operator <(const SSubscriberSubscriptionInfo &other) const;

        SSubscriberSubscriptionInfo();
    };

    std::set<SSubscriberSubscriptionInfo> m_sClientSubscriptions;

protected:

    virtual bool SubscribeTo(std::string sEventInterface,IPublisher *pPublisher);
    virtual void UnsubscribeFrom(std::string sEventInterface,IPublisher *pPublisher,bool bPublisherCommand);
    virtual void UnsubscribeFrom(IPublisher *pPublisher);
    virtual void UnsubscribeInterface(std::string sEventInterface);
    virtual void UnsubscribeAll();

    friend class CPublisherBase;

    CSubscriberBase();
    virtual ~CSubscriberBase();
};

class CPublisherBase: virtual public IPublisher
{
protected:
    struct SPublisherSubscriptionInfo
    {
        std::string    sEventInterface;
        ISubscriber    *pSubscriberBase;

        bool operator<(const SPublisherSubscriptionInfo &other) const;

        SPublisherSubscriptionInfo();
    };

    std::set<SPublisherSubscriptionInfo> m_sServerSubscriptions;
    std::set<SPublisherSubscriptionInfo> m_sPendingSubscriptions; // suscripciones recibidas mientras se notificaba
    std::set<SPublisherSubscriptionInfo> m_sPendingUnsubscriptions; // desuscripciones recibidas mientras se notificaba
    bool                                 m_bNotifyingFlag; // si esta a true las suscripciones y desuscripciones se meten en la lista de pendientes (para evitar el pete de borrar en stl mientras se recorre el iterador)

    void SetNotifyingFlag(bool bNotifying);
public:

    virtual bool Subscribe(std::string sEventInterface,ISubscriber *pSubscriber);
    virtual void Unsubscribe(std::string sEventInterface,ISubscriber *pSubscriber);

    CPublisherBase();
    virtual ~CPublisherBase();
};

#define NOTIFY_EVENT(_interfaceName_,_call_)\
{ \
    SetNotifyingFlag(true);\
    std::set<CPublisherBase::SPublisherSubscriptionInfo>::iterator i;\
    for(i=m_sServerSubscriptions.begin();i!=m_sServerSubscriptions.end();i++)\
    {\
        _interfaceName_ *pTemp=dynamic_cast<_interfaceName_*>(i->pSubscriberBase);\
        if(pTemp){pTemp->_call_;}\
    }\
    SetNotifyingFlag(false);\
}

#define NOTIFY_EVENT_INSTANCE(instance,_interfaceName_,_call_)\
{ \
    instance->SetNotifyingFlag(true);\
    std::set<CPublisherBase::SPublisherSubscriptionInfo>::iterator i;\
    for(i=instance->m_sServerSubscriptions.begin();i!=instance->m_sServerSubscriptions.end();i++)\
    {\
        _interfaceName_ *pTemp=dynamic_cast<_interfaceName_*>(i->pSubscriberBase);\
        if(pTemp){pTemp->_call_;}\
    }\
    instance->SetNotifyingFlag(false);\
}\

#define SUBSCRIBE_TO(publisher,interfaceName)           SubscribeTo(#interfaceName,publisher)
#define SUBSCRIBE_TO_CAST(publisher,interfaceName)      SubscribeTo(#interfaceName,dynamic_cast<IPublisher*>(publisher))
#define UNSUBSCRIBE_FROM_CAST(publisher,interfaceName)  UnsubscribeFrom(#interfaceName,dynamic_cast<IPublisher*>(publisher),false)
#define UNSUBSCRIBE_FROM_INTERFACE(interfaceName)       UnsubscribeFrom(#interfaceName)
#define UNSUBSCRIBE_FROM_PUBLISHER(publisher)           UnsubscribeFrom(publisher)
#define UNSUBSCRIBE_FROM_PUBLISHER_CAST(publisher)      UnsubscribeFrom(publisher,dynamic_cast<IPublisher*>(publisher))
#define UNSUBSCRIBE_FROM_ALL()                          UnsubscribeAll()

