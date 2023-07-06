#pragma once
#include"utils.h"
using namespace std;

class Seat_Type;
class Hall;

json cinemaDataStruct{
	{
		{"columnType", "INT"},
		{"columnName", "CINEMA_ID"},
		{"columnDescription", "Cinema ID"},
		{"format", ""},
		{"input", false},
		{"searchable", false},
		{"updatable", false},
		{"display", false}
	},{
		{"columnType", "STRING"},
		{"columnName", "CINEMA_NAME"},
		{"columnDescription", "Cinema Name"},
		{"format", ""},
		{"input", true},
		{"searchable", true},
		{"updatable", true},
		{"display", true}
	},{
		{"columnType", "STRING"},
		{"columnName", "CINEMA_ADDRESS"},
		{"columnDescription", "Cinema Addess"},
		{"format", "(Full Address)"},
		{"input", true},
		{"searchable", true},
		{"updatable", true},
		{"display", true}
	}
};
json hallDataStruct{
	{
		{"columnType", "INT"},
		{"columnName", "HALL_ID"},
		{"columnDescription", "Hall Id"},
		{"format", ""},
		{"option",""},
		{"input", false},
		{"searchable", false},
		{"updatable", false},
		{"display", false}
	},{
		{"columnType", "INT"},
		{"columnName", "HALL_NUM"},
		{"columnDescription", "Hall Number"},
		{"format", ""},
		{"option",""},
		{"input", false},
		{"searchable", true},
		{"updatable", false},
		{"display", true}
	},{
		{"columnType", "INT"},
		{"columnName", "NUM_ROW"},
		{"columnDescription", "Number of Row"},
		{"format",""},
		{"option",""},
		{"input", true},
		{"searchable", false},
		{"updatable", false},
		{"display", true}
	},{
		{"columnType", "INT"},
		{"columnName", "NUM_SEAT_PER_ROW"},
		{"columnDescription", "Number of Seat per Row"},
		{"format", ""},
		{"option",""},
		{"input", false},
		{"searchable", false},
		{"updatable", false},
		{"display", true}
	},{
		{"columnType", "INT"},
		{"columnName", "HALL_TYPE"},
		{"columnDescription", "Hall Type"},
		{"format", "(Please Choose from below)"},
		{"option",{
			{"name",{"Standard Hall","Premium Hall","Big Screen"}},
			{"capacity", {250, 220, 200}},
			{"seat_type_id", {0, 1, 2}}
		}},
		{"input", true},
		{"searchable", false},
		{"updatable", true},
		{"display", false}
	},{
		{"columnType", "STRING"},
		{"columnName", "HALL_NAME"},
		{"columnDescription", "Hall Name"},
		{"format", ""},
		{"option",""},
		{"input", false},
		{"searchable", true},
		{"updatable", true},
		{"display", true}
	}
};

#ifndef HALL_h
#define HALL_H
class Hall : private utils
{
private:

	//Hall Information
	int hall_id;
	int hall_num;
	int hall_capacity;
	int hall_type_id;
	string hall_type;

public:
	Hall();
	void add_hall(int cinema_id, string cinema_name);
	void add_seat(int row_number, int seat_per_row, int last_hall_id);
	void set_hall_id(int hall_id);
	int get_hall_id();
	void set_hall_number(int hall_number);
	int get_hall_number();
	void set_hall_type(string hall_type);
	string get_hall_type();
};
#endif // !HALL_h

#ifndef CINEMA_H
#define CINEMA_H
class Cinema: public utils
{
	friend class MovieShowTime;
	private:
		DatabaseConnection database_conn;
		Hall hall;

		int cinema_id;
		string name;
		string address;

	public:
		Cinema();
		void add_cinema();
		void read_cinema();
		void edit_cinema();
		void set_cinema_id(int cinema_id);

		void display_cinema();
		bool search_cinema();
		string get_cinema_name();
		string get_cinema_address();
		void data_input(int choice);
		int get_cinema_id();

		Hall& get_hall();
};
#endif // !CINEMA_H

//Cinema
Cinema::Cinema()
{
	cinema_id = 0;
	name = "";
	address = "";
}
void Cinema::add_cinema()
{
	clearScreen();
	cout << "\n\t\t********* Add Cinema *********";
	cout << "\n\t\tPlease enter the following inforamtion : \n";

	cinema_id = generate_id("CINEMA") + 1;

	again:
	//Input Data
	for (int i = 1; i < cinemaDataStruct.size(); i++)
	{
		if (cinemaDataStruct[i]["input"])
		{
			string columnName = cinemaDataStruct[i]["columnDescription"];
			if (cinemaDataStruct[i]["format"] != "")
				columnName += cinemaDataStruct[i]["format"];
			cout << "\t\t" << columnName << " : ";
			this->data_input(i);
		}
	}

	string sql_statement = "INSERT INTO CINEMA (CINEMA_ID, CINEMA_NAME, CINEMA_ADDRESS) VALUES (?, ?, ?)";
	try {
		sql::PreparedStatement* statement = database_conn.prepareStatement(sql_statement);
		statement->setInt(1, cinema_id);
		statement->setString(2, name);
		statement->setString(3, address);
		int status = statement->executeUpdate();
		if (status == 1) {

			delete statement;
			cout << "\n\t\tCinema added successfully";
			cout << "\n\t\tAdd Hall In The Cinema (Y) ? or Will do it later (N) ?";
			if (option_valid_char()) {
				hall.add_hall(cinema_id, name);
				return;
			}
			else {
				return;
			}
		}
	}
	catch (sql::SQLException e) {
		int status = e.getErrorCode();
		if (status == 1062) {
			cout << "\n\t\tCinema Name and Address must be unique. Please reenter";
			goto again;
		}
		else {
			cout << e.what();
			pause();
		}
	}
}
void Cinema::edit_cinema()
{
	cout << "\n\n\t\t********* Cinema Information Edit *********";
	cout << "\n\t\t1. Edit Cinema's Information";
	cout << "\n\t\t2. Creat Hall";
	cout << "\n\t\t3. Back";
	cout << "\n\n\t\tPlease Enter Your Choice Here : ";
	int i = option_valid_int(2, 3);

	if (i == 1)
	{
		clearScreen();
		display_cinema();
		cout << "\n\n\t\t********* Cinema Information Edit *********";
		vector<int>choice;
		string columnName = "";

		//Generate Menu
		int no_of_attr = menu_generate(cinemaDataStruct, choice, "updatable");

		cout << endl << endl;
		again:
		while (1) {
			cout << "\n\t\tPlease Enter Your Choice Here ====> ";
			int j = option_valid_int(no_of_attr);
			int index = choice[--j];

			//Insert Data
			cout << "\n\t\tPlease Enter the information :";
			columnName = cinemaDataStruct[index]["columnDescription"];
			if (cinemaDataStruct[index]["format"] != "")
				columnName += cinemaDataStruct[index]["format"];
			cout << "\n\t\t" << columnName << " : ";
			data_input(index);

			//Edit Another Data 
			cout << "\n\t\tOther Cinema Data To Edit ??";
			if (option_valid_char())
			{
				cout << "\n\t\tPlease Choose another attribute";
			}
			else {
				break;
			}
		}
		
		string sql_attr = create_sql_attr(cinemaDataStruct, "updatable");
		string sql_statement = "UPDATE CINEMA SET " + sql_attr + " WHERE CINEMA_ID = ?";
		try {
			sql::PreparedStatement* statement = database_conn.prepareStatement(sql_statement);
			statement->setString(1, name);
			statement->setString(2, address);
			statement->setInt(3, this->cinema_id);
			int status = statement->executeUpdate();
			if (status >= 1) {
				delete statement;
				cout << "\n\t\tCinema Information Updated Successfully";
				pause();
			}
			else {
				cout << "\n\t\tNo information is updated";
				pause();
			}
		}
		catch (sql::SQLException e) {
			int status = e.getErrorCode();
			if (status == 1062) {
				cout << "\n\t\tCinema Name and Address must be unique. Please reenter";
				goto again;
			}
			else {
				cout << e.what();
				pause();
			}
		}
		return;
	}
	else if (i == 2)
	{
		clearScreen();
		hall.add_hall(cinema_id, name);
		return;
	}
	else if (i == 3)
		return;
}
void Cinema::data_input(int choice)
{
	switch (choice)
	{
	case 1: getline(cin, name);
		break;
	case 2: getline(cin, address);
		break;
	}
}
bool Cinema::search_cinema()
{
	cinema_id = search_function(cinemaDataStruct, "CINEMA", "Cinema", "No Cinema Found");
	if (cinema_id == -1) {
		return false;
	}
	else{
		return true;
	}
}
void Cinema::read_cinema()
{
	sql::PreparedStatement* statement = NULL;
	sql::ResultSet* res = NULL;

	try {
		string sql_statement = "SELECT * FROM CINEMA WHERE CINEMA_ID = ?";
		statement = database_conn.prepareStatement(sql_statement);
		statement->setInt(1, cinema_id);
		res = statement->executeQuery();

		res->first();
		name = res->getString("CINEMA_NAME");
		address = res->getString("CINEMA_ADDRESS");
		delete statement;
		delete res;
	}
	catch (sql::SQLException& e) {
		cout << "\n\t\t" << e.what() << endl;
	}
}
void Cinema::display_cinema()
{
	cout << "\n\t\t" << name;
	cout << "\n\t\tAddress :";
	cout << "\n\t\t" << this->address;
}

string Cinema::get_cinema_name() {
	return this->name;
}
string Cinema::get_cinema_address() {
	return this->address;
}
void Cinema::set_cinema_id(int cinema_id) {
	this->cinema_id = cinema_id;
}
Hall & Cinema::get_hall() {
	return this->hall;
}
int Cinema::get_cinema_id()
{
	return cinema_id;
}

//Hall
Hall::Hall()
{
	hall_id = 0;
	hall_num = 0;
	hall_capacity = 0;
	hall_type_id = 0;
	hall_type = "";
}
void Hall::add_hall(int cinema_id, string cinema_name)
{
	DatabaseConnection database_conn;
	sql::PreparedStatement* statement = NULL;
	sql::ResultSet* res = NULL;

	cout << "\n\t\t********* Add Hall *********";
	cout << "\n\t\tPlease enter the following information : ";

	hall_id = generate_id("HALL");
	++hall_id;
	try 
	{
		string sql_statement = "SELECT HALL_NUM FROM HALL NATURAL JOIN CINEMA WHERE CINEMA_ID = ? ORDER BY HALL_NUM DESC LIMIT 1";
		statement = database_conn.prepareStatement(sql_statement);
		statement->setInt(1, cinema_id);
		res = statement->executeQuery();

		res->first();
		if (res->rowsCount() != 0) {
			hall_num = res->getInt("HALL_NUM");
		}

		delete statement;
		delete res;

	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}
	
	string columnName = "";
	string sql_insert_values = "";

	cout << "\n\t\tCinema : " << cinema_name;
	cout << "\n\t\tHall Number : " << ++hall_num;
		
	columnName = hallDataStruct[4]["columnDescription"];
	columnName += hallDataStruct[4]["format"];

	cout << "\n\t\t" << columnName << ": ";
	cout << "\n\t\t";
	int option_number = (int)hallDataStruct[4]["option"]["name"].size();
	for (int l = 0; l < option_number; l++)
	{
		columnName = hallDataStruct[4]["option"]["name"][l];
		int capacity = hallDataStruct[4]["option"]["capacity"][l];
		columnName += "(Max : " + to_string(capacity) + ")";
		cout << l + 1 << ". " << columnName << "\t\t";
	}

	cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
	hall_type_id = option_valid_int(option_number);
	--hall_type_id;
	
	//Hall Capacity
	int row_number = 0;
	int seat_per_row = 0;
	cout << "\n\t\tHall Capacity";
	while (1)
	{
		cout << "\n\t\tNumber of Row : ";
		row_number = option_valid_int(INT_MAX);
		cout << "\t\tNumber of Seat Per Row : ";
		seat_per_row = option_valid_int(INT_MAX);
		int capacity = row_number * seat_per_row;
		int maximum = hallDataStruct[4]["option"]["capacity"][hall_type_id];

		if (capacity < maximum) {
			break;
		}
		else{
			cout << "\n\t\tThe total number of seat is exceed the maximum requirement. Please reenter : ";
		}
	}

	string sql_statement = "INSERT INTO HALL(HALL_ID, HALL_NUM, NUM_ROW, NUM_SEAT_PER_ROW, HALL_TYPE, CINEMA_ID) VALUES (?, ?, ?, ?, ?, ?)";
	try {
		statement = database_conn.prepareStatement(sql_statement);
		statement->setInt(1, hall_id);
		statement->setInt(2, hall_num);
		statement->setInt(3, row_number);
		statement->setInt(4, seat_per_row);
		statement->setInt(5, hall_type_id);
		statement->setInt(6, cinema_id);
		int status = statement->executeUpdate();
		if (status == 1) {
			delete statement;
			cout << "\n\t\tHall";
			add_seat(row_number, seat_per_row, hall_id);
			pause();
		}
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}
}
void Hall::add_seat(int row_number, int seat_per_row, int last_hall_id)
{
	DatabaseConnection database_conn;
	sql::PreparedStatement* statement = NULL;
	string sql_insert = "";
	string hall_num = to_string(last_hall_id);

	for (int i = 1; i < row_number + 1; i++){
		for (int j = 1; j <= seat_per_row; j++){
			sql_insert += "(" + to_string(i) + "," + to_string(j) + "," + hall_num + "),";
		}
	}

	sql_insert = sql_insert.substr(0, sql_insert.size() - 1);
	string attribute = "INSERT INTO HALL_SEAT (SEAT_ROW, SEAT_NO, HALL_ID) VALUES " + sql_insert;
	try {
		statement = database_conn.prepareStatement(attribute);
		int number = statement->executeUpdate();
		cout << " and " << number << " number of seat inserted successfully";
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}

	delete statement;
}
void Hall::set_hall_id(int hall_id) {
	this->hall_id = hall_id;
}
int Hall::get_hall_id() {
	return this->hall_id;
}
void Hall::set_hall_number(int hall_number) {
	this->hall_num = hall_number;
}
int Hall::get_hall_number() {
	return this->hall_num;
}
void Hall::set_hall_type(string hall_type) {
	this->hall_type =  hall_type;
}
string Hall::get_hall_type() {
	return this->hall_type;
}