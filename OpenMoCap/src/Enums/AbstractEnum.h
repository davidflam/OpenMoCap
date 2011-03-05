/*!
 * Abstract class for enumerations.
 *
 * \name AbstractEnum
 * \author David Lunardi Flam
 * \version
 * \since 10/20/2008
 */
#ifndef ABSTRACTENUM_H_
#define ABSTRACTENUM_H_

#include <string>
#include <iostream>

using namespace std;

class AbstractEnum {

public:

	/*!
	 * Gets the string that represents the enumeration content.
	 *
	 * @return The text that represents the enumeration content.
	 */
	string getString() const {
		return _text;
	}

	/*!
	 * Overload of the operator "==". Compares if the caller enumeration is equal to the argument.
	 *
	 * @param enumValue The enumeration to be compared.
	 *
	 * @return If the value of the caller enumeration is equal to the value of the argument, this function returns "true", otherwise, "false".
	 */
	bool operator==(const AbstractEnum &enumValue) const {
		return _value == &(enumValue);
	}

	/*!
	 * Overload of the operator "=". Copies the argument to the caller enumeration.
	 *
	 * @param enumValue The enumeration to be its text and value copied.
	 *
	 * @return The caller enumeration with its text and value copied of the argument.
	 */
	AbstractEnum& operator=(const AbstractEnum &enumValue) {

		// Check for self-assignment
		if (this == &(enumValue)) {
			return *this;
		}

		_value = enumValue._value;
		_text = enumValue._text;

		return *this;
	}

	/*!
	 * Overload of the operator "<". Compares the caller enumeration content with the argument content.
	 *
	 * @param enumValue The enumeration to be its content compared.
	 *
	 * @return If the caller enumeration content is lexicographically lower than the argument content this function returns "true", otherwise, "false".
	 */
	bool operator<(const AbstractEnum &enumValue) const {
		return _text < enumValue._text;
	}

protected:

	/*!
	 * AbstractEnum constructor.
	 *
	 * @param text String that represents the enumeration content.
	 */
	AbstractEnum(string text) :
		_text(text) {
		_value = this;
	}

private:
	//! Text that represents the enumeration content.
	string _text;

	//! The enumeration memory address.
	AbstractEnum* _value;

};

#endif /* ABSTRACTENUM_H_ */
