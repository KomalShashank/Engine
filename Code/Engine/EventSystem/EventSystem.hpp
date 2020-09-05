#pragma once

#include <vector>



template <typename ...Args>
class EventSystem
{
public:
	struct Subscription;
	typedef void (SubscriptionCallback)(Subscription* subscription, Args...);
	typedef void (FunctionCallback)(Args...);

	struct Subscription
	{
		SubscriptionCallback* m_UtilityCallback;
		void* m_Callback;
		void* m_Argument;
	};

private:
	std::vector<Subscription> m_Subscriptions;

private:
	void RegisterSubscription(void* utilityCallback, void* actualCallback, void* subscriptionData)
	{
		Subscription newSubscription;
		newSubscription.m_UtilityCallback = (SubscriptionCallback*)utilityCallback;
		newSubscription.m_Callback = actualCallback;
		newSubscription.m_Argument = subscriptionData;

		m_Subscriptions.push_back(newSubscription);
	}

	void UnregisterSubscription(void* utilityCallback, void* actualCallback, void* subscriptionData)
	{
		for (auto currentSubscriptionIterator = m_Subscriptions.begin(); currentSubscriptionIterator != m_Subscriptions.end(); ++currentSubscriptionIterator)
		{
			Subscription currentSubscription = *currentSubscriptionIterator;
			if ((currentSubscription.m_UtilityCallback == (SubscriptionCallback*)utilityCallback) && (currentSubscription.m_Callback == actualCallback) && (currentSubscription.m_Argument == subscriptionData))
			{
				m_Subscriptions.erase(currentSubscriptionIterator);
				break;
			}
		}
	}

public:
	void RegisterFunction(FunctionCallback* functionCallback)
	{
		RegisterSubscription(EventFunctionCallback, functionCallback, nullptr);
	}

	void UnregisterFunction(FunctionCallback* functionCallback)
	{
		UnregisterSubscription(EventFunctionCallback, functionCallback, nullptr);
	}

	template <typename data_type>
	void RegisterMethod(data_type* dataObject, void (data_type::*methodCallback)(Args...))
	{
		void* uniquePointer = *(void**)(&methodCallback);
		RegisterSubscription(MethodCallback<data_type>, uniquePointer, dataObject);
	}

	template <typename data_type>
	void UnregisterMethod(data_type* dataObject, void (data_type::*methodCallback)(Args...))
	{
		void* uniquePointer = *(void**)(&methodCallback);
		UnregisterSubscription(MethodCallback<data_type>, uniquePointer, dataObject);
	}

	void TriggerEvent(Args ...args)
	{
		for (auto& currentSubscription : m_Subscriptions)
		{
			currentSubscription.m_UtilityCallback(&currentSubscription, args...);
		}
	}

	template <typename data_type>
	static void MethodCallback(Subscription* subscription, Args ...args)
	{
		typedef void (data_type::*methodCallback)(Args...);

		methodCallback callback = *(methodCallback*)(&subscription->m_Callback);
		data_type* objectPointer = (data_type*)subscription->m_Argument;
		(objectPointer->*callback)(args...);
	}

	static void EventFunctionCallback(Subscription* subscription, Args ...args)
	{
		FunctionCallback* callback = (FunctionCallback*)(subscription->m_Callback);
		callback(args...);
	}
};