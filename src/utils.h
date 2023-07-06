#pragma once
#include<iostream>
#include<string>
#include"DatabaseConnection.h"
#include"nlohmann/json.hpp"
#include"conio.h"
#include<vector>
#include<windows.h>
#include<chrono>
using namespace std;

#include <tabulate/table.hpp>
using Row_t = tabulate::Table::Row_t;
using json = nlohmann::json;

#ifndef UTILS_H
#define UTILS_H
class utils
{
	protected:
		string set_time();
		string set_date();

		// Time Function
		time_t date_time_to_timestamp(string& date);
		string return_current_time_and_date(bool date_only, int number_of_day);

		//Menu Generate
		int menu_generate(json data_struct, vector<int>& choice, string output);
		string create_sql_attr(json data_struct, string type);

		//Search Function
		int search_function(json dataStruct, string entity, string title, string message, string additional_condition);
		vector<string> advance_search_function(json dataStruct, string& sql_condition);
		bool condition_choose(string& sql_condition);
		string data_input_search(string data_type, vector<string>& string_data);

		//Database function
		int generate_id(string table_name);

		//Display Function
		int display_in_table(sql::ResultSet* res, bool show_id, string title);
		void display_string(string string_to_display, int num_of_char_perline);

	public:
		//General function
		inline void clearScreen();
		inline void pause();

		//Option and Input Validation
		int option_valid_int(int option_max, int exception, int option_min);
		bool option_valid_char();
};
#endif

//General Function
inline void utils::clearScreen() {
 	system("cls");
}
inline void utils::pause()
{
	cout << "\n\t\t";
	system("pause");
	cin.sync();
}

//Option and Input Validation
int utils::option_valid_int(int choice_max, int exception = 0, int choice_min = 1)
{
	string input = "";
	int choice = 0;
	int number_of_digit = 0;
	int temp = choice_max;
	while (temp > 0)
	{
		number_of_digit++;
		temp /= 10;
	}

	reenter:
	cin >> input;

	if (input.size() > ++number_of_digit)
		goto invalid;
	else
	{
		for(int i = 0; i < input.size(); i++)
		{
			if (isdigit(input[i]) || input[i] == '-')
				continue;
			else
				goto invalid;
		}
		choice = stoi(input);
	}

	if (choice == exception)
		return exception;

	if (choice < choice_min || choice > choice_max)
	{
		invalid:
		cout << "\n\t\tYour Choice is not valid, Please Reenter : ";
		goto reenter;
	}

	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return choice;
}
bool utils::option_valid_char()
{
	string selection;
	cout << "\n\t\tPlease enter your selection (Y/N): ";
	reenter:
	getline(cin, selection);
	cin.clear();

	if (selection.size() != 1)
		goto invalid;

	if (selection == "Y" || selection == "y")
		return true;
	else if (selection == "N" || selection == "n")
		return false;
	else
	{
		invalid:
		cout << "\t\tPlease enter a valid character : ";
		goto reenter;
	}
}
string utils::set_time()
{
	char* time = NULL;
	int hour, minute;
	string time_string = "";

	reenter:
	getline(cin, time_string);
	cin.clear();
	
	if (time_string.size() > 16)
	{
		cout << "\n\t\tYour Format is not correct. Please reenter : ";
		goto reenter;
	}
	else
	{
		time = new char[time_string.size()];
		for (int i = 0; i < time_string.size() ; i++)
			time[i] = time_string[i];
	}

	int i = sscanf(time, "%d:%d", &hour, &minute);
	if (hour > 24 || minute > 60)
	{
		cout << "\n\t\tPlease Insert a Valid Date and Time : ";
		goto reenter;
	}
	else {
		time_string += ":00";
		delete[] time;
		return time_string;
	}	
}
string utils::set_date() {
	char* time = NULL;
	int year, month, day;
	string time_string = "";

reenter:
	getline(cin, time_string);
	cin.clear();

	if (time_string.size() > 10)
	{
		cout << "\n\t\tYour Format is not correct. Please reenter : ";
		goto reenter;
	}
	else
	{
		time = new char[time_string.size()];
		for (int i = 0; i < time_string.size(); i++)
			time[i] = time_string[i];
	}
	
	int i = sscanf(time, "%d-%d-%d", &year, &month, &day);
	if (year < 1900 || month > 12 || day > 31)
	{
		cout << "\t\tPlease Insert a Valid Date : ";
		goto reenter;
	}
	
	delete[] time;
	return time_string;
}

//Time Function
time_t utils::date_time_to_timestamp(string& date)
{
	tm time_tm{};
	time_t input_date = 0;
	stringstream ss(date);
	if (date.size() == 10) {
		date += " 00:00:00";
	}

	ss >> get_time(&time_tm, "%Y-%m-%d %H:%M:%S");
	return mktime(&time_tm);
}
string utils::return_current_time_and_date(bool date_only = false,  int number_of_day = 0)
{
	auto now = chrono::system_clock::now();
	auto in_time = chrono::system_clock::to_time_t(now);

	number_of_day *= 86400;
	in_time += number_of_day;

	stringstream ss;

	if (!date_only)
	{
		ss << std::put_time(std::localtime(&in_time), "%Y-%m-%d %X");
		return ss.str();
	}
	else
	{
		ss << std::put_time(std::localtime(&in_time), "%Y-%m-%d");
		return ss.str();
	}
}

//Menu Gerate
int utils::menu_generate(json data_struct, vector<int>& choice, string output)
{
	int j = 0;
	if (output == "all") {
		for (int i = 0; i < data_struct.size(); i++)
		{
			string columnName = data_struct[i]["columnDescription"];
			choice.push_back(i);
			cout << "\n\t\t" << ++j << " . " << columnName;
		}
	}
	else {
		for (int i = 0; i < data_struct.size(); i++)
		{
			if (data_struct[i][output])
			{
				string columnName = data_struct[i]["columnDescription"];
				choice.push_back(i);
				cout << "\n\t\t" << ++j << " . " << columnName;
			}
			else
				continue;
		}
	}
	return j;
}
string utils::create_sql_attr(json data_struct, string type = "") {

	string attr = "";
	if (type == "updatable") 
	{
		for (int i = 0; i < data_struct.size(); i++) {
			if (data_struct[i][type]) {
				string columnName = data_struct[i]["columnName"];
				attr += columnName + " = ?,";
			}
		}
	}
	else 
	{
		bool condition = true;
		for (int i = 0; i < data_struct.size(); i++) 
		{
			if (type == "display")
				condition = data_struct[i][type];

			if (condition) {
				string column_desc = data_struct[i]["columnName"];
				string column_label = data_struct[i]["columnDescription"];
				attr += column_desc + " AS " + "\"" + column_label + "\",";
			}
		}
	}

	attr = attr.substr(0, attr.size() - 1);
	return attr;
}

//Search Function
int utils::search_function(json dataStruct, string entity, string title = "", string message = "", string additional_condition = "")
{
	again:
	string name_to_search = "";
	cout << "\n\t\t******** Search " << title << " ********";
	cout << "\n\n\t\tPress -1 if you want advance search or -2 if you want to return";
	name_to_search = dataStruct[1]["columnDescription"];
	cout << "\n\t\t" << name_to_search << " : ";
	cin.clear();
	getline(cin, name_to_search);
	cin.clear();

	if (name_to_search == "-2") 
		return -1;
	else 
	{
		DatabaseConnection database_conn;
		sql::PreparedStatement * statement = NULL;
		sql::ResultSet * res = NULL;

		int no_result_found = false;
		string entity_attribute = create_sql_attr(dataStruct);

		if (name_to_search == "-1")
		{
			string sql_condition = "";
			vector<string>string_data = advance_search_function(dataStruct, sql_condition);
			if (string_data[0] == "-1" && sql_condition.empty())
				return -1;

			string sql_statement = "SELECT " + entity_attribute + " FROM " + entity + sql_condition;
			if (additional_condition != "")
				sql_statement += " AND " + additional_condition;

			try 
			{
				statement = database_conn.prepareStatement(sql_statement);
				if (!string_data.empty()) {
					for (int i = 1; i <= string_data.size(); i++)
						statement->setString(i, string_data[(size_t)i - 1]);
				}

				res = statement->executeQuery();
				if (res->rowsCount() == 0) {
					no_result_found = true;
				}
			}
			catch (sql::SQLException& e) {
				cout << "\n\t\t" << e.what() << endl;
				pause();
			}
		}
		else
		{
			string name = dataStruct[1]["columnName"];
			string condition = "SELECT " + entity_attribute + " FROM " + entity + " WHERE " + name + " LIKE ?";
			if (additional_condition != "")
				condition += " AND " + additional_condition;
			try 
			{
				statement = database_conn.prepareStatement(condition);
				statement->setString(1, "%" + name_to_search + "%");
				res = statement->executeQuery();
				if (res->rowsCount() == 0) {
					no_result_found = true;
				}
			}
			catch (sql::SQLException& e) {
				cout << "\n\t\t" << e.what() << endl;
				pause();
			}
		}
	
		if(no_result_found)
		{
			delete statement;
			delete res;

			if (message != "") 
				cout << "\n\t\t" << message << endl;
			else 
				cout << "\n\t\tSorry, No Result found" << endl;

			pause();
			option:
			cout << "\n\t\t1. Search Again ";
			cout << "\n\t\t2. Return";
			cout << "\n\n\t\tPlease Enter Your Choice Here ===> ";
			int choice = option_valid_int(2);
			if (choice == 1)
				goto again;
			else if (choice == 2) 
				return -1;
		}
		else{
			int index = display_in_table(res, false, title);
			if (index == -1) {
				goto option;
			}
			res->absolute(index);
			int id = res->getInt(1);
			delete statement;
			delete res;
			return id;
		}
	}

	return -1;
}
vector<string> utils::advance_search_function(json dataStruct, string& sql_condition)
{
	clearScreen();
	string columnName;

	vector<int> choice;
	sql_condition = " WHERE ";
	cout << "\n\t\t********* Advance Search *********";
	cout << "\n\t\tSearch by :";
	int no_choice = menu_generate(dataStruct, choice, "searchable");
	cout << "\n\n\t\tPress -1 if you want to return" << endl;

	vector<string>string_data;
	while (1)
	{
		bool loop = true;
		cout << "\n\t\tPlease Enter Your Choice Here ====> ";
		int i = option_valid_int(no_choice, -1);
		if (i == -1) {
			string_data.clear();
			sql_condition = "-1";
			return string_data;
		}

		int index = choice[--i];
		cout << "\n\t\tPlease Enter the information :";
		columnName = dataStruct[index]["columnDescription"];
		if (dataStruct[index]["format"] != "")
			columnName += dataStruct[index]["format"];
		cout << "\n\t\t" << columnName << " : ";
		string info = this->data_input_search(dataStruct[index]["columnType"], string_data);
		sql_condition += (string)dataStruct[index]["columnName"] + info;
		loop = condition_choose(sql_condition);
		if (!loop)
			break;

		cout << "\n\t\tPlease Select Another Attibute :";
	}
	return string_data;
}
bool utils::condition_choose(string& sql_condition)
{
	int choice = 0;
	cout << "\n\t\tAddition Criteria ?? ";
	cout << "\n\t\t1. AND \t\t 2. OR \t\t 3. That's All" << endl;
	cout << "\n\t\tPlease Enter Your Choice Here ===> ";
	choice = option_valid_int(2, 3);
	if (choice == 3)
		return false;
	else
	{
		if (choice == 1)
			sql_condition += " AND ";
		else if (choice == 2)
			sql_condition += " OR ";
		return true;
	}
}
string utils::data_input_search(string data_type, vector<string>&string_data)
{
	if (data_type == "STRING")
	{
		string temp_data;
		cin.clear();
		getline(cin,temp_data);
		string_data.push_back("%" + temp_data + "%");
		return " LIKE ? ";
	}
	else
	{
		string condition = "";
		cout << "\n\t\tCondition : ";
		cout << "\n\t\t1. Equal (=) \t\t2. Less Than (<) \t\t3. More Than (>)";
		cout << "\n\t\t4. Not Equal (!=) \t\t5. Between (< x <)";
		cout << "\n\t\tEnter Your Choice Here ===> ";
		int choice = option_valid_int(5);

		string data = "";
		switch (choice)
		{
		case 1:
			condition += " = ";
			break;
		case 2:
			condition += " < ";
			break;
		case 3:
			condition += " > ";
			break;
		case 4:
			condition += " != ";
			break;
		case 5:
		{
			string between[2] = { "","" };
			cout << endl;
			for (int i = 0; i < 2; i++)
			{
				if (data_type == "INT")
				{
					cout << "\t\tEnter here (" << i << ") : ";
					between[i] = to_string(option_valid_int(INT_MAX));
				}
				else if (data_type == "DATE")
				{
					cout << "\n\t\tEnter here (" << i << ") : ";
					between[i] = set_date();
				}
			}
			return condition += " BETWEEN \'" + between[0] + "\' AND \'" + between[1] + "\'";
		}

		}

		if (data_type == "INT")
		{
			cout << "\n\t\tEnter here : ";
			data = to_string(option_valid_int(INT_MAX));
			return condition + " " + data;
		}
		else if (data_type == "DATE")
		{
			cout << "\n\t\tEnter here : ";
			data = set_date();
			return condition + " " + data;
		}
	}

	return "";
}

//Database Function
int utils::generate_id(string entity)
{
	try{
		DatabaseConnection conn;
		string sql_statement = "SELECT " + entity + "_ID FROM " + entity + " ORDER BY " + entity + "_ID DESC LIMIT 1";
		sql::PreparedStatement * statement = conn.prepareStatement(sql_statement);
		sql::ResultSet * res = statement->executeQuery();

		int last_id;
		if (res->rowsCount() != 1)
			last_id = 1;
		else{
			res->first();
			last_id = res->getInt(entity+"_ID") + 1;
		}

		delete statement;
		delete res;
		return last_id;
	}
	catch (sql::SQLException& e) {
		cout << "\n\t\tError : " << e.what();
		pause();
	}
}

//Display Function
int utils::display_in_table(sql::ResultSet* res, bool show_id = true, string title = "") {

	tabulate::Table table;
 	sql::ResultSetMetaData* meta_data = res->getMetaData();
	int number_of_columns = meta_data->getColumnCount();
	int number_of_row = static_cast<int>(res->rowsCount());

	Row_t first_row;
	first_row.push_back("No");
	int index = 1;
	if (!show_id) 
		index = 2;

	vector<int>column_size;
	int total_column_size = 0;
	for (index; index <= number_of_columns; index++) {
		first_row.push_back((string)meta_data->getColumnLabel(index));
		int data_size = meta_data->getColumnDisplaySize(index);
		total_column_size += data_size;
		column_size.push_back(data_size);
	}
	table.add_row(first_row);

	int j = 0;
	res->beforeFirst();
	while (res->next())
	{
		int i = 1;
		if (!show_id) {
			i = 2;
		}

		string row_number = to_string(++j) + ".";
		Row_t temp_row;
		temp_row.push_back(row_number);
		for (i; i <= number_of_columns; i++) {
			int type = meta_data->getColumnType(i);
			if (type == 4) {
				string data = to_string(res->getInt(i));
				temp_row.push_back(data);
			}
			else{
				temp_row.push_back((string)res->getString(i));
			}
		}
		table.add_row(temp_row);
	}

	//Table Formating
	if (!show_id) {
		table.format().corner("").border("");
		for (auto& row : table) {
			row[0].format().border_left("\t\t");
		}
	}
	if (total_column_size > 1000) {
		for (int i = 0; i < column_size.size(); i++) {
			int column = column_size[i];
			if (column >= 200) {
				int size = 45;
				if (column == 200)
					size = 20;

				for (auto& cell : table.column((size_t)i + 1)) {
					cell.format().width(size);
				}
			}
			else {
				continue;
			}
		}
	}
	for (auto& cell : table[0]) {
		cell.format().font_align(tabulate::FontAlign::center)
			.font_style({tabulate::FontStyle::bold});
	}

	//Title Display
	if (title != "") {
		cout << "\n\t\t" << title;
	}
	else {
		cout << "\n\t\t" << "Result :";
	}

	if (!show_id) {
		cout << endl << table << endl;
		cout << "\n\t\t-1. Return or Not in above";
		cout << "\n\t\tPlease Enter Your Choice Here ===> ";
		int choice = option_valid_int(number_of_row, -1);
		if (choice == -1) {
			return -1;
		}
		else {
			return choice;
		}
	}
	else {
		tabulate::Table nested_table;
		nested_table.add_row({table});
		nested_table.format().corner("").hide_border().padding_left(16);
		cout << endl << nested_table;
		return -1;
	}
}
void utils::display_string(string string_to_display, int num_of_char_perline)
{
	tabulate::Table table;
	table.add_row({string_to_display});
	table.format().corner("").width(num_of_char_perline)
		.border_left("\t\t")
		.border_right("").border_bottom("").border_top("");
	cout << table;
}