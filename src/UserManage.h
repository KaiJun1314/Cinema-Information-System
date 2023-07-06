#pragma once
#include"User.h"
#include"Cinema.h"
#include<map>
using namespace std;

//Age Qualify for Customer
const int AGE_QUALIFY = 18;

#ifndef USERMANAGE_H
#define USERMANAGE_H

class UserManage : public utils
{
	private:
		DatabaseConnection conn;
		map<int, string>user_type;

		//For Customer
		bool user_qualify();
		bool check_manager(Cinema& cinema);

	public:
		UserManage();
		~UserManage() {};

		//User Manage Function
		void add_user(user_type_enum user_type);
		void edit_user(User& edit_user, bool manager, int cinema_id);
		void read_user(User& user, int user_id);
		void read_staff(Staff& staff, int user_id);
		void user_profile(User& user_profile, bool manager, bool display_only, int cinema_id);
		void delete_staff(Staff& staff_profile);

		//User Function 
		int user_login(user_type_enum& user_type);
		void user_password_forget();

		//For Staff
		int search_user(int& user_type);

		//User Interface
		int customer_menu(string username);
		int staff_menu(string username, Staff& staff, vector<int>& staff_choice);
		void transaction_history(int user_id);
};
#endif

UserManage::UserManage() {
	user_type.insert(pair<int, string>(1, "Admin"));
	user_type.insert(pair<int, string>(2, "Manager"));
	user_type.insert(pair<int, string>(3, "Cashier"));
};
//User Manage Function
void UserManage::add_user(user_type_enum type)
{
	clearScreen();
	if (type == Customer)
		cout << "\n\t\t********* Register *********";
	else
		cout << "\n\t\t********* Create Staff Account *********";

	cout << "\n\t\tPlease enter the following information : ";
	User new_user;
	bool qualify = true;
	bool email_and_phone_only = false;
	Cinema cinema;

	if (type == Customer)
		qualify = user_qualify();
	if (qualify)
	{
		//Set UserName
		cout << "\n\t\t";
		new_user.set_username();

		//Set User Email
		reenter:
		cout << "\t\t";
		if (type == Customer)
		{
			new_user.set_user_email();
			if (new_user.user_email.find("@cinema.com") != string::npos)
			{
				cout << "\n\t\tDomain cinena.com is for cinema staff";
				cout << "\n\t\tPlease enter your personal email";
				goto reenter;
			}
		}
		else {
			string email;
			cout << "Staff email prefix (@cinema.com is the domain) : ";
			cin >> email;
			email += "@cinema.com";
			new_user.user_email = email;
		}

		//Set User Phone Number
		cout << "\t\t";
		new_user.set_phone_number();

		if (email_and_phone_only)
			goto end;

		if (type == Customer)
		{
			cout << "\n\t\tReminder : The Password must contain special character and ditig or character\n";
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			new_user.set_password();
			new_user.user_role = Customer;
		}
		else
		{
			cout << "\n\t\tPassword : The default password for new staff is \"123456789\"";
			new_user.set_password("123456789");

			cout << "\n\t\tPosition";
			auto it = user_type.begin();
			for ( ; it != user_type.end(); ++it) {
				cout << "\n\t\t" << it->first << ". " << it->second;
			}
			cout << "\n\t\tPlease Enter Your Choice Here ==> ";
			new_user.user_role = (user_type_enum)option_valid_int(3);

			if (!cinema.search_cinema()) {
				return;
			}
			else {
				cinema.read_cinema();
			}
		}
	}
	else
	{
		cout << "\n\t\tYou are under qualify age ... ";
		pause();
		return;
	}
	
	end:
	try
	{
		while (new_user.user_role == 2)
		{
			if (check_manager(cinema))
			{
				cout << "\n\n\t\tNext step ??";
				cout << "\n\t\t1. Edit data";
				cout << "\n\t\t2. Back";
				cout << "\n\t\tEnter Your Choice Here ==> ";

				int index = option_valid_int(2);
				if (index == 1)
				{
					cout << "\n\t\tPosition : ";
					auto it = user_type.begin();
					for (; it != user_type.end(); ++it) {
						cout << "\n\t\t" << it->first << ". " << it->second;
					}
					cout << "\n\t\tPlease Enter Your Choice Here ==> ";
					new_user.user_role = (user_type_enum)option_valid_int(3);

					if (!cinema.search_cinema()) {
						return;
					}
					else {
						cinema.read_cinema();
					}
				}
				else {
					return;
				}
			}
			else {
				break;
			}
		}
		
		string sql_statement = "INSERT INTO ACCOUNT (USERNAME, PHONE, EMAIL, PASSWORD, ROLE";
		if (type == Customer) {
			sql_statement += ") VALUES(? , ? , ? , ? , ?)";
		}
		else {
			sql_statement += ", CINEMA_ID) VALUES(? , ? , ? , ? , ?, ?)";
		}

		sql::PreparedStatement * statement = conn.prepareStatement(sql_statement);
		statement->setString(1, new_user.username);
		statement->setString(2, new_user.phone_number);
		statement->setString(3, new_user.user_email);
		statement->setString(4, new_user.password);
		statement->setInt(5, (int)new_user.user_role);
		if (type != Customer) {
			statement->setInt(6, cinema.get_cinema_id());
		}

		int status = statement->executeUpdate();
		delete statement;
		if (status == 1){
			cout << "\n\t\tRegister Successfully ...";
			pause();
			return;
		}	
	}
	catch (sql::SQLException& e)
	{
		if (e.getErrorCode() == 1062) 
		{
			cout << "\n\t\tError : Email or Phone Number Is Registed Already";
			cout << "\n\t\tDo You Want to Try Again or Edit the data: ";
			cout << "\n\t\t1. Edit Data \t\t2.Exit";
			cout << "\n\t\tPlease Enter Your Choice Here ===> ";
			switch (option_valid_int(2))
			{
				case 1: 
				{
					goto reenter;
					email_and_phone_only = true;
					break; 
				}
				case 2: 
					return;
			}
		}
		else
		{
			cout << "\n\t\tError : " << e.what();
			pause();
			return;
		}
	}
}
void UserManage::edit_user(User& edit_user, bool manager = false, int cinema_id = 0)
{
	vector<int> user_edit;
	int choice = 0;
	int valid_choice = 1;
	if (manager) {
		cout << "\n\t\tEdit Which Information ??";
		goto manager_edit;
	}

	cout << "\n\t\t********* Information Edit *********";
	cout << "\n\t\tWhat information you which to edit : ";

	if (edit_user.user_role == Customer)
	{
	manager_edit:
		valid_choice = 4;
		user_edit.push_back(-1);
		cout << "\n\t\t" << ++choice << ". Username";

		user_edit.push_back(-2);
		cout << "\n\t\t" << ++choice << ". Email";

		user_edit.push_back(-3);
		cout << "\n\t\t" << ++choice << ". Phone Number";
		goto password;
	}
	else
	{
	password:
		user_edit.push_back(-4);
		cout << "\n\t\t" << ++choice << ". Password";
	}

	if (manager && edit_user.user_role != Customer) {
		valid_choice+=2;
		user_edit.push_back(-5);
		cout << "\n\t\t" << ++choice << ". Staff Position";

		user_edit.push_back(-6);
		cout << "\n\t\t" << ++choice << ". Staff Cinema";
	}

	cout << "\n\n\t\tPlease Enter Your Choice Here ====> ";
	choice = option_valid_int(valid_choice);
	choice = user_edit[--choice];

	string password;
	Cinema cinema;
	if (manager && choice == -4) {
		goto manager_reset_password;
	}

	while (1) 
	{
		cout << "\n\t\tPlease Enter Your Information :";
		switch (choice)
		{
			case -1:{
				cout << "\n\t\tNew ";
				edit_user.set_username();
				break;
			}
			case -2:{
				cout << "\n\t\tNew ";
				edit_user.set_user_email();
				break;
			}
			case -3:{
				cout << "\n\t\tNew ";
				edit_user.set_phone_number();
				break;
			}
			case -4:{
				cout << "\n\t\tPlease enter your current password : ";
				cin >> password;
				if (password == edit_user.password)
				{
					cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				manager_reset_password:
					edit_user.set_password();
					break;
				}
				else
				{
					cout << "\n\t\tPassword is not correct ... "
						<< "\n\t\tPlease Try Again";
					pause();
					return;
				}
			}
			case -6:{
				cinema:
				cinema.set_cinema_id(cinema_id);
				cinema.read_cinema();
				cout << "\n\t\tThe Staff currently work in " << cinema.get_cinema_name();
				cout << "\n\t\tRemain the same ??";
				if (!option_valid_char()) {
					cinema_again:
					cout << "\n\t\tStaff will transfer to : ";
					if (cinema.search_cinema()) {
						cinema_id = cinema.get_cinema_id();
					}
					else {
						return;
					}
				}
				if (choice == -6)
					goto role;
				break;
			}
			case -5:{
				role:
				auto it = user_type.find(edit_user.user_role);
				cout << "\n\t\tCurrent Position : " + it->second;
				for (it = user_type.begin(); it != user_type.end(); ++it) {
					cout << "\n\t\t" << it->first << ". " << it->second;
				}
				cout << "\n\t\t4. Remain the same ";
				cout << "\n\t\tPlease Enter Your Choice Here ==> ";
				int option = option_valid_int(4);
				if (option != 4) {
					edit_user.user_role = (user_type_enum)option;
				}
				if (choice == -5)
					goto cinema;
				break;
			}
		}

		cout << "\n\t\tAnother Information to edit ?? ";
		if (option_valid_char()) {
			cout << "\n\t\tEnter Your Choice Here ==> ";
			choice = option_valid_int(valid_choice);
			choice = user_edit[--choice];
		}
		else {
			break;
		}
	}

	try_again:
	sql::PreparedStatement* statement = NULL;
	int status;

	try 
	{
		if (choice == -5 || choice == -6) {
			if (edit_user.user_role == 2) {
				if (check_manager(cinema)) {

					cout << "\n\n\t\tNext step ??";
					cout << "\n\t\t1. Edit data";
					cout << "\n\t\t2. Back";
					cout << "\n\t\tEnter Your Choice Here ==> ";

					int index = option_valid_int(2);
					if (index == 1) {
						choice = -6;
						goto cinema_again;
					}
					else {
						return;
					}
				}
			}
		}

		int position = 5;
		string sql_statement = "UPDATE ACCOUNT SET USERNAME = ?, PHONE = ?, EMAIL = ?, PASSWORD = ? ";
		if (manager && edit_user.user_role != Customer) {
			sql_statement += ", ROLE = ? , CINEMA_ID = ? ";
		}
		
		sql_statement += "WHERE ACCOUNT_ID = ?";
		statement = conn.prepareStatement(sql_statement);
		statement->setString(1, edit_user.username);
		statement->setString(2, edit_user.phone_number);
		statement->setString(3, edit_user.user_email);
		statement->setString(4, edit_user.password);

		if (manager && edit_user.user_role != Customer) {
			position = 7;
			statement->setInt(5, edit_user.user_role);
			statement->setInt(6, cinema_id);
		}

		statement->setInt(position, edit_user.userID);
		status = statement->executeUpdate();
		if (status >= 1)
		{
			cout << "\n\t\tYour information is update Successfull .... ";
			pause();
			delete statement;
			return;
		}
		else {
			cout << "\n\t\tNo information is updated";
			pause();
		}
	}
	catch (sql::SQLException e) 
	{
		status = e.getErrorCode();
		if (status == 1062)
		{
			cout << "\n\t\tError : Phone Number or Email Is Registed Already";
			cout << "\n\t\tDo You Want to Edit the data: ";
			cout << "\n\t\t1.Edit Data \t\t2.Exit";
			cout << "\n\t\tPlease Enter Your Choice Here ===> ";
			switch (option_valid_int(2))
			{
				case 1:{
					if (choice == -2)
						edit_user.set_user_email();
					else
						edit_user.set_phone_number();
					goto try_again;
				}
				case 2:
					delete statement;
					return;
			}
		}
		else {
			cout << e.what();
			pause();
		}
	}
}
void UserManage::read_user(User& user, int user_id)
{
	try 
	{
		string sql_statement = "SELECT * FROM ACCOUNT WHERE ACCOUNT_ID = ? ";
		sql::PreparedStatement* statement = conn.prepareStatement(sql_statement);
		statement->setInt(1, user_id);
		sql::ResultSet * res = statement->executeQuery();

		res->first();
		user.userID = user_id;
		user.username = res->getString("USERNAME");
		user.phone_number = res->getString("PHONE");
		user.user_email = res->getString("EMAIL");
		user.set_password(res->getString("PASSWORD"));
		user.user_role = Customer;
		delete statement;
		delete res;
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}
	
}
void UserManage::read_staff(Staff& staff, int staff_id)
{
	try {
		string sql_statement = "SELECT * FROM ACCOUNT WHERE ACCOUNT_ID = ? ";
		sql::PreparedStatement * statement = conn.prepareStatement(sql_statement);
		statement->setInt(1, staff_id);
		sql::ResultSet * res = statement->executeQuery();

		res->first();
		staff.userID = staff_id;
		staff.username = res->getString("USERNAME");
		staff.phone_number = res->getString("PHONE");
		staff.user_email = res->getString("EMAIL");
		staff.set_password(res->getString("PASSWORD"));
		staff.cinema_id = res->getInt("CINEMA_ID");
		staff.user_role = (user_type_enum)res->getInt("ROLE");
		staff.set_permission();
		delete statement;
		delete res;
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}
}
void UserManage::user_profile(User& user_profile, bool manager = false, bool display_only = true, int cinema_id = 0) {

	clearScreen();
	cout << "\n\t\t********* Profile *********";
	cout << "\n\t\tUserName : " << user_profile.username;
	cout << "\n\t\tEmail Address : " << user_profile.user_email;
	cout << "\n\t\tPhone Number: " << user_profile.phone_number;

	if (!display_only) 
	{
		cout << "\n\n\t\tPlease choose the following operation : ";
		cout << "\n\t\t1. Edit Information";
		cout << "\n\t\t2. Back";
		cout << "\n\n\t\tPlease Enter Your Choice Here ====> ";
		int choice = option_valid_int(2);

		if (choice == 1) {
			this->edit_user(user_profile,manager, cinema_id);
		}
	}
	return;
}
void UserManage::delete_staff(Staff& staff_profile) {

	clearScreen();
	cout << "\n\t\t******** Delete Staff *******";
	cout << "\n\t\tStaff Name : " << staff_profile.get_username();
	cout << "\n\t\tPhone Number " << staff_profile.phone_number;
	cout << "\n\t\tEmail : " << staff_profile.user_email;
	cout << "\n\n\t\tAre You Sure to delete this staff ??";
	if (!option_valid_char()) {
		return;
	}
	else {
		try {
			sql::PreparedStatement* statement = NULL;
			statement = conn.prepareStatement("SET FOREIGN_KEY_CHECKS = 0");
			int status = statement->executeUpdate();
			delete statement;

			statement = conn.prepareStatement("DELETE FROM ACCOUNT WHERE ACCOUNT_ID = ?");
			statement->setInt(1, staff_profile.get_user_id());
			status = statement->executeUpdate();
			if (status >= 1) {
				cout << "\n\t\tThe Staff is removed successfully";
				pause();
			}
			delete statement;

			statement = conn.prepareStatement("SET FOREIGN_KEY_CHECKS = 1");
			status = statement->executeUpdate();
			delete statement;
		}
		catch (sql::SQLException e) {
			cout << e.what();
			pause();
		}
	}
}

//User Function
int UserManage::user_login(user_type_enum& user_type)
{
	string user_email = "";
	string password = "";
	string db_password = "";

	clearScreen();
	cout << "\n\t\t******** Login ********\n";
	cout << "\n\t\tPlease Enter your email and password";
	cout << "\n\t\tEmail : ";
	getline(cin, user_email);
	cout << "\t\tPassword : ";
	getline(cin, password);

	cout << "\n\t\tVerifying your account, please wait....\n";

	sql::PreparedStatement* statement =  NULL;
	sql::ResultSet* res = NULL;

	try 
	{
		string sql_statement = "SELECT PASSWORD, ACCOUNT_ID, ROLE FROM ACCOUNT WHERE EMAIL = ? ";
		statement = conn.prepareStatement(sql_statement);
		statement->setString(1, user_email);
		res = statement->executeQuery();

		if (res->rowsCount() != 1)
		{
			cout << "\n\t\tUnable to find this account.";
			pause();
			clearScreen();
			delete statement;
			delete res;
			return -1;
		}
		else
		{
			res->first();
			db_password = res->getString("PASSWORD");
			if (db_password == password)
			{
				cout << "\n\t\tYour are logged in";
				int type = res->getInt("ROLE");
				switch (type)
				{
					case (int)Customer: user_type = Customer;
						break;
					default:
						user_type = NONE;
				}
				pause();
				int account_id =res->getInt("ACCOUNT_ID");
				delete statement;
				delete res;
				return account_id;
			}
			else
			{
				cout << "\n\t\tYour Email or Password is not correct";
				delete statement;
				delete res;
				pause();
				return -1;
			}
		}
	}
	catch (sql::SQLException& e) {
		cout << "\n\t\tERROR: " << e.getErrorCode() << endl;
		cout << "\n\t\tERROR: " << e.what() << endl;
		pause();
		return 0;
	}
};
void UserManage::user_password_forget()
{
	User user;
	string temp = "";

	clearScreen();
	cout << "\n\t\t********* FORGET PASSWORD *********";
	cout << "\n\t\tPlease enter your Email Address : ";
	cout << "\n\t\t";
	user.set_user_email();
	temp = user.user_email;

	cout << "\n\t\tGetting your account information";
	string sql_statement = "SELECT ACCOUNT_ID, PHONE FROM ACCOUNT WHERE EMAIL = ? ";
	sql::PreparedStatement* statement = NULL;
	sql::ResultSet* res = NULL;

	try {
		statement = conn.prepareStatement(sql_statement);
		statement->setString(1, temp);
		res = statement->executeQuery();

		if (res->rowsCount() != 1)
		{
			cout << "\n\t\tUnable to find this account";
			delete statement;
			delete res;
			return;
		}
		else
		{
			res->first();
			cout << "\n\n\t\tPlease enter your Phone Number: ";
			cout << "\n\t\t";
			user.set_phone_number();
			temp = user.phone_number;

			if (temp == res->getString("PHONE"))
			{
				int account_id = res->getInt("ACCOUNT_ID");
				delete statement;
				delete res;
				cout << "\n\t\tYour phone number is correct";
				cout << "\n\t\tPlease enter your new password\n";
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				user.set_password();

				string sql = "UPDATE ACCOUNT SET PASSWORD = ? WHERE ACCOUNT_ID = ?";
				statement = conn.prepareStatement(sql);
				statement->setString(1, user.password);
				statement->setInt(2, account_id);
				int status = statement->executeUpdate();
				if (status >=  0) {
					cout << "\n\t\tPassword reset successfully ...";
				}
			}
			else
			{
				cout << "\n\t\tThe phone number entered is not correct";
				cout << "\n\t\tPlease contact system admin to reset yout password";
			}

			delete statement;
			return;
		}
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}
}

//For Customer
bool UserManage::user_qualify()
{
	cout << "\n\t\tBefore register, Please make sure that you are " << AGE_QUALIFY << " years of age or older ";
	return option_valid_char();
}

//For Staff
int UserManage::search_user(int& user_type)
{
	cout << "\n\t\t********* User Search *********";
	int choice = 0;
	if (user_type == 0) {
		cout << "\n\t\tSearch : ";
		cout << "\n\t\t1. Customer \t\t2. Staff";
		cout << "\n\t\t-1. Return";
		cout << "\n\n\t\tPlease Enter Your Choice Here ==> ";
		user_type = option_valid_int(2, -1);
	}

	if (user_type == -1) {
		return -1;
	}

	cout << "\n\t\tPlease input the name of the user : ";
	string user_name = "";
	cin.clear();
	getline(cin, user_name);

	int user_id = 0;
	string sql_statement = "SELECT ACCOUNT_ID, USERNAME AS Name, PHONE AS \"Phone Number\" FROM ACCOUNT WHERE ";

	if (user_type == 1)
		sql_statement += "ROLE = 0 AND USERNAME LIKE ?";
	else if (user_type == 2)
		sql_statement += "ROLE != 0 AND USERNAME LIKE ?";

	sql::PreparedStatement* statement = NULL;
	sql::ResultSet* res = NULL;

	try {
		statement = conn.prepareStatement(sql_statement);
		statement->setString(1, "%" + user_name + "%");
		res = statement->executeQuery();
		if (res->rowsCount() == 0) {
			cout << "\n\t\tSorry, No User Found";
			pause();
			return -1;
		}
		else {
			int index = display_in_table(res, false, "User");
			if (index == -1) {
				return -1;
			}
			res->absolute(index);
			int id = res->getInt(1);
			delete statement;
			delete res;
			return id;
		}
	}
	catch (sql::SQLException e) {
		cout << e.what();
		pause();
	}

	return -1;
}

//User Interface
int UserManage::customer_menu(string username)
{
	clearScreen();
	cout << "\n\t\tWelcome Back " << username;
	cout << "\n\t\tPlease choose the following operation : \n";
	cout << "\t\t1. Movies\n";
	cout << "\t\t2. Showtimes\n";
	cout << "\t\t3. Cinemas\n";
	cout << "\t\t4. Transaction History\n";
	cout << "\t\t5. My Profile\n";
	cout << "\t\t6. Log Out\n";

	cout << "\n\t\tPlease Enter Your Choice Here ====> ";
	return 6;
}
int UserManage::staff_menu(string username, Staff& user_profile, vector<int>& staff_choice)
{
	clearScreen();
	cout << "\n\t\tWelcome Back " << username;

	int index = 0; 
	int iterator = 0;
	for(int i : user_profile.module)
	{
		int module_to_access = 0;
		int permission = 0;
		permission = i % 10;
		module_to_access = i - permission;
		switch (module_to_access){
			case Movie_Module:{
				cout << "\n\n\t\t********* Movie *********";
				switch (permission){
					case All:{
						staff_choice.push_back(Movie_Module + Create);
						cout << "\n\t\t" << ++index << ". " << "Add Movie";

						staff_choice.push_back(Movie_Module + Edit);
						cout << "\n\t\t" << ++index << ". " << "Edit Movie";

						staff_choice.push_back(Movie_Module + Delete);
						cout << "\n\t\t" << ++index << ". " << "Delete Movie";
					}
					case Read_Only:{
						staff_choice.push_back(Movie_Module + Read);
						cout << "\n\t\t" << ++index << ". " << "Read Movie";
					}
				}
				break;
			}
			case Cinema_Module:{
				cout << "\n\n\t\t********* Cinema *********";
				switch (permission){
					case All:{
						staff_choice.push_back(Cinema_Module + Create);
						cout << "\n\t\t" << ++index << ". " << "Add Cinema";

						staff_choice.push_back(Cinema_Module + Edit);
						cout << "\n\t\t" << ++index << ". " << "Edit Cinema";

						staff_choice.push_back(Cinema_Module + Update_Price);
						cout << "\n\t\t" << ++index << ". Update Hall Price";
					}
					case Read_Only:{
						staff_choice.push_back(Cinema_Module + Read);
						cout << "\n\t\t" << ++index << ". " << "Read Cinema";
					}
				}
				break;
			}
			case Staff_Module:{
				cout << "\n\n\t\t********* Staff *********";
				switch (permission){
					case All:{
						staff_choice.push_back(Staff_Module + Create);
						cout << "\n\t\t" << ++index << ". " << "Add Staff";

						staff_choice.push_back(Staff_Module + Edit);
						cout << "\n\t\t" << ++index << ". " << "Edit Staff";

						staff_choice.push_back(Staff_Module + Delete);
						cout << "\n\t\t" << ++index << ". " << "Delete Staff";
					}
					case Read_Only:{
						staff_choice.push_back(Staff_Module + Read);
						cout << "\n\t\t" << ++index << ". " << "Read User";
					}
				}
				break;
			}
			case Ticket_Module: {
				cout << "\n\n\t\t********* Ticket Booking *********";
				switch (permission) {
				case All: {
					staff_choice.push_back(Ticket_Module + Create);
					cout << "\n\t\t" << ++index << ". " << "Book Ticket";
				}
				}
				break;
			}
			case Movie_Schedule: {
				cout << "\n\n\t\t********* Movie Schedule *********";
				switch (permission) {
				case All: {
					staff_choice.push_back(Movie_Schedule + Create);
					cout << "\n\t\t" << ++index << ". " << "Create Movie Schedule";
				}
				}
				break;
			}
			case Report_Module:{
				cout << "\n\n\t\t********* Report *********";
				switch (permission){
					case Cinema_report:{
						staff_choice.push_back(Report_Module + Cinema_report);
						cout << "\n\t\t" << ++index << ". " << "Cinema Report";
					}
					case User_report: {
						staff_choice.push_back(Report_Module + User_report);
						cout << "\n\t\t" << ++index << ". " << "User Report";
					}
					case Finance_report: {
						staff_choice.push_back(Report_Module + Finance_report);
						cout << "\n\t\t" << ++index << ". " << "Movie and Finance Report";
					}
				}
				break;
			}
		}
		iterator++;
	}

	cout << "\n\n\t\t********* My Account *********";
	staff_choice[index] = -1;
	cout << "\n\t\t" << ++index << ". My Profile";

	staff_choice[index] = -2;
	cout << "\n\t\t" << ++index << ". Logout";

	cout << "\n\n\t\tPlease Enter Your Choice Here ====> ";
	return index;
}
void UserManage::transaction_history(int user_id) 
{
	sql::PreparedStatement* statement = NULL;
	sql::ResultSet* res = NULL;

	try {
	string sql_statement = "SELECT BOOKING_ID, BOOKING_DATE_TIME AS \"Date and Time\", MOVIE_TITLE AS Movie, CINEMA_NAME AS \"Cinema Name\", ";
	sql_statement += "HALL_NUM AS \"Hall Number\", HALL_NAME AS Type, TOTAL_PRICE AS Price FROM BOOKING NATURAL JOIN MOVIE_SHOWTIME ";
	sql_statement += "NATURAL JOIN MOVIE NATURAL JOIN CINEMA NATURAL JOIN HALL JOIN HALL_TYPE WHERE HALL.HALL_TYPE = HALL_TYPE.HALL_TYPE AND BUYER_ID = ? ";
	sql_statement += "GROUP BY BOOKING_ID ORDER BY BOOKING_DATE_TIME ASC";
	statement = conn.prepareStatement(sql_statement);
	statement->setInt(1, user_id);
	res = statement->executeQuery();
	if (res->rowsCount() == 0) {
		cout << "\n\t\t********* Transaction History *********";
		cout << "\n\t\tNo Transaction History Found in this account";
	}
	else 
	{
		int index = display_in_table(res, false, "********* Transaction History *********");
		if (index != -1) 
		{
			res->absolute(index);
			int booking_id = res->getInt(1);
			string booking_date_time = res->getString(2);
			string movie_name = res->getString(3);
			string cinema_name = res->getString(4);
			int hall_num = res->getInt(5);
			string hall_type = res->getString(6);
			delete statement;
			delete res;

			clearScreen();
			cout << "\n\t\t********* Booking Detail *********";
			cout << left << setw(25) << "\n\t\t1. Booking Date and Time : " << booking_date_time;
			cout << left << setw(25) << "\n\t\t2. Cinema Name : " << cinema_name ;
			cout << left << setw(25) << "\n\t\t3. Hall Number : " << hall_num;
			cout << left << setw(25) << "\n\t\t4. Hall Type : " << hall_type ;
			cout << left << setw(25) << "\n\t\t5. Movie Name : " << movie_name;
			cout << endl;

			sql_statement = "SELECT SEAT_ROW AS Row, SEAT_NO AS \"Seat Number\", DEFAULT_PRICE as Price FROM  ";
			sql_statement += "BOOKING NATURAL JOIN TICKET NATURAL JOIN HALL JOIN HALL_TYPE NATURAL JOIN HALL_SEAT ";
			sql_statement += "WHERE BOOKING_ID = ? AND ";
			sql_statement += "HALL.HALL_TYPE = HALL_TYPE.HALL_TYPE AND ";
			sql_statement += "DEFAULT_PRICE = (SELECT DEFAULT_PRICE FROM HALL_TYPE WHERE HALL_TYPE = HALL.HALL_TYPE AND UPDATE_TIME < ? ORDER BY UPDATE_TIME DESC LIMIT 1) ";
			sql_statement += "GROUP BY SEAT_ID";
			statement = conn.prepareStatement(sql_statement);
			statement->setInt(1, booking_id);
			statement->setString(2, booking_date_time);
			res = statement->executeQuery();
			display_in_table(res, true, "Seat Detail");
			pause();
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
bool UserManage::check_manager(Cinema& cinema) {

	try {
		string sql_statement = "SELECT USERNAME, PHONE FROM ACCOUNT WHERE ROLE = ?  AND CINEMA_ID = ?";
		sql::PreparedStatement * statement = conn.prepareStatement(sql_statement);
		statement->setInt(1, 2);
		statement->setInt(2, cinema.get_cinema_id());
		sql::ResultSet* res = statement->executeQuery();
		if (res->rowsCount() >= 1)
		{
			cout << "\n\t\t" << cinema.get_cinema_name() << " have existing manager :";
			res->first();
			cout << "\n\t\tName : " << res->getString("USERNAME");
			cout << "\n\t\tPhone Number : " << res->getString("PHONE");
			delete statement;
			delete res;
			return true;
		}
		else {
			delete statement;
			delete res;
			return false;
		}
	}
	catch (sql::SQLException e) {
		cout << e.what();
	}
}