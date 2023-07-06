#pragma once
#include"Movie.h"
#include"Cinema.h"
#include<ctime>
#include<sstream>
#include<algorithm>
#include<math.h>
using namespace std;

// ad time + clean time
const int time_between_movieshow = 30;

#ifndef MOVIESHOWTIME_H
#define MOVIESHOWTIME_H
class MovieShowTime: public utils
{
	private :
		DatabaseConnection database_conn;
		MovieManage movie;
		Cinema cinema;

		int movie_show_id;
		int duration;
		string show_date;
		string start_time;
		string start_time_12hours;

		void set_hall();
		void set_movie_show_id();
		int show_hall(string title);
		void show_booking_receipt(int booking_id, string booking_date_time, double price, int number);

	public :
		MovieShowTime();
		MovieShowTime(int manager_cinema_id);

		//Set Require Data
		void set_movie_date();
		void set_movie_cinema();
		void set_movie(bool with_date);

		//Manager Create Movie Schedule
		void create_movie_schedule(int manager_cinema_id);

		//Customer Make Booking
		int get_movie_schedule(int user_id);
		void ticket_book(int user_id, vector<int>ticket_choosed, int price);
		void show_seat_and_book_ticket(int user_id);
		void reset();
		void update_default_price();

		void set_cinema_id(int cinema_id);
};
#endif

MovieShowTime::MovieShowTime()
{
	start_time = "";
	show_date = "";
	show_date = "";
	duration = 0;
	start_time_12hours = "";

}
void MovieShowTime::reset() 
{
	start_time = "";
	show_date = "";
	duration = 0;
	cinema.cinema_id = 0;
	start_time_12hours = "";
	movie.get_movie().set_movie_id(0);
}
MovieShowTime::MovieShowTime(int manager_cinema_id)
{
	cinema.cinema_id = manager_cinema_id;
	show_date = "";
	start_time = "";
	duration = 0;
}

void MovieShowTime::set_movie_date()
{
	cout << "\n\t\tMovie Schedule";
	cout << "\n\t\tPlease Select the Date : ";
	cout << "\n\t\t";
	for (int i = 0; i <= 4; i++)
		cout << i + 1 << ". " << return_current_time_and_date(true, i) << "\t";

	cout << endl << "\t\t";
	for (int i = 5; i <= 7; i++)
		cout << i + 1 << ". " << return_current_time_and_date(true, i) << "\t";

	cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
	int choice = option_valid_int(8);
	show_date = return_current_time_and_date(true, --choice);
}
void MovieShowTime::set_movie_cinema()
{
	sql::PreparedStatement* statement = NULL;
	sql::ResultSet* res = NULL;

	cout << "\n\t\tPlease Select the Cinema : ";
	string sql_statement = "SELECT CINEMA_ID, CINEMA_NAME AS Name, CINEMA_ADDRESS AS Address FROM CINEMA";

	try {
		statement = database_conn.prepareStatement(sql_statement);
		res = statement->executeQuery();
		if (res->rowsCount() == 0) {
			cinema.cinema_id = 0;
		}
		else {
			int index = display_in_table(res, false, "Cinema");
			if (index == -1) {
				cinema.cinema_id = -1;
			}
			else {
				res->absolute(index);
				cinema.cinema_id = res->getInt(1);
				cinema.read_cinema();
			}
		}
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}

	delete statement;
	delete res;
}
void MovieShowTime::set_movie(bool with_date = true) {

	sql::PreparedStatement* statement = NULL;
	sql::ResultSet* res = NULL;

	string sql_statement = "SELECT MOVIE_ID, MOVIE_TITLE AS Title, MOVIE_GENRES AS Genres, MOVIE_DURATION AS Duration, ";
	sql_statement += "MOVIE_ORIGINAL_LANGUAGE AS Language FROM MOVIE_SHOWTIME NATURAL JOIN MOVIE NATURAL JOIN CINEMA ";

	try
	{
		if (with_date) {
			sql_statement += "WHERE SHOW_DATE = ? AND CINEMA_ID = ? GROUP BY MOVIE_ID";
			statement = database_conn.prepareStatement(sql_statement);
			statement->setString(1, show_date);
			statement->setInt(2, cinema.cinema_id);
		}
		else {
			sql_statement += "WHERE SHOW_DATE BETWEEN ? AND ? GROUP BY MOVIE_ID";
			statement = database_conn.prepareStatement(sql_statement);
			statement->setString(1, return_current_time_and_date(true, 0));
			statement->setString(2, return_current_time_and_date(true, 7));
		}

		res = statement->executeQuery();
		if (res->rowsCount() == 0) {
			cout << "\n\t\tNo Movie on Show";
			movie.get_movie().set_movie_id(-1);
		}
		else {
			int index = display_in_table(res, false, "Movie on Show");
			if (index == -1) {
				movie.get_movie().set_movie_id(-1);
			}
			else {
				res->absolute(index);
				movie.get_movie().set_movie_id(res->getInt("MOVIE_ID"));
				movie.read_movie();
			}
		}
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}

	delete statement;
	delete res;
}
void MovieShowTime::set_hall() {

	sql::PreparedStatement* statement = NULL;
	sql::ResultSet* res = NULL;

	string sql_statement = "SELECT HALL_ID, HALL_NUM AS \"Hall Number\", HALL_NAME AS Type, SHOW_DATE AS \"Movie Show Date\", ";
	sql_statement += "TIME_FORMAT(SHOW_TIME, '%h:%i %p') AS \"Start Time\", DURATION AS Duration FROM MOVIE_SHOWTIME NATURAL JOIN MOVIE NATURAL JOIN CINEMA ";
	sql_statement += "NATURAL JOIN HALL JOIN HALL_TYPE WHERE HALL.HALL_TYPE = HALL_TYPE.HALL_TYPE AND SHOW_DATE = ? AND CINEMA_ID = ? AND MOVIE_ID = ? ";
	sql_statement += "GROUP BY MOVIE_SHOW_ID";

	try
	{
		statement = database_conn.prepareStatement(sql_statement);
		statement->setString(1, show_date);
		statement->setInt(2, cinema.cinema_id);
		statement->setInt(3, movie.get_movie().get_movie_id());
		res = statement->executeQuery();

		if (res->rowsCount() == 0) {
			cout << "\n\t\tThis is movie is not scheduled in the cinema";
			pause();
		}
		else {
			int index = display_in_table(res, false, "Hall");
			if (index == -1) {
				movie.get_movie().set_movie_id(-1);
			}
			else {
				res->absolute(index);
				cinema.get_hall().set_hall_id(res->getInt(1));
				cinema.get_hall().set_hall_number(res->getInt(2));
				cinema.get_hall().set_hall_type(res->getString(3));
				show_date = res->getString(4);
				start_time = res->getString(5);
				start_time_12hours = res->getString(5);

				string tmp_time;
				int h1 = (int)start_time[1]  - '0';
				int h2 = (int)start_time[0] - '0';
				int hh = (h2 * 10 + h1 % 10);
				if (start_time[6] == 'A') {
					if (hh == 12) {
						tmp_time = "00";
						tmp_time += start_time.substr(2, 3);
					}
					else {
						tmp_time = start_time.substr(0, 5);
					}
				}
				else {
					if (hh == 12) {
						tmp_time = "12";
						tmp_time += start_time.substr(2, 3);
					}
					else {
						hh += 12;
						tmp_time = to_string(hh);
						tmp_time += start_time.substr(2, 3);
					}
				}
				tmp_time += ":00";
				start_time = tmp_time;
			}
		}
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}

	delete statement;
	delete res;
}
void MovieShowTime::set_movie_show_id() {

	try {
		string sql_statement = "SELECT MOVIE_SHOW_ID FROM MOVIE_SHOWTIME WHERE SHOW_DATE = ? AND HALL_ID = ? AND MOVIE_ID = ? AND SHOW_TIME = ?";
		sql::PreparedStatement* statement = database_conn.prepareStatement(sql_statement);
		statement->setString(1, show_date);
		statement->setInt(2, cinema.get_hall().get_hall_id());
		statement->setInt(3, movie.get_movie().get_movie_id());
		statement->setString(4, start_time);

		sql::ResultSet* res = statement->executeQuery();
		res->first();
		movie_show_id = res->getInt("MOVIE_SHOW_ID");
		
		delete statement;
		delete res;
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}	
}

int MovieShowTime::get_movie_schedule(int user_id = 0)
{
	if (movie.get_movie().get_movie_id() == -1) {
		return -1;
	}

	if (cinema.cinema_id == 0 && show_date == "") {
		set_movie_date();
		set_movie_cinema();
	}
	else if (show_date == "") {
		set_movie_date();
	}
	else if (cinema.cinema_id == 0){
		set_movie_cinema();
	}
	

	if (cinema.cinema_id == -1) {
		this->reset();
		return -1;
	}

	clearScreen();
	cinema.display_cinema();
	cout << "\n\t\tDate : " << show_date << endl;

	if (movie.get_movie().get_movie_id() == 0) {
		set_movie();
		if (movie.get_movie().get_movie_id() == -1) {
			pause();
			return -1;
		}
	}
	
	clearScreen();
	cinema.display_cinema();
	cout << "\n\t\tDate : " << show_date << endl;
	movie.display_movie();
	cout << endl;
	
	if (user_id != 0) {
		set_hall();
		if (cinema.get_hall().get_hall_id() == 0) {
			return -1;
		}
		set_movie_show_id();
	}
	else 
	{
		cout << "\n\t\tWanna book a ticket ?? Login Now !!!";
		if (option_valid_char()) {
			return cinema.get_hall().get_hall_id();
		}
		else {
			return -1;
		}
	}
}
void MovieShowTime::create_movie_schedule(int manager_cinema_id)
{
	sql::PreparedStatement* statement = NULL;
	sql::ResultSet* res = NULL;

	cinema.set_cinema_id(manager_cinema_id);
	clearScreen();
	cout << "\n\t\t********* Set Movie Schedule *********";
	cout << "\n\t\tPlease enter the following information : ";
	
	//Set Movie
	MovieManage movieModel;
	bool result = movieModel.search_movie();
	if (!result) {
		return;
	}

	movieModel.read_movie();
	duration = movieModel.get_movie().get_movie_duration();
	duration += time_between_movieshow;

	//Set Date
	dateAgain:
	cout << "\n\t\tDate (YYYY-MM-DD) : ";
	show_date = set_date();
	time_t date_timestamp = date_time_to_timestamp(show_date);
	if (date_timestamp < time(0))
	{
		cout << "\n\t\tSorry, You cannot schedule movie show time for today or previous date";
		cout << "\n\t\tPlease enter another date : ";
		goto dateAgain;
	}

	show_date = show_date.substr(0, 10);
	//Display current schedule
	int choice = show_hall("Current Movie Schedule");
	if (choice == 0) {
		cout << endl;
		goto proceed;
	}

	cout << "\n\t\tPlease Choose The Following Operation : ";
	cout << "\n\t\t1. Continue";
	cout << "\n\t\t2. Select another date ";
	cout << "\n\t\t-1. Return";
	cout << "\n\t\tPlease Enter your choice here ==> ";
	choice = option_valid_int(2, -1);
	if (choice == 2) {
		goto dateAgain;
	}
	else if (choice == -1) {
		return;
	}

proceed:
	try {
		//Set Hall
		string hall_attr = create_sql_attr(hallDataStruct, "display");
		string sql_statement = "SELECT HALL_ID, " + hall_attr + " FROM HALL JOIN HALL_TYPE WHERE HALL.HALL_TYPE = HALL_TYPE.HALL_TYPE AND CINEMA_ID = ? GROUP BY HALL_ID";
		statement = database_conn.prepareStatement(sql_statement);
		statement->setInt(1,manager_cinema_id);
		res = statement->executeQuery();
		if (res->rowsCount() == 0) {
			cout << "\n\t\tNo Hall in this cinema";
			pause();
			return;
		}
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}
	
	int hall_id = display_in_table(res, false, "Hall");
	if (hall_id == -1) {
		return;
	}
	else {
		res->absolute(hall_id);
		hall_id = res->getInt(1);
	}
	cinema.get_hall().set_hall_id(hall_id);
	delete statement;
	delete res;

	timeAgain:
	cout << "\n\t\tSet Time - 24 hr format (HH:MM) : ";
	start_time = set_time();
	string date_time = show_date + " " + start_time;
	time_t date_time_timestamp = date_time_to_timestamp(date_time);
	if (date_time_timestamp < time(0))
	{
		cout << "\n\t\tSorry, You cannot schedule movie show time for previous time";
		cout << "\n\t\tPlease enter another date and time: ";
		goto timeAgain;
	}

	int status = 0;
	try{
		string sql_statement = "SELECT SHOW_TIME, DURATION FROM MOVIE_SHOWTIME WHERE HALL_ID = ?";
		sql_statement += " AND SHOW_TIME <= ? AND SHOW_DATE = ? ORDER BY SHOW_TIME DESC LIMIT 1";
		statement = database_conn.prepareStatement(sql_statement);
		statement->setInt(1, hall_id);
		statement->setString(2, start_time);
		statement->setString(3, show_date);
		res = statement->executeQuery();
	}
	catch (sql::SQLException& e) {
		cout << "\n\t\tError Code : " << e.what();
		pause();
	}

	if (res->rowsCount() == 1)
	{
		//Schedule Time in timestamp
		string date_time_temp = show_date + " " + start_time;
		time_t scehedule_time = date_time_to_timestamp(date_time_temp);

		//Movie Schedule in Database
		res->first();
		int duration_db = res->getInt("DURATION");
		date_time_temp = show_date + " " + res->getString("SHOW_TIME");

		time_t check_time_start = date_time_to_timestamp(date_time_temp);
		time_t check_time_end = check_time_start + (static_cast<time_t>(duration_db) * 60);

		if (scehedule_time > check_time_start && scehedule_time < check_time_end || 
			scehedule_time == check_time_start || scehedule_time == check_time_end)
		{
			cout << "\n\t\tThis time slot is scheduled for a movie show please reenter ";
			cout << "\n\t\t1. Enter another date";
			cout << "\n\t\t2. Enter another time";
			cout << "\n\t\t-1. Return";
			cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
			int choice = option_valid_int(2, -1);
			if (choice == 1)
				goto dateAgain;
			else if (choice == 2)
				goto timeAgain;
			else if (choice == -1)
				return;
			pause();
		}
	}

	delete statement;
	delete res;

	string sql_statement = "INSERT INTO MOVIE_SHOWTIME (MOVIE_ID, HALL_ID, SHOW_DATE, SHOW_TIME, DURATION) VALUES (?, ?, ?, ?, ?)";
	statement = database_conn.prepareStatement(sql_statement);
	statement->setInt(1, movieModel.get_movie().get_movie_id());
	statement->setInt(2, cinema.get_hall().get_hall_id());
	statement->setString(3, show_date);
	statement->setString(4, start_time);
	statement->setInt(5, duration);
	status = statement->executeUpdate();

	if (status == 1){
		cout << "\n\t\tMovie Schedule Inserted Succesfully";
		pause();
		delete statement;
		return;
	}
}
void MovieShowTime::show_seat_and_book_ticket(int user_id) {

	sql::PreparedStatement* statement = NULL;
	sql::ResultSet* res = NULL;

	int number_of_rows = 0;
	int seat_per_rows = 0;
	int price = 0;
	try
	{
		string sql_statement = "SELECT NUM_ROW, NUM_SEAT_PER_ROW, DEFAULT_PRICE FROM HALL JOIN HALL_TYPE ";
		sql_statement += "WHERE HALL.HALL_TYPE = HALL_TYPE.HALL_TYPE AND ";
		sql_statement += "DEFAULT_PRICE = (SELECT DEFAULT_PRICE FROM HALL_TYPE WHERE HALL_TYPE = HALL.HALL_TYPE ORDER BY UPDATE_TIME DESC LIMIT 1) AND  ";
		sql_statement += "HALL_ID = ? ";
		statement = database_conn.prepareStatement(sql_statement);
		statement->setInt(1, cinema.get_hall().get_hall_id());
		res = statement->executeQuery();
		res->first();
		number_of_rows = res->getInt("NUM_ROW");
		seat_per_rows = res->getInt("NUM_SEAT_PER_ROW");
		price = res->getInt("DEFAULT_PRICE");

		delete statement;
		delete res;
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}

	try {
		string sql_statement = "SELECT SEAT_ID, SEAT_ROW, SEAT_NO ";
		sql_statement += "FROM MOVIE_SHOWTIME NATURAL JOIN HALL JOIN HALL_TYPE NATURAL JOIN HALL_SEAT ";
		sql_statement += "WHERE HALL.HALL_TYPE = HALL_TYPE.HALL_TYPE AND ";
		sql_statement += "MOVIE_SHOW_ID = ? AND ";
		sql_statement += "SEAT_ID NOT IN (SELECT SEAT_ID FROM TICKET NATURAL JOIN BOOKING WHERE MOVIE_SHOW_ID = ?) ";
		sql_statement += "GROUP BY SEAT_ID";

		statement = database_conn.prepareStatement(sql_statement);
		statement->setInt(1, movie_show_id);
		statement->setInt(2, movie_show_id);
		res = statement->executeQuery();
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}

	vector<vector<int>>seat_matrix(number_of_rows);
	vector<int> temp(seat_per_rows);
	fill(temp.begin(), temp.end(), -1);
	fill(seat_matrix.begin(), seat_matrix.end(), temp);

	while (res->next()) {
		int id = res->getInt("SEAT_ID");
		int row = res->getInt("SEAT_ROW");
		int seat = res->getInt("SEAT_NO");
		seat_matrix[--row][--seat] = id;
	}

	delete statement;
	delete res;
	tabulate::Table table;
	
	int index = 0;
	int row_index = 65;
	for (int i = 0; i < number_of_rows; i++) {
		Row_t row;
		char row_char = static_cast<char>(row_index++);
		string temp;
		temp.push_back(row_char);
		row.push_back(temp);

		for (int j = 0; j < seat_per_rows; j++) {
			index++;
			temp = to_string(index);
			row.push_back(temp);
		}
		table.add_row(row);
	}

	table.format().corner("").border_top(" ").border_bottom(" ")
		.font_align(tabulate::FontAlign::center);

	for (int i = 0; i < seat_matrix.size(); i++)
	{
		vector<int>::iterator it = seat_matrix[i].begin();
		it = find(it, seat_matrix[i].end(), -1);
		while (it != seat_matrix[i].end())
		{
			int index = (int)(it - seat_matrix[i].begin());
			table[i][(size_t)index + 1].format().font_background_color(tabulate::Color::red);
			it++;
			it = find(it, seat_matrix[i].end(), -1);
		}
	}
	for (int i = 0; i < number_of_rows; i++) {
		table[i][0].format().border_left("\t\t");
	}

	string space((seat_per_rows +1) * 5 / 2, ' ');
	cout << "\n\n\t\t" << space << "<<< Screen Here >>>";
	cout << endl << table << endl;
	cout << "\n\t\tRed Colour is the booked seat, and not allowed to be choose";
	cout << "\n\t\tYou may choose more than one chooice, press return after one choice.";
	cout << "\n\t\tPress -1 at the end of your choice";
	vector<int>ticket_choosed;

	again:
	cout << "\n\t\tPlease Enter Your Choice Here ==> ";
	while (1) 
	{
		int choice = option_valid_int(index, -1);
		if (choice == -1) {
			break;
		}
		else
		{
			int row, seat;
			if (choice < seat_per_rows) {
				row = 0;
				seat = choice-1;
			}
			else if(choice % seat_per_rows == 0) {
				row = choice / seat_per_rows - 1;
				seat = seat_per_rows - 1;
			}
			else {
				row = (int)floor(choice / seat_per_rows);
				seat = choice - (row * seat_per_rows);
				seat--;
			}

			int id = seat_matrix[row][seat];
			if(id != -1){
				ticket_choosed.push_back(id);
				cout << "\t\tPlease Enter Your Choice Here ==> ";
			}
			else {
				cout << "\n\t\tThe Seat cannot be choose. Please Chosee another : ";
				cout << "\n\t\tPlease Enter Your Choice Here ==> ";
			}
		}
	}

	vector<int>::iterator ip = unique(ticket_choosed.begin(), ticket_choosed.end());
	ticket_choosed.resize(distance(ticket_choosed.begin(), ip));
	if (ticket_choosed.size() > 0) {
		cout << "\n\t\tYour have booked " << ticket_choosed.size() << " number(s) of ticket";
		cout << "\n\t\tSeat Number\n\t\t";
		for (int i : ticket_choosed) {
			cout << i << "\t";
		}

		cout << "\n\t\tEach ticket price is RM " << price;
		price *= (int)ticket_choosed.size();
		cout << "\n\t\tTotal Price of your ticket is : RM " << price;
		cout << "\n\t\tConfirm ??";
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		if (option_valid_char()) {
			ticket_book(user_id, ticket_choosed, price);
		}
		else {
			ticket_choosed.clear();
			cout << "\n\t\tThe ticket you have choosed in cleared. Please reenter";
			goto again;
		}
	}
	else {
		cout << "\n\t\tNo Ticket Booked";
		pause();
	}
}

int MovieShowTime::show_hall(string title)
{
	sql::PreparedStatement* statement = NULL;
	sql::ResultSet* res = NULL;

	string attr = "SELECT HALL_NUM, HT.HALL_NAME, SHOW_DATE, SHOW_TIME, DURATION FROM MOVIE_SHOWTIME NATURAL JOIN CINEMA NATURAL JOIN HALL H JOIN HALL_TYPE HT ";
	attr += "WHERE H.HALL_TYPE = HT.HALL_TYPE AND SHOW_DATE = ? AND CINEMA_ID = ? GROUP BY MOVIE_SHOW_ID";

	try {
		statement = database_conn.prepareStatement(attr);
		statement->setString(1, show_date);
		statement->setInt(2, cinema.cinema_id);
		res = statement->executeQuery();
		if (res->rowsCount() == 0) {
			cout << "\n\t\tNo Movie Scheduled in " << show_date;
			return 0;
		}
		else {
			display_in_table(res, true, title);
		}
	}
	catch (sql::SQLException e) {
		cout << e.what();
	}

	delete statement;
	delete res;

	return 0;
}
void MovieShowTime::ticket_book(int user_id, vector<int>ticket_choosed, int price)
{
	sql::PreparedStatement* statement = NULL;

	int last_booking_id = generate_id("BOOKING");
	last_booking_id++;
	
	try 
	{
		string sql_statement = "INSERT INTO BOOKING VALUES (?, ?, current_timestamp(), ?, ?)";
		statement = database_conn.prepareStatement(sql_statement);
		statement->setInt(1, last_booking_id);
		statement->setInt(2, movie_show_id);
		statement->setInt(3, price);
		statement->setInt(4, user_id);
		int status = statement->executeUpdate();

		if (status == 1) 
		{
			delete statement;
			sql_statement = "INSERT INTO TICKET (SEAT_ID, BOOKING_ID) VALUES ";
			string booking_id = to_string(last_booking_id);
			for (int seat_id : ticket_choosed) {
				sql_statement += "(" + to_string(seat_id) + "," + booking_id + "),";
			}

			sql_statement = sql_statement.substr(0, sql_statement.size() - 1);
			statement = database_conn.prepareStatement(sql_statement);
			int status = statement->executeUpdate();
			if (status >= 1) {
				cout << "\n\t\tYour booking is confirmed";
				cout << "\n\t\tDo you want a receipt ??";
				if (option_valid_char()) {
					show_booking_receipt(last_booking_id, return_current_time_and_date(), price,(int)ticket_choosed.size());
				}
				else {
					cout << "\n\t\tThank you, enjoy your movie";
					pause();
				}
			}
			delete statement;
		}
		else {
			delete statement;
		}
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}
}
void MovieShowTime::update_default_price() 
{

	try {
		string sql_statement = "SELECT HALL_NAME AS Type, DEFAULT_PRICE AS Price FROM HALL_TYPE S1 ";
		sql_statement += "WHERE UPDATE_TIME = (SELECT MAX(UPDATE_TIME) FROM HALL_TYPE S2 WHERE S1.HALL_TYPE = S2.HALL_TYPE) ";
		sql_statement += "ORDER BY UPDATE_TIME";

		sql::PreparedStatement* statement = database_conn.prepareStatement(sql_statement);
		sql::ResultSet* res = statement->executeQuery();
		display_in_table(res, true, "Current Hall Price");

		delete statement;
		delete res;

		cout << "\n\n\t\tHall Price to update : ";
		int option_number = (int)hallDataStruct[4]["option"]["name"].size();
		for (int l = 0; l < option_number; l++)
		{
			string columnName = hallDataStruct[4]["option"]["name"][l];
			cout << "\n\t\t" << l + 1 << ". " << columnName;
		}

		cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
		option_number = option_valid_int(option_number);
		option_number--;
		string hall_name = hallDataStruct[4]["option"]["name"][option_number];
		cout << "\n\t\tPlease Enter the New Price : ";
		double price;
		cin >> price;

		sql_statement = "INSERT INTO HALL_TYPE (HALL_TYPE, DEFAULT_PRICE, HALL_NAME) VALUES (?, ?, ?)";
		statement = database_conn.prepareStatement(sql_statement);
		statement->setInt(1, option_number);
		statement->setDouble(2, price);
		statement->setString(3, hall_name);
		int status = statement->executeUpdate();
		if (status >= 1) {
			cout << "\n\t\tPrice Updated Successfully";
			pause();
		}

		delete statement;
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}
	
}

void MovieShowTime::show_booking_receipt(int booking_id, string booking_date_time, double price, int number) {

	clearScreen();
	tabulate::Table table;
	table.add_row(Row_t{ this->cinema.name });
	table.add_row(Row_t{ this->cinema.address });
	table.add_row(Row_t{ "\nReceipt" });
	table.add_row(Row_t{ "\nBooking No# : " + to_string(booking_id) });
	table.add_row(Row_t{ "Booking Date and Time : "+ booking_date_time + "\n\n" });

	tabulate::Table price_display;
	double individual_price = price / number;
	stringstream stream;
	stream << fixed << setprecision(2) << individual_price;

	price_display.add_row(Row_t{ "Quantity", "Price" });
	price_display.add_row(Row_t{ to_string(number), stream.str()});
	price_display.add_row(Row_t{ "" });

	stringstream stream2;
	stream2 << fixed << setprecision(2) << price;
	price_display.add_row(Row_t{ "Total", "RM " + stream2.str()});
	price_display.format().border("").corner("");

	tabulate::Table nested_table;
	nested_table.add_row(Row_t{ this->movie.get_movie().get_movie_name() });
	nested_table.add_row(Row_t{ "Hall " + to_string(this->cinema.get_hall().get_hall_number()) + " (" + this->cinema.get_hall().get_hall_type() + ")\n\n" });
	nested_table.add_row(Row_t{ "Show Date : " + this->show_date });
	nested_table.add_row(Row_t{ "Show Time : " + this->start_time_12hours });
	nested_table.format().border("").corner("");

	tabulate::Table total;
	total.format().corner("").hide_border();
	total.add_row({ nested_table, price_display });

	table.add_row(Row_t{ total });
	table.add_row(Row_t{ "\n\nThank you for purchasing tickes" });
	table.add_row(Row_t{ "Enjoy watching your movie !" });
	table.add_row(Row_t{ "Hotline : 1300-11-1212" });

	table.format().border_bottom("").border_right("").border_top("").corner("").width(50);
	table[0].format().font_align(tabulate::FontAlign::center);
	table[1].format().font_align(tabulate::FontAlign::center);
	table[2].format().font_align(tabulate::FontAlign::center).font_style({ tabulate::FontStyle::dark });
	table[6].format().font_align(tabulate::FontAlign::center);
	table[7].format().font_align(tabulate::FontAlign::center);
	table[8].format().font_align(tabulate::FontAlign::center);

	for (auto& row : table) {
		row[0].format().border_left("\t\t");
	}

	cout << endl << table << endl;
	pause();

}
void MovieShowTime::set_cinema_id(int cinema_id) {
	this->cinema.set_cinema_id(cinema_id);
	this->cinema.read_cinema();
}