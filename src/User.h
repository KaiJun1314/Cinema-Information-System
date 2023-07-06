#pragma once
#include<cstdio>
#include"utils.h"
using namespace std;

enum user_type_enum {Customer, Admin, Manager, Cashier, NONE};
enum system_module {Movie_Module = 10, Cinema_Module = 20, Report_Module = 30, Staff_Module = 40, Ticket_Module = 50, Movie_Schedule = 60};
enum permission {Read_Only = 1, All = 2};
enum function {Create = 1, Edit = 2, Read = 3, Delete = 4, Update_Price = 5};
enum report_type {Movie_report = 1, Cinema_report =2, Finance_report = 3, User_report = 4};

#ifndef USER_H
#define USER_H
class User
{
	friend class UserManage;
	protected:
		int userID;
		string username;
		string user_email;
		string phone_number;
		string password;
		user_type_enum user_role;

	public:
		User();
		~User() {};

		string get_username();
		user_type_enum get_user_role();
		int get_user_id();

		void set_user_id(int user_id);
		void set_username();
		void set_password();
		void set_user_email();
		void set_phone_number();
		void set_password(string password);
};
#endif

#ifndef STAFF_H
#define STAFF_H

class Staff : public User
{
	friend class UserManage;
	private:
		int cinema_id;

	public:
		Staff();
		~Staff() {};
		vector<int> module;
		int get_cinema_id();
		void set_permission();
};
#endif

//User
User::User() {
	userID = 0;
	username = "";
	user_email = "";
	phone_number = "";
	password = "";
}
string User::get_username()
{
	return username;
}
int User::get_user_id() {
	return this->userID;
}
user_type_enum User::get_user_role()
{
	return user_role;
}

void User::set_username()
{
	cout << "Username : ";
	cin.clear();
	getline(cin, username);
}
void User::set_password()
{
	string password = "";
	string password_confirm = "";
	bool contain_special = false;
	bool contain_normal = false;
	reenter:
	cout << "\t\tPassword : ";
	cin.clear();
	getline(cin, password);

	if (password.size() < 9)
	{
		cout << "\n\t\tThe length of password must more than 9";
		goto invalid;
	}
	
	for (int i = 0; i < password.size(); i++)
	{
		if ((password[i] >= 33 && password[i] <= 47) ||
			(password[i] >= 58 && password[i] <= 64) ||
			(password[i] >= 91 && password[i] <= 96) ||
			(password[i] >= 123 && password[i] <= 126))
		{
			contain_special = true;
		}
		else if ((password[i] >= 48 && password[i] <= 57) ||
			(password[i] >= 65 && password[i] <= 90) ||
			(password[i] >= 97 && password[i] <= 122)) 
		{
			contain_normal = true;
		}
		
		if (contain_special && contain_normal) {
			break;
		}
	}

	if(!contain_special || !contain_normal)
	{
		cout << "\n\t\tPassword must contain at least one special character and one digit or character";
		goto invalid;
	}

	cout << "\t\tConfirm Password : ";
	cin.clear();
	getline(cin, password_confirm);

	if (password != password_confirm)
	{
		invalid :
		cout << "\n\t\tPlease enter agian : \n";
		goto reenter;
	}
	else
		this->password = password;
}
void User::set_user_email()
{
	string email;
	cout << "Email: ";
	cin >> email;
	while (1)
	{
		size_t valid1 = email.find('@');
		size_t valid2 = email.find('.');

		if (valid1 == -1 || valid2 == -1)
		{
			cout << "\t\tYour email is not in correct format. Please enter agian : ";
			cin >> email;
		}
		else
			break;

	}
	this->user_email = email;
}
void User::set_phone_number()
{
	string phone;
	cout << "Phone Number (Without - ) : ";
	cin >> phone;
	bool valid = false;

	while (1)
	{
		if (phone.size() <= 11 && phone.size() >= 10)
		{
			for (int i = 0; i < phone.size(); i++)
			{
				if (isdigit(phone[i])){
					continue;
				}
				else {
					cout << "\n\t\tPhone number must contain digit only";
					goto again;
				}
			}
			break;
		}
		else
			cout << "\n\t\tThe phone number not in correct format";

		again:
		cout << "\n\t\tPlease reenter : ";
		cin >> phone;
	}
	this->phone_number = phone;
}
void User::set_password(string password)
{
	this->password = password;
}
void User::set_user_id(int user_id) {
	this->userID = user_id;
}

//Staff
Staff::Staff() {
	cinema_id = 0;
}
int Staff::get_cinema_id()
{
	return cinema_id;
}
void Staff::set_permission()
{
	switch (get_user_role()){
		case Admin:
		{
			module.push_back(Movie_Module + All);
			module.push_back(Cinema_Module + All);
			module.push_back(Staff_Module + All);
			module.push_back(Report_Module + Cinema_report);
			break;
		}
		case Cashier:
		{
			module.push_back(Movie_Module + Read_Only);
			module.push_back(Ticket_Module + All);
			module.push_back(Report_Module + Finance_report);
			break;
		}
		case Manager:
		{
			module.push_back(Movie_Module + Read_Only);
			module.push_back(Staff_Module + Read_Only);
			module.push_back(Movie_Schedule + All);
			module.push_back(Report_Module + User_report);
			break;
		}
	}
}