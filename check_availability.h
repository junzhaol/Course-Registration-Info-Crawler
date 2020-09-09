#ifndef CHECK_AVAILABILITY_H
#define CHECK_AVAILABILITY_H
#include<string>
#include<curl.h>
#include<vector>
#include<fstream>

struct Session_Info
{
	std::string session_num;
	std::string session_time;
	std::string registered;
	void print_info() const;
};

class Checker
{
public:
	Checker(std::string);
	~Checker();
	void clear_prior_session();
	void new_session(std::string);
	void parse_target();
	static int curlWriter(char* data, int size, int nmemb, std::string *curr);
	void output(std::ofstream& ostr);
private:
	CURL* curl;
	std::string url;
	std::string department;
	std::string class_ID;
	std::string updated_time;
	std::string buff;
	int	num_of_session = 0;
	std::vector<Session_Info> session;
};
//credit to frizzkitten for the help on this class declaration

#endif