#include "Engine/EventSystem/GlobalEventSystem.hpp"



GlobalEventSystem* g_GlobalEventSystem = nullptr;



ObjectMethod::ObjectMethod()
{

}



ObjectMethod::~ObjectMethod()
{

}



GlobalEventSystem::GlobalEventSystem()
{

}



GlobalEventSystem::~GlobalEventSystem()
{

}



void GlobalEventSystem::RegisterFunctionForEvent(const char* eventName, EventFunction* eventFunction)
{
	auto functionEventIterator = m_FunctionEvents.find(eventName);
	if (functionEventIterator == m_FunctionEvents.end())
	{
		m_FunctionEvents[eventName] = std::vector<EventFunction*>();
		functionEventIterator = m_FunctionEvents.find(eventName);
	}

	functionEventIterator->second.push_back(eventFunction);
}



void GlobalEventSystem::UnregisterFunctionFromEvent(const char* eventName, EventFunction* eventFunction)
{
	auto functionEventIterator = m_FunctionEvents.find(eventName);
	if (functionEventIterator != m_FunctionEvents.end())
	{
		for (auto functionIterator = functionEventIterator->second.begin(); functionIterator != functionEventIterator->second.end();)
		{
			EventFunction* currentFunction = *functionIterator;
			if (currentFunction == eventFunction)
			{
				functionIterator = functionEventIterator->second.erase(functionIterator);
				continue;
			}

			++functionIterator;
		}
	}
}



void GlobalEventSystem::UnregisterFunctionFromAllEvents(EventFunction* eventFunction)
{
	for (auto& currentEvent : m_FunctionEvents)
	{
		for (auto functionIterator = (&currentEvent)->second.begin(); functionIterator != (&currentEvent)->second.end();)
		{
			EventFunction* currentFunction = *functionIterator;
			if (currentFunction == eventFunction)
			{
				functionIterator = (&currentEvent)->second.erase(functionIterator);
				continue;
			}

			++functionIterator;
		}
	}
}



void GlobalEventSystem::TriggerEventFunctionsAndMethods(const char* eventName)
{
	NamedProperties functionAndMethodArguments = NamedProperties();
	
	auto methodEventIterator = m_MethodEvents.find(eventName);
	if (methodEventIterator != m_MethodEvents.end())
	{
		for (ObjectMethod* currentObjectMethod : methodEventIterator->second)
		{
			currentObjectMethod->CallMethod(functionAndMethodArguments);
		}
	}

	auto functionEventIterator = m_FunctionEvents.find(eventName);
	if (functionEventIterator != m_FunctionEvents.end())
	{
		for (EventFunction* currentFunction : functionEventIterator->second)
		{
			currentFunction(functionAndMethodArguments);
		}
	}
}



void GlobalEventSystem::TriggerEventFunctionsAndMethods(const char* eventName, NamedProperties& functionAndMethodArguments)
{
	auto methodEventIterator = m_MethodEvents.find(eventName);
	if (methodEventIterator != m_MethodEvents.end())
	{
		for (ObjectMethod* currentObjectMethod : methodEventIterator->second)
		{
			currentObjectMethod->CallMethod(functionAndMethodArguments);
		}
	}

	auto functionEventIterator = m_FunctionEvents.find(eventName);
	if (functionEventIterator != m_FunctionEvents.end())
	{
		for (EventFunction* currentFunction : functionEventIterator->second)
		{
			currentFunction(functionAndMethodArguments);
		}
	}
}



GlobalEventSystem* GlobalEventSystem::SingletonInstance()
{
	if (g_GlobalEventSystem == nullptr)
	{
		g_GlobalEventSystem = new GlobalEventSystem();
	}

	return g_GlobalEventSystem;
}



void GlobalEventSystem::RegisterFunction(const char* eventName, EventFunction* eventFunction)
{
	SingletonInstance()->RegisterFunctionForEvent(eventName, eventFunction);
}



void GlobalEventSystem::UnregisterFunction(const char* eventName, EventFunction* eventFunction)
{
	SingletonInstance()->UnregisterFunctionFromEvent(eventName, eventFunction);
}



void GlobalEventSystem::UnregisterFunctionFromAll(EventFunction* eventFunction)
{
	SingletonInstance()->UnregisterFunctionFromAllEvents(eventFunction);
}



void GlobalEventSystem::TriggerEvent(const char* eventName)
{
	SingletonInstance()->TriggerEventFunctionsAndMethods(eventName);
}



void GlobalEventSystem::TriggerEvent(const char* eventName, NamedProperties& functionAndMethodArguments)
{
	SingletonInstance()->TriggerEventFunctionsAndMethods(eventName, functionAndMethodArguments);
}