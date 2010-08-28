#pragma once

namespace rgde
{
	namespace core
	{
		namespace events
		{
			namespace details
			{
				class base_sender;
				class base_listener;
				class base_events_manager;

				// ������� ����� ��������� �������
				class base_events_manager
				{
				protected:
					base_events_manager();
					virtual ~base_events_manager();

				public:
					//�������� ���������� �� ��������� (������� ������ base_events_manager)
					virtual void unsubscribe (base_listener*) = 0;
					virtual void unsubscribe (base_listener*, base_sender*) = 0;
				};
			}

			// ��������� ����� ��������� �������
			template <typename Event>
			class manager: public details::base_events_manager
			{
				//�������� �� ��������� �������
				struct subscription
				{
					details::base_listener                *m_listener; //��� ����� �������� �������
					details::base_sender                  *m_sender;   //�� ���� ����� �������� ������� (0 - �� ����)
					boost::function<void(Event)>  m_func;      //����� ����� ��������

					bool operator==(const subscription &s)
					{
						return m_listener == s.m_listener;
					}
				};

				 typedef std::list<subscription> subscriptions_list;

			public:
				//��������
				static manager& get()
				{
					static manager instance;
					return instance;
				}

				//��������� listener �� ��������� ������� � ������� func �� ����������� sender (���� ����� 0 - �� �� ����)
				void subscribe (details::base_listener *listener, boost::function<void(Event)> func, 
					details::base_sender *sender)
				{
					subscription subs;
					subs.m_listener = listener;
					subs.m_func      = func;
					subs.m_sender   = sender;
					m_subscriptions.push_back(subs);
				}

				//�������� listener �� ��������� ���� ������� ���� Event
				void unsubscribe (details::base_listener *listener)
				{
					subscriptions_list::iterator i = m_subscriptions.begin();
					while (i != m_subscriptions.end())
					{
						if (i->m_listener == listener)
							i = m_subscriptions.erase(i);
						else
							++i;
					}
				}

				//�������� listener �� ��������� ���� ������� ���� Event
				void unsubscribe (details::base_listener *listener, details::base_sender *sender)
				{
					subscriptions_list::iterator i = m_subscriptions.begin();
					while (i != m_subscriptions.end())
					{
						if (i->m_listener == listener && i->m_sender == sender)
							i = m_subscriptions.erase(i);
						else
							++i;
					}
				}

				//��������� ������� event �� ����������� sender
				void send_event (const Event& event, const details::base_sender *sender)
				{
					subscriptions_list tmp(m_subscriptions.begin(), m_subscriptions.end());

					subscriptions_list::iterator i = tmp.begin();
					while (i != tmp.end())
					{
						if (std::find(m_subscriptions.begin(), m_subscriptions.end(), *i) != m_subscriptions.end())
						{
							if (i->m_sender == 0 || i->m_sender == sender)
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

				subscriptions_list m_subscriptions;
			};

			namespace details
			{
				// ������� ����� ����������
				class base_listener
				{
				protected:
					base_listener();
					virtual ~base_listener();

					//����������� �� ��������� �������
					template <typename Event>
					void subscribe( boost::function<void(Event)> f, base_sender *sender = 0)
					{
						manager<Event>::get().subscribe(this,f,sender);
					}

					//���������� �� ��������� �������
					template <typename Event>
					void unsubscribe(base_sender *sender)
					{
						manager<Event>::get().unsubscribe(this, sender);
					}

					template <typename Event>
					void unsubscribe()
					{
						manager<Event>::get().unsubscribe(this);
					}


				private:
					base_listener(const base_listener&);
					base_listener& operator= (const base_listener&);
				};


				// ������� ����� �����������
				class base_sender
				{
				protected:
					base_sender();
					virtual ~base_sender();

					template<typename Event>
					void base_send_event(const Event& event)
					{
						manager<Event>::get().send_event(event,this);
					}

				private:
					base_sender(const base_sender&);
					base_sender& operator= (const base_sender&);
				};
			}


			// ����������
			class listener	: private details::base_listener
			{
			public:
				//����������� �� ��������� �������
				//  func    - ������ � ���������� void(Event)
				//  sender - �� ���� ����� �������� ������� (0 ��� ������, ���� ����� �������� �� ����)
				template <typename Event>
				void subscribe( boost::function<void(Event)> func, details::base_sender *sender=0 )
				{
					details::base_listener::subscribe<Event>(func,sender);
				}

				//����������� �� ��������� �������
				//  ptr     - ��������� �� ����-������� � ���������� void(Event)
				//  sender - �� ���� ����� �������� ������� (0 ��� ������, ���� ����� �������� �� ����)
				template<typename Event, typename Class, typename EventArg>
				void subscribe (void (Class::*ptr)(EventArg), details::base_sender *sender=0)
				{
					details::base_listener::subscribe<Event>( 
						boost::bind(ptr, static_cast<Class*>(this), _1), sender );
				}

				//���������� �� ��������� �������
				template <typename Event>
				void unsubscribe(details::base_sender* sender)
				{
					details::base_listener::unsubscribe<Event>(sender);
				}

				template <typename Event>
				void unsubscribe()
				{
					details::base_listener::unsubscribe<Event>();
				}
			};



			// �����������
			class sender: public listener, public details::base_sender
			{
			public:
				template<typename Event>
				void send_event(const Event& event)
				{
					base_send_event(event);
				}
			};
		}
	}
}