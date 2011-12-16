
#include "config.h"
#include <fstream>

Configer::Configer() {
}
Configer::~Configer() {
}

ConfigerValue& Configer::get(const std::string &key) {
	std::map<std::string, ConfigerValue>::iterator iter = instance()._kv.find(key);
	if(iter == instance()._kv.end()) {
		std::cerr << "miss key: " << key << std::endl;
		/* Logger::warn("no such key"); */
		throw "Configer::get return NULL";
	}
	return iter->second;
}

Configer& Configer::instance() {
	static Configer configer;
	return configer;
}

/* Initialize the Configer singleton. */
void Configer::init(const std::string &path) {
	instance().load(path);
}

/* Load config file. */
void Configer::load(const std::string &path) {
	/* clear the _kv, make sure it's empty before loadin new config file. */
	_kv.clear();	
	std::ifstream in(path.c_str());
	if(in) {
		std::string line;
		while(getline(in, line)) {
			line = trim(line);	
			/* If the line is a empty line, or first character equals '#' means it's a comment; just skip it. */
			if(line.empty() || line.at(0) == '#') continue;
			size_t delimit = line.find_first_of("=");

			/* Not found '=', just skip it. */
			if(delimit == std::string::npos) continue;
			std::string key = line.substr(0, delimit);
			std::string value = line.substr(delimit + 1, line.size() - delimit);
			insert(key, value);
		}
	}
}

/* Trim a string. */
std::string Configer::trim(const std::string &str) const {
	std::string whitespaces (" \t\f\v\n\r");
	size_t left = str.find_first_not_of(whitespaces);
	size_t right = str.find_last_not_of(whitespaces);
	if(left == std::string::npos) {
		left = 0;
	}
	if(right == std::string::npos) {
		right = str.size();
	}

	return str.substr(left, right - left + 1);
}

void Configer::insert(const std::string &key, const std::string &value) {
	/* Trim key/value before insert. */
	std::string _key(trim(key)), _value(trim(value));
	if(_key.empty() || _value.empty()) {
		// Logger::warn: throw "configer line format should be: key=value, key and value not empty.";
		return ;
	}

	if(_kv.find(_key) == _kv.end()) {
		//_kv[_key] = ConfigerValue(_value);
		_kv.insert(std::make_pair<std::string, ConfigerValue>(_key, _value));
	}
	else {
		/* Logger::warn() if key is exist now. */
	}
}

