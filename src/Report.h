#pragma once
#include"MovieShowTime.h"
#include<fstream>

#ifndef REPORT_H
#define REPORT_H
class Report : public utils 
{
	private :
		DatabaseConnection database_conn;

		//General Function
		void list_report(json dataStruct, string entity, string report_name);
		string date_range();
		string custom_column(json dataStruct);
		string custom_order(json dataStruct, string column_name);
		void print_to_file(sql::ResultSet* result, string file_name);
		int report_choice();
		int operation_choice();
		void display_percentage(sql::ResultSet* result, int detail_to_display_position);

		//Report List
		void cinema_finance_report();
		void movie_finance_report(user_type_enum user_type, int cinema_id);
		
	public :
		void cinema();
		void movie(user_type_enum user_type, int cinema_id,int staff_id);
		void user(user_type_enum user_type, int cinema_id);
};
#endif

void Report::cinema() {

	clearScreen();
	cout << "\n\t\t********* Cinema Report *********";
	cout << "\n\t\t1. Cinema List";
	cout << "\n\t\t2. Cinema Sales Report";
	cout << "\n\t\t-1. Return";
	cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
	int choice = option_valid_int(2, -1);
	if (choice == 1) {
		list_report(cinemaDataStruct, "CINEMA", "Cinema Report");
	}
	else if (choice == 2) {
		cinema_finance_report();
	}
	else if (choice == -1) {
		return;
	}
}
void Report::movie(user_type_enum user_type, int cinema_id, int staff_id) {

	if (user_type == Cashier) {
		sql::PreparedStatement* statement = NULL;
		sql::ResultSet* res = NULL;

		if (user_type == Cashier) {
			string sql_statement = "SELECT SUM(TOTAL_PRICE) FROM BOOKING WHERE BUYER_ID = ? AND BOOKING_DATE_TIME LIKE ?";
			statement = database_conn.prepareStatement(sql_statement);
			statement->setInt(1, staff_id);
			statement->setString(2, "%" + return_current_time_and_date(true, 0) + "%");
			res = statement->executeQuery();

			if (res->next()) {
				cout << "\n\t\tToday your sales is RM " << res->getDouble(1);
			}
			else {
				cout << "\n\t\tToday your sales is RM 0.00";
			}
			pause();
			delete statement;
			delete res;
			return;
		}
	}

	clearScreen();
	cout << "\n\t\t********* Movie Report *********";
	cout << "\n\t\t1. Movie List";
	cout << "\n\t\t2. Movie Sales Report";
	cout << "\n\t\t-1. Return";
	cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
	int choice = option_valid_int(2, -1);
	if (choice == 1) {
		list_report(movieDataStruct, "MOVIE", "Movie Report");
	}
	else if (choice == 2) {
		movie_finance_report(user_type, cinema_id);
	}
	else if (choice == -1) {
		return;
	}
}
void Report::user(user_type_enum user_type, int cinema_id)
{
	int choice;
	if (user_type == Cashier) {
		cout << "\n\t\t********* Customer Report *********";
		choice = 1;
	}
	else {
		cout << "\n\t\t********* User Report *********";
		cout << "\n\t\tReport For :";
		cout << "\n\t\t1. Customer";
		cout << "\n\t\t2. Staff";
		cout << "\n\t\t-1. Return";
		cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
		choice = option_valid_int(2, -1);
	}

	string report_name = "";
	string sql_statement;
	if (choice == 1)
	{
		sql_statement = "SELECT ACCOUNT_ID AS \"Customer Id\", USERNAME AS \"Customer Username\", ";
		sql_statement += "PHONE AS \"Phone Number\", EMAIL AS Email FROM ACCOUNT WHERE ROLE = 0";
		report_name = "Customer Particular Report";

	}
	else if (choice == 2)
	{
		Cinema cinema;
		if (user_type == Admin) {
			cinema.search_cinema();
			cinema_id = cinema.get_cinema_id();
		}
		else {
			cinema.set_cinema_id(cinema_id);
			cinema.read_cinema();
		}

		sql_statement = "SELECT ACCOUNT_ID AS \"Staff Account Id\", USERNAME AS \"Name\", ";
		sql_statement += "PHONE AS \"Phone Number\", EMAIL AS Email FROM ACCOUNT NATURAL JOIN CINEMA ";
		sql_statement += "WHERE ROLE != 0 AND CINEMA_ID = " + to_string(cinema_id);

		report_name = "Staff Particular Report";
		report_name += "\n\t\tBranch Name : " + cinema.get_cinema_name();
		report_name = "\n\t\tAddress : " + cinema.get_cinema_address();
	}
	else if (choice == -1) {
		return;
	}

	sql::PreparedStatement* statement = NULL;
	sql::ResultSet* res = NULL;
	try
	{
		statement = database_conn.prepareStatement(sql_statement);
		res = statement->executeQuery();
		if (res->rowsCount() == 0) {
			cout << "\n\t\tSorry, No Result found" << endl;
			pause();
		}
		else
		{
			clearScreen();
			display_in_table(res, true, report_name);
			int choice = operation_choice();
			if (choice == 1) {
				print_to_file(res, report_name);
			}
		}
	}
	catch (sql::SQLException& e) {
		cout << "\n\t\t" << e.what() << endl;
		pause();
	}

	delete statement;
	delete res;
	return;
}
int Report::operation_choice() {
	cout << "\n\n\t\tPlease choose the following operation : ";
	cout << "\n\t\t1. Export to file";
	cout << "\n\t\t2. Back";
	cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
	return option_valid_int(2);
}

void Report::list_report(json dataStruct, string entity, string report_name)
{
	string sql_condition = "";
	vector<string> data = advance_search_function(dataStruct, sql_condition);
	if (sql_condition == "-1") {
		return;
	}
	else
	{
		string attr = "";
		string order = "";
		int choice = report_choice();
		if (choice == 1) {
			attr = create_sql_attr(dataStruct);
		}
		else {
			attr = custom_column(dataStruct);
			order = custom_order(dataStruct, "");
		}

		sql::PreparedStatement* statement = NULL;
		sql::ResultSet* res = NULL;
		try
		{
			string sql_statement = "SELECT " + attr + " FROM " + entity + sql_condition + order;
			statement = database_conn.prepareStatement(sql_statement);
			for (int i = 0; i < data.size(); i++) {
				statement->setString(i + 1, data[i]);
			}

			res = statement->executeQuery();
			if (res->rowsCount() == 0) {
				cout << "\n\t\tSorry, No Result found" << endl;
				pause();
			}
			else
			{
				clearScreen();
				display_in_table(res, true, report_name);
				int choice = operation_choice();
				if (choice == 1) {
					print_to_file(res, report_name);
				}
			}
		}
		catch (sql::SQLException e) {
			cout << e.what();
			pause();
		}

		delete statement;
		delete res;
		return;
	}
}
void Report::print_to_file(sql::ResultSet* result, string file_name) {

	file_name += to_string(time(0)) + ".tsv";
	string output = "C:\\Users\\Wong\\Desktop\\" + file_name;
	ofstream outFile(output, ios::out);
	sql::ResultSetMetaData* meta_data = result->getMetaData();
	int number_of_column = meta_data->getColumnCount();

	outFile << meta_data->getColumnName(1);
	for (int i = 2; i <= number_of_column; i++) {
		outFile << "\t" << meta_data->getColumnLabel(i);
	}
	outFile << "\n";

	result->beforeFirst();
	while (result->next())
	{
		int type = meta_data->getColumnType(1);
		if (type == 4) {
			outFile << result->getInt(1);
		}
		else {
			outFile << (string)result->getString(1);
		}

		for (int i = 2; i <= number_of_column; i++) {
			type = meta_data->getColumnType(i);
			if (type == 4) {
				outFile << "\t" << result->getInt(i);
			}
			else {
				outFile << "\t" << (string)result->getString(i);
			}
		}
		outFile << "\n";
	}
    outFile.close();
	cout << "\n\t\tThe file export successfully !!!";
	cout << "\n\t\tFile Name is : " << file_name;
	pause();
}
string Report::date_range() {

	string date_range = "";
	cout << "\n\t\tPlease choose the Date range : ";
	cout << "\n\t\t1. 1 Month";
	cout << "\n\t\t2. 3 Month";
	cout << "\n\t\t3. Custom Date";
	cout << "\n\t\t-1. Return";
	cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";

	int choice = option_valid_int(3, -1);
	if (choice == -1) {
		return "-1";
	}
	else if (choice == 3) {
		
		cout << "\n\t\tPlease enter the date in YYYY-MM-DD format : ";
	again:
		cout << "\n\t\tFrom : ";
		string date = set_date();
		time_t check = date_time_to_timestamp(date);
		if (time(0) < check)
		{
			cout << "\n\t\tPlease Enter Date before today";
			goto again;
		}

	second_again:
		cout << "\t\tTo : ";
		string second_date = set_date();
		check = date_time_to_timestamp(second_date);
		if (time(0) < check)
		{
			cout << "\n\t\tPlease Enter Date before today";
			goto second_again;
		}
	
		date_range += "BETWEEN \'" + date + "\'";
		date_range += " AND \'" + second_date +"\'";
		return date_range;
	}
	else {
		int num_day = 0;
		if (choice == 1)
			num_day = -30;
		else if (choice == 2)
			num_day = -90;

		date_range += "BETWEEN \'" + return_current_time_and_date(true, num_day) + "\'";
		date_range += " AND \'" + return_current_time_and_date(true, 1) +"\'";
		return date_range;
	}
}

void Report::cinema_finance_report() {

	cout << "\n\t\tCinema Financial Report : ";
	string date_condition = date_range();
	if (date_condition == "-1") {
		return;
	}

	string attr = "";
	string order = "";
	int choice = report_choice();
	if (choice == 1) {
		attr = create_sql_attr(cinemaDataStruct);
	}
	else {
		attr = custom_column(cinemaDataStruct);
		order = custom_order(cinemaDataStruct, "Sales");
	}

	attr += ", SUM(TOTAL_PRICE) AS Sales";

	string sql_statement = " SELECT " + attr + " FROM BOOKING NATURAL JOIN MOVIE_SHOWTIME ";
	sql_statement += "NATURAL JOIN HALL NATURAL JOIN CINEMA ";
	sql_statement += "WHERE BOOKING_DATE_TIME " + date_condition + " GROUP BY CINEMA_ID" + order;
	
	try
	{
		sql::PreparedStatement* statement = database_conn.prepareStatement(sql_statement);
		sql::ResultSet* res = statement->executeQuery();
		if (res->rowsCount() == 0) {
			cout << "\n\t\tSorry, No Result found" << endl;
			pause();

			delete statement;
			delete res;
			return;
		}
		else
		{
			clearScreen();
			string from_date = date_condition.substr(9, 10);
			string tmp = "Cinema Sales Report";
			string report_name = tmp += " From " + from_date + " To ";
			int find = date_condition.find_first_of("D");
			string to_date = date_condition.substr(find + 3, 10);
			report_name += to_date;

			display_in_table(res, true, report_name);
			display_percentage(res, 2);
			int choice = operation_choice();
			if (choice == 1) {
				print_to_file(res, tmp);
			}
			else if (choice == 2) {
				delete res;
				return;
			}

			delete statement;
			delete res;
			return;
		}
	}
	catch (sql::SQLException& e) {
		cout << "\n\t\t" << e.what() << endl;
		pause();
	}
}
int Report::report_choice() {
	cout << "\n\t\t1. Default Report";
	cout << "\n\t\t2. Custom Report";
	cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
	int choice = option_valid_int(2);
	return choice;
}
string Report::custom_column(json dataStruct) {
	
	vector<int>choice;
	vector<int>choose;
	cout << "\n\t\tPlease Choose the column you which to display";
	int valid_choice = menu_generate(dataStruct, choice, "all");
	cout << "\n\t\t" << ++valid_choice << " . That's all";
	cout << "\n\n\t\tEnter Your Choice Here : ";
	cout << "\n\t\tPress return after key in your choice, otherwise it will consider not valid\n";

	int index;
	reenter:
	while (1)
	{
		cout << "\t\tEnter Here : ";
		index = option_valid_int(valid_choice);
		if (index == valid_choice) {
			break;
		}
		else {
			choose.push_back(choice[--index]);
		}
	}

	if (choose.empty()) {
			cout << "\n\t\tPlease choose at least one";
			goto reenter;
	}
	sort(choose.begin(), choose.end());
	choose.erase(unique(choose.begin(), choose.end()), choose.end());
	string attr;
	string column_name = dataStruct[1]["columnName"];
	string label = dataStruct[1]["columnDescription"];
	attr += column_name + " AS \"" + label + "\",";

	for (int i = 0; i < choose.size(); i++) {
		index = choose[i];
		if(index != 1){
			column_name = dataStruct[index]["columnName"];
			label = dataStruct[index]["columnDescription"];
			attr += column_name + " AS \"" + label + "\",";
		} 
	}

	attr = attr.substr(0, attr.size() - 1);
	return attr;
}
string Report::custom_order(json dataStruct, string column_name = "") {

	cout << "\n\t\tOrder By : ";
	vector<int>choice;
	int num = menu_generate(dataStruct, choice, "all");

	if (column_name != "") {
		cout << "\n\t\t" << ++num << " ." << column_name;
	}

	string name;
	cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
	int index = option_valid_int(num);

	if (index == num) {
		name = column_name;
	}
	else {
		index = choice[--index];
		name = dataStruct[index]["columnName"];
	}

	string attr = " ORDER BY \"" + name + "\"";
	
	cout << "\n\t\tOption : ";
	cout << "\n\t\t1. Ascending";
	cout << "\n\t\t2. Descending";
	cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
	int option = option_valid_int(2);
	if (option == 1) {
		attr += " ASC";
	}
	else {
		attr += " DESC";
	}

	return attr;
}
void Report::movie_finance_report(user_type_enum user_type, int cinema_id) {

	cout << "\n\t\tMovie Financial Report : ";
	string entity = "";
	string date_condition = date_range();
	if (date_condition == "-1") {
		return;
	}

	string attr = "";
	string order = "";
	int choice = report_choice();
	if (choice == 1) {
		attr = create_sql_attr(movieDataStruct);
	}
	else {
		attr = custom_column(movieDataStruct);
		order = custom_order(movieDataStruct, "Sales");
	}

	attr += ", SUM(TOTAL_PRICE) AS Sales";
	
	Cinema cinema;
	string sql_condition = "WHERE BOOKING_DATE_TIME " + date_condition;
	if (user_type == Manager) {
			sql_condition += " AND CINEMA_ID = " + to_string(cinema_id);
		}
	else if(user_type == Admin){
			cout << "\n\t\tDisplay For : ";
			cout << "\n\t\t1. Paticular cinema branch";
			cout << "\n\t\t2. Total of all cinema branch";
			cout << "\n\t\t-1. Return";
			cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
			int choice = option_valid_int(2, -1);
			if (choice == -1) {
				return;
			}
			else if (choice == 1) {
				cinema.search_cinema();
				cinema_id = cinema.get_cinema_id();
				sql_condition += " AND CINEMA_ID = " + to_string(cinema_id);
			}
			else if (choice == 2) {
				cinema_id = -1;
			}
		}
	sql_condition += " GROUP BY MOVIE_ID";

	string sql_statement = "SELECT " + attr + " FROM BOOKING NATURAL JOIN MOVIE_SHOWTIME NATURAL JOIN MOVIE NATURAL JOIN "; 
	sql_statement += "HALL NATURAL JOIN CINEMA " + sql_condition + order;
	try
	{
		sql::PreparedStatement* statement = database_conn.prepareStatement(sql_statement);
		sql::ResultSet* res = statement->executeQuery();
		if (res->rowsCount() == 0) {
			cout << "\n\t\tSorry, No Result found" << endl;
			pause();

			delete statement;
			delete res;
			return;
		}
		else
		{
			clearScreen();
			string from_date = date_condition.substr(9, 10);
			string tmp = "Movie Sales Report";
			string report_name = tmp += " From " + from_date + " To ";
			int find = (int)date_condition.find_first_of("D") + 3;
			string to_date = date_condition.substr(find, 10);
			report_name += to_date;

			if (user_type == Manager) {
				report_name += " (" + cinema.get_cinema_name() + ")";
				report_name += "\n\t\tAddress : " + cinema.get_cinema_address();
			}
			else if(user_type == Admin){
				if (cinema_id != -1) {
					report_name += " (" + cinema.get_cinema_name() + ")";
					report_name += "\n\t\tAddress : " + cinema.get_cinema_address();
				}
			}

			display_in_table(res, true, report_name);
			display_percentage(res, 2);
			int choice = operation_choice();
			if (choice == 1) {
				print_to_file(res, tmp);
			}
			
			delete statement;
			delete res;
			return;
		}
	}
	catch (sql::SQLException e) {
		cout << e.what();
	}
}
void Report::display_percentage(sql::ResultSet* result, int detail_to_display_position) {
	
	sql::ResultSetMetaData * meta = result->getMetaData();
	int last = meta->getColumnCount();

	tabulate::Table table;
	double total = 0.0;
	vector<double>individual;
	vector<string>name;
	result->beforeFirst();
	while (result->next()) 
	{
		string tmp = result->getString(detail_to_display_position);
		name.push_back(tmp);

		double temp = result->getDouble(last);
		individual.push_back(temp);
		total += temp;
	}

	for (int i = 0; i < individual.size(); i++) 
	{
		double data = individual[i]/total * 100;
		stringstream ss;
		ss << fixed << setprecision(2) << data;
		string percentage((int)data, '#');
		Row_t row;
		row.push_back(name[i]);
		row.push_back(percentage);
		row.push_back(ss.str()  + " %");
		table.add_row(row);
	}
	table.format().corner("").border_top(" ").border_left("").border_right("").border_bottom("");
	for (auto& row : table) {
		row[0].format().border_left("\t\t");
		row[1].format().font_color(tabulate::Color::cyan);
	}

	cout << "\n\t\tTotal : RM " << total;
	cout << endl << table << endl;
}