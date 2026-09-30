#ifndef __INC_METIN_II_UTILS_H__
#define __INC_METIN_II_UTILS_H__

#include <math.h>
#include <cstring>  // Added for std::strlen and std::memcpy
#include <cstddef>  // Added for std::size_t (good practice)
#pragma once
#define IS_SET(flag, bit)		((flag) & (bit))
#define SET_BIT(var, bit)		((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)	((var) = (var) ^ (bit))

template <std::size_t N>
void CopyStringSafe(char(&dest)[N], const char* src) {
	if (!src) {
		dest[0] = '\0';
		return;
	}

	std::size_t len = std::strlen(src);
	if (len >= N)
		len = N - 1;

	std::memcpy(dest, src, len);
	dest[len] = '\0';
}

//inline float DISTANCE_SQRT(int32_t dx, int32_t dy)
//{
//	float fX = (float)dx;
//	float fY = (float)dy;
//	return std::sqrt((fX * fX) + (fY * fY));
//}

inline float DISTANCE_SQRT(int dx, int dy)
{
	float fX = (float)dx;
	float fY = (float)dy;
	return std::sqrt((fX * fX) + (fY * fY));
}

extern bool LEVEL_DELTA(int iLevel, int yLevel, int iDifLev);

#ifndef _WIN32
inline WORD MAKEWORD(BYTE a, BYTE b)
{
	return static_cast<WORD>(a) | (static_cast<WORD>(b) << 8);
}
#endif

extern void set_global_time(time_t t);
extern time_t get_global_time();

#include <string>
std::string mysql_hash_password(const char* tmp_pwd);

extern int	dice(int number, int size);
extern size_t str_lower(const char * src, char * dest, size_t dest_size);

extern void	skip_spaces(char **string);

extern const char *	one_argument(const char *argument, char *first_arg, size_t first_size);
extern const char *	two_arguments(const char *argument, char *first_arg, size_t first_size, char *second_arg, size_t second_size);
extern const char * three_arguments(const char * argument, char * first_arg, size_t first_size, char * second_arg, size_t second_size, char * third_flag, size_t third_size);
extern const char* four_arguments(const char* argument, char* first_arg, size_t first_size, char* second_arg, size_t second_size, char* third_flag, size_t third_size, char* four_flag, size_t four_size);
extern void split_argument(const char *argument, std::vector<std::string> & vecArgs);
extern const char *	first_cmd(const char *argument, char *first_arg, size_t first_arg_size, size_t *first_arg_len_result);

extern int CalculateDuration(int iSpd, int iDur);

extern float gauss_random(float avg = 0, float sigma = 1);

extern int parse_time_str(const char* str);

extern bool WildCaseCmp(const char *w, const char *s);

extern std::string GetFullDateFromTime(time_t&& end_time_sec, bool bWhiteSpace = true);
extern std::string GetFullDateFromTime(const time_t& end_time_sec, bool bWhiteSpace = true);

inline int getRemainingSecondsUntilMidnight(int unixTime) {
	// Get current time
	time_t now = time(0);

	// Convert the current time to tm struct
	tm* currentTime = localtime(&now);

	// Set the tm struct to the beginning of the next day (00:00)
	currentTime->tm_sec = 0;
	currentTime->tm_min = 0;
	currentTime->tm_hour = 0;
	currentTime->tm_mday += 1;

	// Calculate the difference in seconds between the given time and the beginning of the next day
	time_t nextMidnight = mktime(currentTime);
	int remainingSeconds = static_cast<int>(nextMidnight - unixTime);

	return remainingSeconds;
}

// Function to calculate remaining seconds until the end of the week (Sunday 23:59:59)
inline int getRemainingSecondsUntilEndOfWeek(int unixTime) {
	// Get current time
	time_t now = time(0);

	// Convert the current time to tm struct
	tm* currentTime = localtime(&now);

	// Calculate the number of days remaining until the end of the week (Sunday)
	int daysRemaining = 7 - currentTime->tm_wday;

	// Set the tm struct to the end of the week (Sunday 23:59:59)
	currentTime->tm_sec = 59;
	currentTime->tm_min = 59;
	currentTime->tm_hour = 23;
	currentTime->tm_mday += daysRemaining;

	// Calculate the difference in seconds between the given time and the end of the week
	time_t endOfWeek = mktime(currentTime);
	int remainingSeconds = static_cast<int>(endOfWeek - unixTime);

	return remainingSeconds;
}

inline time_t GetMidnightTime()
{
	time_t now = time(0);
	tm* currentTime = localtime(&now);
	
	// Set to next day at 00:00:00
	currentTime->tm_sec = 0;
	currentTime->tm_min = 0;
	currentTime->tm_hour = 0;
	currentTime->tm_mday += 1;
	
	return mktime(currentTime);
}

std::string num_with_point(int n);

namespace CryptoGraphy
{
	std::string EncodeBase64(const std::string& sRaw);
	std::string DecodeBase64(const std::string& sRaw);
}

#endif /* __INC_METIN_II_UTILS_H__ */
//martysama0134's ceqyqttoaf71vasf9t71218
