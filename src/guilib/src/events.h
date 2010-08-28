#pragma once

namespace gui
{
namespace events
{
    class BaseSender;
    class BaseListener;
    class BaseEventsManager;

    // ������� ����� ��������� �������
    class BaseEventsManager
    {
	protected:
		BaseEventsManager();
		virtual ~BaseEventsManager();

    public:
        //�������� ���������� �� ��������� (������� ������ BaseEventsManager)
		virtual void unsubscribe (BaseListener*) = 0;
        virtual void unsubscribe (BaseListener*, BaseSender*) = 0;
    };

    // ��������� ����� ��������� �������
    template <typename Event>
    class EventsManager: public BaseEventsManager
    {
        //�������� �� ��������� �������
        struct Subscription
        {
            BaseListener                *m_pListener; //��� ����� �������� �������
            BaseSender                  *m_pSender;   //�� ���� ����� �������� ������� (0 - �� ����)
            boost::function<void(Event)>  m_func;      //����� ����� ��������

            bool operator==(const Subscription &s)
            {
                return m_pListener == s.m_pListener;
            }
        };

		 typedef std::list<Subscription> SubscriptionsList;

    public:
        //��������
        static EventsManager& Get()
        {
            static EventsManager instance;
            return instance;
        }

        //��������� pListener �� ��������� ������� � ������� func �� ����������� pSender (���� ����� 0 - �� �� ����)
        void subscribe (BaseListener *pListener, boost::function<void(Event)> func, BaseSender *pSender)
        {
            Subscription subs;
            subs.m_pListener = pListener;
            subs.m_func      = func;
            subs.m_pSender   = pSender;
            m_subscriptions.push_back(subs);
        }

		//�������� pListener �� ��������� ���� ������� ���� Event
		void unsubscribe (BaseListener *listener)
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

        //�������� pListener �� ��������� ���� ������� ���� Event
        void unsubscribe (BaseListener *listener, BaseSender *sender)
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
        void sendEvent (const Event& event, const BaseSender *pSender)
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
		EventsManager () {}
		~EventsManager() {}

        EventsManager(const EventsManager&);
        EventsManager& operator= (const EventsManager&);

        SubscriptionsList m_subscriptions;
    };


    // ������� ����� ����������
    class  BaseListener
    {
    protected:
        BaseListener();
        virtual ~BaseListener();

        //����������� �� ��������� �������
        template <typename Event>
        void subscribe( boost::function<void(Event)> f, BaseSender *sender = 0)
        {
            EventsManager<Event>::Get().subscribe(this,f,sender);
        }

        //���������� �� ��������� �������
        template <typename Event>
        void unsubscribe(BaseSender *sender)
        {
            EventsManager<Event>::Get().unsubscribe(this, sender);
        }

		template <typename Event>
		void unsubscribe()
		{
			EventsManager<Event>::Get().unsubscribe(this);
		}


    private:
        BaseListener(const BaseListener&);
        BaseListener& operator= (const BaseListener&);
    };

    // ������� ����� �����������
    class  BaseSender
    {
    protected:
        BaseSender();
		virtual ~BaseSender();

        template<typename Event>
        void base_send_event(const Event& event)
        {
			EventsManager<Event>::Get().sendEvent(event,this);
        }

    private:
        BaseSender(const BaseSender&);
        BaseSender& operator= (const BaseSender&);
    };

	// ����������
	class  Listener: private BaseListener
	{
	public:
		//����������� �� ��������� �������
		//  func    - ������ � ���������� void(Event)
		//  pSender - �� ���� ����� �������� ������� (0 ��� ������, ���� ����� �������� �� ����)
		template <typename Event>
		void subscribe( boost::function<void(Event)> func, BaseSender *pSender=0 )
		{
			BaseListener::subscribe<Event>(func,pSender);
		}

		//����������� �� ��������� �������
		//  ptr     - ��������� �� ����-������� � ���������� void(Event)
		//  pSender - �� ���� ����� �������� ������� (0 ��� ������, ���� ����� �������� �� ����)
		template<typename Event, typename Class, typename EventArg>
		void subscribe (void (Class::*ptr)(EventArg), BaseSender *pSender=0)
		{
			BaseListener::subscribe<Event>( boost::bind(ptr, static_cast<Class*>(this), _1), pSender );
		}

		//���������� �� ��������� �������
		template <typename Event>
		void unsubscribe(BaseSender* sender)
		{
			BaseListener::unsubscribe<Event>(sender);
		}

		template <typename Event>
		void unsubscribe()
		{
			BaseListener::unsubscribe<Event>();
		}
	};


	// �����������
	class  Sender: public Listener, public BaseSender
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