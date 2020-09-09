#include"check_availability.h"
#include<string>
#include<stdexcept>
#include<cctype>


void Session_Info::print_info() const
{
	
}

//Credit to frizzkitten for the explanation on how to initialize CURL 
Checker::Checker(std::string dept):url("https://classes.usc.edu/term-20203/course/")
{
	curl = curl_easy_init();
	if(!curl)
		throw std::runtime_error("Failed to initialize URL");
	new_session(dept);
}

Checker::~Checker()
{
	session.clear();
	curl_easy_cleanup(curl);
}

void Checker::clear_prior_session()
{
	session.clear();
}

void Checker::new_session(std::string input)
{
	clear_prior_session();
	std::string dept, cls_num;
	if(input[0] == 'i' || input[0] == 'I')
	{
		dept = "itp";
		cls_num = "435";
	}
	else if(input[0] == 'c' || input[0] == 'C')
	{
		dept = "csci";
		cls_num = "270";
	}
	url = url.substr(0,43);
	url = url + dept + "-" + cls_num;
	buff = "";
	curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, &Checker::curlWriter);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA, &buff);
	curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1);
	curl_easy_perform(curl);
}

void Checker::parse_target()
{
	std::string tmp = "", cls_id = "";
	bool in_table_row = 0, table_row_finished = 0;
	for(int i = 0; i < (int)buff.size(); ++i)	
	{
		if(!in_table_row && !table_row_finished)
		{
			if(buff[i] == '<' && buff[i+1] == 't' && buff[i+2] == 'r')
			{
				i+=21;
				if(isdigit(buff[i]))
				{
					while(buff[i]!= '\"')
					{
						tmp+=buff[i];
						++i;
					}
					Session_Info new_s;
					session.push_back(new_s);
					session[num_of_session].session_num = tmp;
					tmp = "";
					in_table_row = 1;
				}
			}
			else if(buff.substr(i,8) == "</table>")
				table_row_finished = 1;
		}
		else if(in_table_row && !table_row_finished)
		{
			if(buff[i] == '<' && buff[i+1] == 't' && buff[i+2] == 'd')
			{
				tmp = buff.substr(i+11,4);
				if(tmp == "time")
				{
					tmp = "";
					i+=17;
					while(buff[i]!='<')
					{
						tmp += buff[i];
						++i;
					}
					session[num_of_session].session_time = tmp;
					tmp = "";
				}
				else if(tmp == "regi")
				{
					tmp = "";
					i+=23;
					do
					{
						++i;
					}while(buff[i]!='>');
					++i;
					if(!isdigit(buff[i]))
						i+=20;
					while(buff[i]!='<')
					{
						tmp+=buff[i];
						++i;
					}
					session[num_of_session].registered = tmp;
					tmp = "";
					++num_of_session;
					in_table_row = 0;
				}
			}
		}
		else if(table_row_finished)
		{
			if(buff[i] == '<' && buff[i+1] == 'd' && buff[i+2] == 'i' && buff[i+3] == 'v')
			tmp = buff.substr(i+12,9);
			if(tmp == "timestamp")
			{
				tmp = "";
				i+=23;
				while(buff[i] != '>')
					++i;
				++i;
				while(buff[i] != '<')
				{
					tmp += buff[i];
					++i;
				}
				updated_time = tmp;
				tmp = "";
				return;
			}
		}
	}
}

//credit to https://curl.haxx.se/ on the explanation for the usage of this function
int Checker::curlWriter(char *data, int size, int nmemb, std::string *buffer) 
{
    int result_size = 0;
    if (buffer != NULL) {
        buffer->append(data, size * nmemb);
        result_size = size * nmemb;
    }
    return result_size;
}

void Checker::output(std::ofstream& ostr)
{
	ostr.open("a.txt");
	std::string tmp1 = "Section                Time                Registered",
				tmp2 = "-----------------------------------------------------",
				space = "              ";
	ostr << tmp1 << std::endl << tmp2 << std::endl;
	for(unsigned i = 0; i < session.size(); ++i)
		ostr << session[i].session_num << space << session[i].session_time << space << session[i].registered << std::endl << std::endl; 
	ostr << "Updated Time: " << updated_time;
	ostr.close();
}
