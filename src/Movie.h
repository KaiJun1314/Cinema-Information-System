#pragma once
#include<string>
#include<iostream>
#include"utils.h"
using namespace std;

json movieDataStruct{
	{
		{"columnType", "INT"},
		{"columnName", "MOVIE_ID"},
		{"columnDescription", "Movie ID"},
		{"format", ""},
		{"input", false},
		{"searchable", false},
		{"updatable", false},
		{"display", false}
	},
	{
		{"columnType", "STRING"},
		{"columnName", "MOVIE_TITLE"},
		{"columnDescription", "Movie Title"},
		{"format", ""},
		{"input", true},
		{"searchable", true},
		{"updatable", true},
		{"display", true}
	},
	{
		{"columnType", "STRING"},
		{"columnName", "MOVIE_GENRES"},
		{"columnDescription", "Movie Genres"},
		{"format", "(Separate by Comma)"},
		{"input", true},
		{"searchable", true},
		{"updatable", true},
		{"display", true}
	},
	{
		{"columnType", "INT"},
		{"columnName", "MOVIE_DURATION"},
		{"columnDescription", "Movie Duration"},
		{"format", "(in minute)"},
		{"input", true},
		{"searchable", true},
		{"updatable", true},
		{"display", true}
	},
	{
		{"columnType", "DATE"},
		{"columnName", "MOVIE_RELEASE_DATE"},
		{"columnDescription", "Movie Release Date"},
		{"format", "(YYYY-MM-DD)"},
		{"input", true},
		{"searchable", true},
		{"updatable", true},
		{"display", true}
	},
	{
		{"columnType", "STRING"},
		{"columnName", "MOVIE_SYNOPSIS"},
		{"columnDescription", "Movie Synopsis"},
		{"format", ""},
		{"input", true},
		{"searchable", false},
		{"updatable", true},
		{"display", true}
	},
	{
		{"columnType", "STRING"},
		{"columnName", "MOVIE_ACTOR"},
		{"columnDescription", "Movie Cast"},
		{"format", "(Separate by Comma)"},
		{"input", true},
		{"searchable", true},
		{"updatable", true},
		{"display", true}
	},
	{
		{"columnType", "STRING"},
		{"columnName", "MOVIE_DISTRIBUTOR"},
		{"columnDescription", "Distributor"},
		{"format", ""},
		{"input", true},
		{"searchable", true},
		{"updatable", true},
		{"display", true}
	},
	{
		{"columnType", "STRING"},
		{"columnName", "MOVIE_ORIGINAL_LANGUAGE"},
		{"columnDescription", "Original Language"},
		{"format", ""},
		{"input", true},
		{"searchable", true},
		{"updatable", true},
		{"display", true}
	}
};

#ifndef MOVIE_H
#define MOVIE_H

class Movie: public utils
{
	friend class MovieManage;

	private :
		int movie_id;
		string title;
		string genres;
		int duration;
		string release_date;
		string synopsis;
		string actor;
		string distributor;
		string orginal_language;

	public :
		Movie();
		void set_movie_id(int movie_id);
		int get_movie_id();
		int get_movie_duration();
		string get_movie_name();
		
		
};
#endif // !MOVIE_H

Movie::Movie()
{
	movie_id = 0;
	duration = 0;
	title = "";
	genres = "";
	release_date = "";
	synopsis = "";
	actor = "";
	distributor = "";
	orginal_language = "";
}
void Movie::set_movie_id(int movie_id) {
	this->movie_id = movie_id;
}
int Movie::get_movie_id()
{
	return movie_id;
}
int Movie::get_movie_duration() {
	return this->duration;
}

string Movie::get_movie_name() {
	return this->title;
}