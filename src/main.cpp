#pragma once
#include <climits>
#include "UserManage.h"
#include "MovieManage.h"
#include "Report.h"

using namespace std;
bool movie_selected= false;

int main_menu()
{
	system("cls");
	cout << "\n\t\t******** Welcome to Jun Cinema ********";
	cout << "\n\t\tPlease choose the following operation : ";
	cout << "\n\t\t1. Login";
	cout << "\n\t\t2. Register";
	cout << "\n\t\t3. Forget Password";
	cout << "\n\t\t4. Check Movie Scheduling";
	cout << "\n\t\t5. Exit";

	cout << "\n\n\t\tPlease Enter Your Choice Here ====> ";
	return 5;
}

int main()
{
	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
	UserManage usermanage;
	MovieShowTime show;
	while (1)
	{
		int choice = 0;
		user_type_enum user_type = NONE;
		choice = main_menu();
		choice = usermanage.option_valid_int(choice);
		
		//Main Menu
		if(choice == 1)
		{
			login:
			int user_id = usermanage.user_login(user_type);
			if (user_id == -1) {
				continue;
			}

			if (user_type == Customer){
				User customer;
				usermanage.read_user(customer, user_id);
				while(1)
				{
					if (movie_selected) 
					{
						int status = show.get_movie_schedule(customer.get_user_id());
						movie_selected = false;
						if (status == -1) {
							show.reset(); 
							continue;
						}
						else {
							show.show_seat_and_book_ticket(customer.get_user_id());
							show.reset();
						}
					}

					choice = usermanage.customer_menu(customer.get_username());
					choice = usermanage.option_valid_int(choice);
					if (choice <= 3 && choice >= 1) {
						MovieShowTime show;
						if (choice == 1) {
							show.set_movie(false);
						}
						else if (choice == 2) {
							show.set_movie_date();
						}
						else if (choice == 3) {
							show.set_movie_cinema();
						}
						int status = show.get_movie_schedule(customer.get_user_id());
						if (status == -1) {
							show.reset();
							continue;
						}
						else {
							show.show_seat_and_book_ticket(customer.get_user_id());
						}
					}
					else if (choice == 4) {
						usermanage.transaction_history(customer.get_user_id());
					}
					else if (choice == 5){
						usermanage.user_profile(customer,false,false);
					}
					else if (choice == 6){
						cout << "\n\t\tLog Out Successfully ...";
						cout << "\n\t\t";
						system("pause");
						break;
					}
				}
			}
			else{
				Staff staff;
				usermanage.read_staff(staff, user_id);
				
				while(1){
					vector<int> staff_choice;
					choice = usermanage.staff_menu(staff.get_username(), staff, staff_choice);
					choice = usermanage.option_valid_int(choice);
					choice = staff_choice[--choice];
					
					if (choice == -1) {
						usermanage.user_profile(staff, false, false);
					}
					else if (choice == -2){
						cout << "\n\t\tLog Out Successfully ...";
						usermanage.pause();
						break;
					}
					
					int functionToAccess = choice % 10;
					int moduleToAccess = choice - functionToAccess;
					
					if (moduleToAccess == Movie_Module) {
						MovieManage movie;
						if (functionToAccess == Create) {
							movie.add_movie();
						}
						else {
							bool status = movie.search_movie();
							if (status) {
								movie.read_movie();
							}
							else {
								continue;
							}

							if (functionToAccess == Edit) {
								movie.edit_movie();
							}
							else if (functionToAccess == Read) {
									movie.clearScreen();
									movie.display_movie();
									movie.pause();
							}
							else if (functionToAccess == Delete) {
								movie.delete_movie();
							}
						}
					}
					else if (moduleToAccess == Staff_Module) {
						if (functionToAccess == Create) {
							usermanage.add_user(NONE);
						}
						else if (functionToAccess == Edit) {
							int user_type = 2;
							int staff_id = usermanage.search_user(user_type);
							if (staff_id == -1) {
								continue;
							}
							else 
							{
								Staff temp_staff;
								usermanage.read_staff(temp_staff, staff_id);
								usermanage.user_profile(temp_staff, true, false, temp_staff.get_cinema_id());
							}				
						}
						else if (functionToAccess == Read) {
							int user_type = 0;
							int user_id = usermanage.search_user(user_type);
							if (user_id != -1) 
							{
								if (user_type == 1) {
									User temp_customer;
									usermanage.read_user(temp_customer, user_id);
									usermanage.user_profile(temp_customer);
									cout << endl;
									usermanage.pause();
								}
								else
								{
									Staff temp_staff;
									usermanage.read_staff(temp_staff, user_id);
									usermanage.user_profile(temp_staff);
									cout << endl;
									usermanage.pause();
								}
							}
							else {
								continue;
							}
						}
						else if (functionToAccess == Delete) {
							int user_type = 2;
							int user_id = usermanage.search_user(user_type);
							if (user_id != -1)
							{
								Staff temp_staff;
								usermanage.read_staff(temp_staff, user_id);
								usermanage.delete_staff(temp_staff);
							}
							else {
								continue;
							}

						}
					}
					else if (moduleToAccess == Cinema_Module) {
						Cinema cinema;
						if (functionToAccess == Create) {
							cinema.add_cinema();
						}
						else if (functionToAccess == Update_Price) {
							show.update_default_price();
						}
						else {
							if (cinema.search_cinema()) {
								cinema.read_cinema();
							}
							else {
								continue;
							}

							if (functionToAccess == Edit) {
								cinema.edit_cinema();
							}
							else if (functionToAccess == Read) {
								cinema.display_cinema();
								cinema.pause();
							}
						}
						
					}
					else if (moduleToAccess == Report_Module) {
						Report rpt;
						if (functionToAccess == Cinema_report) {
							rpt.cinema();
						}
						else if (functionToAccess == Finance_report) {
							rpt.movie(staff.get_user_role(), staff.get_cinema_id(), staff.get_user_id());
						}
						else if (functionToAccess == User_report) {
							rpt.user(staff.get_user_role(), staff.get_cinema_id());
						}
					}
					else if (moduleToAccess == Ticket_Module) {
						MovieShowTime showTime;
						if (functionToAccess == Create) {
							showTime.set_cinema_id(staff.get_cinema_id());
							showTime.get_movie_schedule(staff.get_user_id());

							showTime.show_seat_and_book_ticket(staff.get_user_id());
						}
					}
					else if (moduleToAccess == Movie_Schedule) {
						MovieShowTime showTime;
						if (functionToAccess == Create) {
							showTime.create_movie_schedule(staff.get_cinema_id());
						}
					}
				}
			}
		}
		else if (choice == 2)
		{
			usermanage.add_user(Customer);
		}
		else if (choice == 3)
		{
			usermanage.user_password_forget();
			cout << "\n\t\t";
			system("pause");
		}
		else if (choice == 4)
		{
			int movie_show_id = show.get_movie_schedule(0);
			if (movie_show_id != -1) {
				movie_selected = true;
				goto login;
			}
			else {
				show.reset();
				continue;
			}
		}
		else if (choice == 5)
		{
			cout << "\n\t\tThank You ... ";
			exit(EXIT_SUCCESS);
		}
	}
	return 0;
}
