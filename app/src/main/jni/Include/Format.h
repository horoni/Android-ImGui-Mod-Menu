#ifndef LSC0X_FORMAT_H
#define LSC0X_FORMAT_H

#include <string>

std::string format(const char *fmt, ...);

/*
std::string str = format("I have %d apples and %d oranges, so I have %d fruits", apples, oranges, apples + oranges);
std::string country = "Great Britain";
std::string capital = "London";
std::cout << format("%s is a capital of %s", capital.c_str(), country.c_str()) << std::endl;
*/

#endif // LSC0X_FORMAT_H