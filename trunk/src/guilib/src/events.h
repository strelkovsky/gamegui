#pragma once

namespace gui
{
namespace events
{
    class sender_base;
    class listener_base;
    class manager_base;

    // ������� ����� ��������� �������
    class manager_base
    {
	protected:
		manager_base();
		virtual ~manager_base();

    public:
        //�������� ���������� �� ��������� (������� ������ manager_base)
		virtual void unsubscribe (listener_base*) = 0;
        virtual void unsubscribe (listener_base*, sender_base*) = 0;
    };

    // ��������� ����� ��������� �������
    template <typename Event>
    class manager: public manager_base
    {
        //�������� �� ��������� �������
        struct subscription
        {
            listener_base                *m_pListener; //��� ����� �������� �������
            sender_base                  *m_pSender;   //�� ���� ����� �������� ������� (0 - �� ����)
            boost::function<void(Event)>  m_func;      //����� ����� ��������

            bool operator==(const subscription &s)
            {
                return m_pListener == s.m_pListener;
            }
        };

		 typedef std::list<subscription> SubscriptionsList;

    public:
        //��������
        static manager& get()
        {
            static manager instance;
            return instance;
        }

        //��������� l �� ��������� ������� � ������� func �� ����������� pSender (���� ����� 0 - �� �� ����)
        void subscribe (listener_base *l, boost::function<void(Event)> func, sender_base *pSender)
        {
            subscription subs;
            subs.m_pListener = l;
            subs.m_func      = func;
            subs.m_pSender   = pSender;
            m_subscriptions.push_back(subs);
        }

		//�������� l �� ��������� ���� ������� ���� Event
		void unsubscribe (listener_base *listener)
		{
			SubscriptionsList::iterator i = m_subscriptions.begin();
			while (i != m_subscriptions.end())
			{
				if (i->m_pListener == listener)
					i = m_subscriptions.erase(i);
				else
					++i;
			}
		}

        //�������� l �� ��������� ���� ������� ���� Event
        void unsubscribe (listener_base *listener, sender_base *sender)
        {
            SubscriptionsList::iterator i = m_subscriptions.begin();
            while (i != m_subscriptions.end())
            {
                if (i->m_pListener == listener && i->m_pSender == sender)
                    i = m_subscriptions.erase(i);
                else
                    ++i;
            }
        }

        //��������� ������� event �� ����������� pSender
        void sendEvent (const Event& event, const sender_base *pSender)
        {
            SubscriptionsList tmp(m_subscriptions.begin(), m_subscriptions.end());

            SubscriptionsList::iterator i = tmp.begin();
            while (i != tmp.end())
            {
                if (std::find(m_subscriptions.begin(), m_subscriptions.end(), *i) != m_subscriptions.end())
                {
                    if (i->m_pSender == 0 || i->m_pSender == pSender)
                        i->m_func(event);
                    ++i;
                }
            }
        }

    private:
		manager () {}
		~manager() {}

        manager(const manager&);
        manager& operator= (const manager&);

        SubscriptionsList m_subscriptions;
    };


    // ������� ����� ����������
    class  listener_base
    {
    protected:
        listener_base();
        virtual ~listener_base();

        //����������� �� ��������� �������
        template <typename Event>
        void subscribe( boost::function<void(Event)> f, sender_base *sender = 0)
        {
            manager<Event>::get().subscribe(this,f,sender);
        }

        //���������� �� ��������� �������
        template <typename Event>
        void unsubscribe(sender_base *sender)
        {
            manager<Event>::get().unsubscribe(this, sender);
        }

		template <typename Event>
		void unsubscribe()
		{
			manager<Event>::get().unsubscribe(this);
		}


    private:
        listener_base(const listener_base&);
        listener_base& operator= (const listener_base&);
    };

    // ������� ����� �����������
    class  sender_base
    {
    protected:
        sender_base();
		virtual ~sender_base();

        template<typename Event>
        void base_send_event(const Event& event)
        {
			manager<Event>::get().sendEvent(event,this);
        }

    private:
        sender_base(const sender_base&);
        sender_base& operator= (const sender_base&);
    };

	// ����������
	class  Listener: private listener_base
	{
	public:
		//����������� �� ��������� �������
		//  func    - ������ � ���������� void(Event)
		//  pSender - �� ���� ����� �������� ������� (0 ��� ������, ���� ����� �������� �� ����)
		template <typename Event>
		void subscribe( boost::function<void(Event)> func, sender_base *pSender=0 )
		{
			listener_base::subscribe<Event>(func,pSender);
		}

		//����������� �� ��������� �������
		//  ptr     - ��������� �� ����-������� � ���������� void(Event)
		//  pSender - �� ���� ����� �������� ������� (0 ��� ������, ���� ����� �������� �� ����)
		template<typename Event, typename Class, typename EventArg>
		void subscribe (void (Class::*ptr)(EventArg), sender_base *pSender=0)
		{
			listener_base::subscribe<Event>( boost::bind(ptr, static_cast<Class*>(this), _1), pSender );
		}

		//���������� �� ��������� �������
		template <typename Event>
		void unsubscribe(sender_base* sender)
		{
			listener_base::unsubscribe<Event>(sender);
		}

		template <typename Event>
		void unsubscribe()
		{
			listener_base::unsubscribe<Event>();
		}
	};


	// �����������
	class  Sender : public Listener, public sender_base
	{
	public:
		template<typename Event>
		void send_event(const Event& event)
		{
			base_send_event(event);
		}
	};
}//events
}//gui