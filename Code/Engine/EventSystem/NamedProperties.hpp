#pragma once

#include "Engine/ErrorHandling/StringUtils.hpp"

#include <map>



enum PropertyActionResult
{
	GET_PROPERTY_FAILED_EMPTY,
	GET_PROPERTY_FAILED_NOT_PRESENT,
	GET_PROPERTY_FAILED_WRONG_TYPE,
	GET_PROPERTY_SUCCESS,
	SET_PROPERTY_ADDED,
	SET_PROPERTY_REPLACED,
	REMOVE_PROPERTY_FAILED_NOT_PRESENT,
	REMOVE_PROPERTY_SUCCESS
};



class NamedProperty
{
public:
	NamedProperty();
	virtual ~NamedProperty();
};



template <typename data_type>
class TypedNamedProperty : public NamedProperty
{
public:
	TypedNamedProperty(const data_type& propertyData) :
		NamedProperty(),
		m_PropertyData(propertyData)
	{

	}

	~TypedNamedProperty()
	{

	}

public:
	data_type m_PropertyData;
};



class NamedProperties
{
public:
	NamedProperties();
	
	template <typename data_type>
	NamedProperties(const std::string& propertyName, const data_type& propertyValue)
	{
		SetNamedProperty(propertyName, propertyValue);
	}

	template <typename first_data_type, typename second_data_type>
	NamedProperties(const std::string& firstPropertyName, const first_data_type& firstPropertyValue, const std::string& secondPropertyName, const second_data_type& secondPropertyValue)
	{
		SetNamedProperty(firstPropertyName, firstPropertyValue);
		SetNamedProperty(secondPropertyName, secondPropertyValue);
	}

	template <typename first_data_type, typename second_data_type, typename third_data_type>
	NamedProperties(const std::string& firstPropertyName, const first_data_type& firstPropertyValue, const std::string& secondPropertyName, const second_data_type& secondPropertyValue, const std::string& thirdPropertyName, const third_data_type& thirdPropertyValue)
	{
		SetNamedProperty(firstPropertyName, firstPropertyValue);
		SetNamedProperty(secondPropertyName, secondPropertyValue);
		SetNamedProperty(thirdPropertyName, thirdPropertyValue);
	}

	~NamedProperties();

	template <typename data_type>
	PropertyActionResult GetNamedProperty(const std::string& propertyName, data_type& propertyValue) const
	{
		if (m_Properties.size() <= 0)
		{
			return GET_PROPERTY_FAILED_EMPTY;
		}
		
		auto propertyIterator = m_Properties.find(propertyName);
		if (propertyIterator == m_Properties.end())
		{
			return GET_PROPERTY_FAILED_NOT_PRESENT;
		}

		NamedProperty* namedProperty = propertyIterator->second;
		TypedNamedProperty<data_type>* typedProperty = dynamic_cast<TypedNamedProperty<data_type>*>(namedProperty); // TODO: Consider replacing with type IDs.
		if (typedProperty == nullptr)
		{
			return GET_PROPERTY_FAILED_WRONG_TYPE;
		}

		propertyValue = typedProperty->m_PropertyData;

		return GET_PROPERTY_SUCCESS;
	}

	template <typename data_type>
	PropertyActionResult SetNamedProperty(const std::string& propertyName, const data_type& propertyValue)
	{
		PropertyActionResult setResult = SET_PROPERTY_ADDED;
		if (RemoveNamedProperty(propertyName) == REMOVE_PROPERTY_SUCCESS)
		{
			setResult = SET_PROPERTY_REPLACED;
		}

		TypedNamedProperty<data_type>* typedValue = new TypedNamedProperty<data_type>(propertyValue);
		m_Properties[propertyName] = (NamedProperty*)typedValue;

		return setResult;
	}

	PropertyActionResult RemoveNamedProperty(const std::string& propertyName)
	{
		auto propertyIterator = m_Properties.find(propertyName);
		if (propertyIterator == m_Properties.end())
		{
			return REMOVE_PROPERTY_FAILED_NOT_PRESENT;
		}

		delete propertyIterator->second;
		propertyIterator = m_Properties.erase(propertyIterator);

		return REMOVE_PROPERTY_SUCCESS;
	}

public:
	std::map<std::string, NamedProperty*> m_Properties;
};