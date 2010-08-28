#pragma once

namespace game
{
	//������������ ������ ��� ����
	//(� ���� ���������� ����� update �� game_task)
	class dynamic_object
	{
		friend class game_system;
	public:
		dynamic_object();
		virtual ~dynamic_object();
		virtual void update(float dt)=0;

	private:
		void unsubscribe() {m_is_subscribed = false;}

	private:
		bool m_is_subscribed;
	};
}