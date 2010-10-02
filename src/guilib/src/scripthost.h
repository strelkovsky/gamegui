#pragma once

struct lua_State;

namespace gui
{
	class  ScriptObject;

	class ScriptStack
	{
	public:
		ScriptStack();
		void clear();
		void push(ScriptObject* obj);
		void pop(lua_State* state);
	private:
		std::vector<ScriptObject*> m_stack;
	};

	class  ScriptSystem
	{
	public:
		/// @brief - ctor
		/// @param externalState - требует валидный стейт либо 0, если внутренний
		explicit ScriptSystem(lua_State* externalState);
		~ScriptSystem();

		lua_State* LuaState();

		bool ExecuteString(const std::string& script, ScriptObject* obj, const std::string& filename = "unknown event");
		bool ExecuteString(const std::string& script, const std::string& filename = "mem buffer");
		bool ExecuteFile(const std::string& filename);

		const std::string& GetLastError() { return m_error; }

	protected:
		std::string LoadFile(const std::string& filename);
		std::string GetLuaError();
		bool Execute(const std::string& script, const std::string& filename);
	
	private:
		lua_State* m_state;
		std::string m_error;

		ScriptStack m_thisStack;
		bool m_ext;
	};
}