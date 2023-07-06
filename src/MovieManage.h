#pragma once
#include<string>
#include"Movie.h"
using namespace std;

#ifndef MOVIEMANAGE_H
#define MOVIEMANAGE_H
class MovieManage : public utils{

	friend class MovieShowTime;
	private :
		DatabaseConnection conn;
		Movie MovieModel;
		void input_movie_data(int choice);
		
	public :
		void add_movie();
		void edit_movie();
		void read_movie();
		void delete_movie();

		bool search_movie();
		void display_movie();

		Movie& get_movie();
};
#endif // !MOVIEMANAGE_H

void MovieManage::add_movie()
{
    int index = 0;
	bool loop = false;
	do 
    {
		clearScreen();
		cout << "\n\t\t********* Add Movie *********";
		cout << "\n\t\tPlease enter the following inforamtion : ";

		string attribute = "";
		cout << endl;
		for (int i = 0; i < movieDataStruct.size(); i++)
		{
			bool input = movieDataStruct[i]["input"];
			if (input)
			{
				string temp = movieDataStruct[i]["columnName"];
				attribute += temp + ",";
				temp = movieDataStruct[i]["columnDescription"];
				if (movieDataStruct[i]["format"] != "")
					temp += movieDataStruct[i]["format"];
				cout << "\t\t" << temp << " : ";
				input_movie_data(i);
			}
			else
				continue;
		}

		attribute = attribute.substr(0, attribute.size() - 1);
		string sql_statement = "INSERT INTO MOVIE (" + attribute + ") VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
		sql::PreparedStatement * statement = conn.prepareStatement(sql_statement);
		statement->setString(1, MovieModel.title);
		statement->setString(2, MovieModel.genres);
		statement->setInt(3, MovieModel.duration);
		statement->setString(4, MovieModel.release_date);
		statement->setString(5, MovieModel.synopsis);
		statement->setString(6, MovieModel.actor);
		statement->setString(7, MovieModel.distributor);
		statement->setString(8, MovieModel.orginal_language);

        try 
        {
            int status = statement->executeUpdate();
            delete statement;

            cout << "\n\t\tMore Movie to add ?? ";
            index += status;
		    loop = option_valid_char();
        }
        catch (sql::SQLException& e)
        {
            cout << "\n\t\tError : " << e.what();
            pause();
        }
	} 
	while (loop);
	cout << "\n\t\t" << index << " Movie added successfully";
	pause();
}
void MovieManage::read_movie()
{
	try
	{
		string sql_statement = "SELECT * FROM MOVIE WHERE MOVIE_ID = ?";
		sql::PreparedStatement * statement = conn.prepareStatement(sql_statement);
		statement->setInt(1, MovieModel.movie_id);
		
		sql::ResultSet * res = statement->executeQuery();
		res->first();
		MovieModel.title = res->getString("MOVIE_TITLE");
		MovieModel.genres = res->getString("MOVIE_GENRES");
		MovieModel.duration = res->getInt("MOVIE_DURATION");
		MovieModel.release_date = res->getString("MOVIE_RELEASE_DATE");
		MovieModel.synopsis = res->getString("MOVIE_SYNOPSIS");
		MovieModel.actor = res->getString("MOVIE_ACTOR");
		MovieModel.distributor = res->getString("MOVIE_DISTRIBUTOR");
		MovieModel.orginal_language = res->getString("MOVIE_ORIGINAL_LANGUAGE");

		delete statement;
		delete res;
	}
	catch (sql::SQLException& e) {
		cout << "\n\t\t" << e.getErrorCode();
		cout << "\n\t\t" << e.what();
	}
}
void MovieManage::edit_movie()
{
	clearScreen();
	display_movie();
	cout << "\n\n\t\t********* Movie Information Edit *********";
	vector<int>choice;
	string columnName = "";

	//Generate Menu
	int no_of_attr = menu_generate(movieDataStruct, choice, "updatable");
	bool loop = false;

	do {

		cout << "\n\n\t\tPlease Enter Your Choice Here ====> ";
		int i = option_valid_int(no_of_attr);
		int index = choice[--i];

		cout << "\n\t\tPlease Enter the new information :";
		columnName = movieDataStruct[index]["columnDescription"];
		if (movieDataStruct[index]["format"] != "")
			columnName += movieDataStruct[index]["format"];
		cout << "\n\t\t" << columnName << " : ";
		input_movie_data(choice[i]);

		cout << "\n\t\tAnother Movie Data To Edit ??";
		loop = option_valid_char();
		if(loop){
			cout << "\n\t\tPlease Choose another attribute";
		}
	} while (loop);
	
	string sql_statement = "UPDATE MOVIE SET " + create_sql_attr(movieDataStruct, "updatable");

    try
	{
        sql_statement += " WHERE MOVIE_ID = ?";
        sql::PreparedStatement* statement = conn.prepareStatement(sql_statement);
        statement->setString(1, MovieModel.title);
        statement->setString(2, MovieModel.genres);
        statement->setInt(3, MovieModel.duration);
        statement->setString(4, MovieModel.release_date);
        statement->setString(5, MovieModel.synopsis);
        statement->setString(6, MovieModel.actor);
        statement->setString(7, MovieModel.distributor);
        statement->setString(8, MovieModel.orginal_language);
        statement->setInt(9, MovieModel.movie_id);

        int status = statement->executeUpdate();
		delete statement;
        if (status >= 1) {
            cout << "\n\t\t" << "Movie : " << MovieModel.title;
            cout << "\n\t\t" << "Updated Successfully";
			pause();
		}
		else {
			cout << "\n\t\tNo Information is updated";
			pause();
		}
    }
    catch (sql::SQLException& e)
	{
        cout << "\n\t\tError : " << e.what();
        pause();
	}
}
void MovieManage::delete_movie() {

	clearScreen();
	cout << "\n\n\t\t********* Delete Movie *********";
	display_movie();
	cout << "\n\n\t\tConfirm to delete this movie ??";
	if (option_valid_char()) {
		sql::PreparedStatement* statement = NULL;
		try {
			statement = conn.prepareStatement("DELETE FROM MOVIE WHERE MOVIE_ID = ?");
			statement->setInt(1, MovieModel.movie_id);
			int status = statement->executeUpdate();
			if (status >= 1) {
				cout << "\n\t\tMovie Deleted Successfully";
				pause();
			}
		}
		catch (sql::SQLException e) {
			int code  = e.getErrorCode();
			if (code == 1451) {
				cout << "\n\t\tThis movie is not allow to remove because it is scheduled for movie show";
			}
			else {
				cout << e.what();
			}
			pause();
		}

		delete statement;
	}
	else {
		return;
	}
}

bool MovieManage::search_movie()
{
	MovieModel.movie_id = search_function(movieDataStruct, "MOVIE", "Movie", "No Movie Found");
	if (MovieModel.movie_id == -1) {
		MovieModel.movie_id = 0;
		return false;
	}
	else
	{
		return true;
	}
}
void MovieManage::input_movie_data(int choice)
{
	switch (choice)
	{
	case 1: getline(cin, MovieModel.title);
		break;
	case 2: getline(cin, MovieModel.genres);
		break;
	case 3: cin >> MovieModel.duration;
		break;
	case 4:
	{
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		MovieModel.release_date = set_date();
		break;
	}
	case 5: getline(cin, MovieModel.synopsis);
		break;
	case 6: getline(cin, MovieModel.actor);
		break;
	case 7: getline(cin, MovieModel.distributor);
		break;
	case 8: getline(cin, MovieModel.orginal_language);
		break;
	}
}
void MovieManage::display_movie()
{
	cout << "\n\t\t" << MovieModel.title;
	cout << "\n\t\t" << MovieModel.genres << " - " << MovieModel.duration << " Minute - " << MovieModel.orginal_language;
	cout << left << setw(15) << "\n\t\tRelease Date" << " : " << MovieModel.release_date;
	cout << left << setw(15) << "\n\t\tCast" << " : " << MovieModel.actor;
	cout << left << setw(15) << "\n\t\tDistributor" << " : " << MovieModel.distributor;
	cout << left << setw(15) << "\n\t\tSynopsis" << " : \n";
	display_string(MovieModel.synopsis, 100);
}

Movie& MovieManage::get_movie() {
	return this->MovieModel;
}