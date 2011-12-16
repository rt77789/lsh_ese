
#ifndef LSH_ESE_UTILS_CONFIG_H
#define LSH_ESE_UTILS_CONFIG_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>

/* Value Object of configer. */
class ConfigerValue {
	const std::string _value;
	public:
		ConfigerValue(const std::string &value):_value(value) {
		}
		ConfigerValue(const ConfigerValue &cv):_value(cv._value) {
		}
		/*
		ConfigerValue& operator=(const ConfigerValue &cv) {
			_value = cv.value;
			Con
			return *this;
		}
		*/
		~ConfigerValue() {
		}
		int toInt() const {
			return atoi(_value.c_str());			
		}
		bool toBool() const {
			if(_value != "true" && _value != "false") {
				throw "ConfigerValue->toBool() is not true or false, please check your config file.";
			}

			return _value == "true" ? true : 
				_value == "false" ? false :
				true;
		}
		double toDouble() const {
			return atof(_value.c_str());
		}
		float toFloat() const {
			return atof(_value.c_str());
		}
		std::string toString() const {
			return _value;
		}
};

class Configer {
	std::map<std::string, ConfigerValue> _kv;

	Configer();
	~Configer();

	void load(const std::string &path);
	/* Trim str, remove the left most and right most whitespaces. */
	std::string trim(const std::string &str) const;
	void insert(const std::string &key, const std::string &value);

	static Configer& instance();

	public:
	static ConfigerValue& get(const std::string &key);
	static void init(const std::string &path);
};

#endif
