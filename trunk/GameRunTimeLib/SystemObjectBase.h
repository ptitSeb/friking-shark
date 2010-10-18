#pragma once


class CSystemObjectBase:virtual public CSystemSerializableBase,
                        public CSubscriberBase, 
                        public CPublisherBase,
                        virtual public ISystemObject
{
protected:

    std::string  m_sName;
    std::string  m_sClass;
    ISystem     *m_piSystem;
    
		void FinalRelease();

public:


    virtual bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    virtual void Destroy();

    virtual std::string  GetName();
    virtual std::string  GetClass();
    virtual ISystem     *GetSystem();
    virtual std::string  GetSystemName();

    CSystemObjectBase(void);
    virtual ~CSystemObjectBase(void);
};
