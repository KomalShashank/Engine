#pragma once

#include "Engine/EventSystem/NamedProperties.hpp"
#include "Engine/ErrorHandling/StringUtils.hpp"

#include <map>



class ObjectMethod
{
public:
	ObjectMethod();
	virtual ~ObjectMethod();

	virtual void CallMethod(NamedProperties& methodArguments) = 0;
};



template <typename object_data_type>
class RegisteredObjectMethod : public ObjectMethod
{
public:
	RegisteredObjectMethod(object_data_type* registeredObject, void (object_data_type::*registeredMethod)(NamedProperties&)) :
		ObjectMethod(),
		m_RegisteredObject(registeredObject),
		m_RegisteredMethod(*(void**)(&registeredMethod))
	{

	}

	~RegisteredObjectMethod()
	{

	}

	void CallMethod(NamedProperties& methodArguments) override
	{
		typedef void (object_data_type::*registeredMethod)(NamedProperties&);

		registeredMethod methodCallback = *(registeredMethod*)(&m_RegisteredMethod);
		(m_RegisteredObject->*methodCallback)(methodArguments);
	}

public:
	object_data_type* m_RegisteredObject;
	void* m_RegisteredMethod;
};



typedef void (EventFunction)(NamedProperties&);



class GlobalEventSystem
{
private:
	GlobalEventSystem();
	~GlobalEventSystem();

	template <typename object_data_type>
	void RegisterMethodForEvent(const char* eventName, object_data_type* eventObject, void (object_data_type::*eventMethod)(NamedProperties&))
	{
		RegisteredObjectMethod<object_data_type>* newObjectMethod = new RegisteredObjectMethod<object_data_type>(eventObject, eventMethod);

		auto methodEventIterator = m_MethodEvents.find(eventName);
		if (methodEventIterator == m_MethodEvents.end())
		{
			m_MethodEvents[eventName] = std::vector<ObjectMethod*>();
			methodEventIterator = m_MethodEvents.find(eventName);
		}

		methodEventIterator->second.push_back((ObjectMethod*)newObjectMethod);
	}

	template <typename object_data_type>
	void UnregisterMethodFromEvent(const char* eventName, object_data_type* eventObject, void (object_data_type::*eventMethod)(NamedProperties&))
	{
		auto methodEventIterator = m_MethodEvents.find(eventName);
		if (methodEventIterator != m_MethodEvents.end())
		{
			for (auto methodIterator = methodEventIterator->second.begin(); methodIterator != methodEventIterator->second.end();)
			{
				ObjectMethod* currentMethod = *methodIterator;
				RegisteredObjectMethod<object_data_type>* currentObjectMethod = dynamic_cast<RegisteredObjectMethod<object_data_type>*>(currentMethod);
				if (currentObjectMethod != nullptr)
				{
					if (currentObjectMethod->m_RegisteredObject == eventObject && currentObjectMethod->m_RegisteredMethod == *(void**)(&eventMethod))
					{
						methodIterator = methodEventIterator->second.erase(methodIterator);
						continue;
					}
				}

				++methodIterator;
			}
		}
	}

	template <typename object_data_type>
	void UnregisterMethodFromAllEvents(object_data_type* eventObject, void (object_data_type::*eventMethod)(NamedProperties&))
	{
		for (auto& currentEvent : m_MethodEvents)
		{
			for (auto methodIterator = (&currentEvent)->second.begin(); methodIterator != (&currentEvent)->second.end();)
			{
				ObjectMethod* currentMethod = *methodIterator;
				RegisteredObjectMethod<object_data_type>* currentObjectMethod = dynamic_cast<RegisteredObjectMethod<object_data_type>*>(currentMethod);
				if (currentObjectMethod != nullptr)
				{
					if (currentObjectMethod->m_RegisteredObject == eventObject && currentObjectMethod->m_RegisteredMethod == *(void**)(&eventMethod))
					{
						methodIterator = (&currentEvent)->second.erase(methodIterator);
						continue;
					}
				}

				++methodIterator;
			}
		}
	}

	void RegisterFunctionForEvent(const char* eventName, EventFunction* eventFunction);
	void UnregisterFunctionFromEvent(const char* eventName, EventFunction* eventFunction);
	void UnregisterFunctionFromAllEvents(EventFunction* eventFunction);

	void TriggerEventFunctionsAndMethods(const char* eventName);
	void TriggerEventFunctionsAndMethods(const char* eventName, NamedProperties& functionAndMethodArguments);

public:
	static GlobalEventSystem* SingletonInstance();

	template <typename object_data_type>
	static void RegisterMethod(const char* eventName, object_data_type* eventObject, void (object_data_type::*eventMethod)(NamedProperties&))
	{
		SingletonInstance()->RegisterMethodForEvent(eventName, eventObject, eventMethod);
	}

	template <typename object_data_type>
	static void UnregisterMethod(const char* eventName, object_data_type* eventObject, void (object_data_type::*eventMethod)(NamedProperties&))
	{
		SingletonInstance()->UnregisterMethodFromEvent(eventName, eventObject, eventMethod);
	}

	template <typename object_data_type>
	static void UnregisterMethodFromAll(object_data_type* eventObject, void (object_data_type::*eventMethod)(NamedProperties&))
	{
		SingletonInstance()->UnregisterMethodFromAllEvents(eventObject, eventMethod);
	}

	static void RegisterFunction(const char* eventName, EventFunction* eventFunction);
	static void UnregisterFunction(const char* eventName, EventFunction* eventFunction);
	static void UnregisterFunctionFromAll(EventFunction* eventFunction);

	static void TriggerEvent(const char* eventName);
	static void TriggerEvent(const char* eventName, NamedProperties& functionAndMethodArguments);

public:
	std::map<const char*, std::vector<ObjectMethod*>, CompareCStrings> m_MethodEvents;
	std::map<const char*, std::vector<EventFunction*>, CompareCStrings> m_FunctionEvents;
};