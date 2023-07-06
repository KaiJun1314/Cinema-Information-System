#pragma once
#include<iostream>
#include<string>
#include<mysql/jdbc.h>
using namespace std;

#define URL "tcp://127.0.0.1:3306"
#define USER "root"
#define PASSWORD ""
#define DATABASE "workshop"

#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

class DatabaseConnection
{
	private:
		sql::Connection* con;

	public:
		DatabaseConnection();
		~DatabaseConnection();
		inline sql::PreparedStatement* prepareStatement(string sql_statement);
};
#endif // !DATABASECONNECTION

DatabaseConnection::DatabaseConnection(){
	con = NULL;
	try {
		sql::Driver* driver = get_driver_instance();
		con = driver->connect(URL, USER, PASSWORD);
		con->setSchema(DATABASE);
	}
	catch (sql::SQLException& e) {
		cout << "\n\t\t# ERR: " << e.what();
		cout << "\n\t\t";
		system("pause");
	}
}

DatabaseConnection::~DatabaseConnection(){
	con->close();
	delete con;
}

inline sql::PreparedStatement* DatabaseConnection::prepareStatement(string sql_statement){
	return con->prepareStatement(sql_statement);
}
