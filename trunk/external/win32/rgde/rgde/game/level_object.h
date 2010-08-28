#pragma once

#include "rgde/core/factory.h"

namespace game
{
	//������� ������ ��� ���� ��������, ������� ������� �������
	//(����� ���� �����, ���� ���� ����� ����������� ����� ����������
	//� ����� ��������)
	class ILevelObject
	{
	public:
		virtual ~ILevelObject(){}
	};

	//������� ��������, ������� ����� ��������� ������
	typedef factory::TFactory<game::ILevelObject> LevelObjFactory;

	template <class T>
	class _registrator
	{
	public:
		_registrator(){LevelObjFactory::Instance().RegisterType<T>();}
	protected:
		static _registrator<T> instance;
	};

	#define REGISTER_LEVEL_TYPE(type) _registrator<type> _registrator<type>::instance;
}