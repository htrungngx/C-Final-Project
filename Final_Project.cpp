#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
using namespace std;

class BookCard
{
protected:
	const unsigned int MAXLENGTH = 256;
	string id;
	string name;
	string author;
public:
	void SetId()
	{
		cout << "|   Enter book id (max 256 char, can NOT be empty) : ";
		string r;
		do
		{
			getline(cin, r);
		} while (r.length() >= MAXLENGTH || r.empty());
		id = r;
	}
	void SetId(const string& id)
	{
		this->id = id;
	}
	string GetId()
	{
		return id;
	}
	void SetName()
	{
		cout << "|   Enter book name (max 256 char, can NOT be empty) : ";
		string r;
		do
		{
			getline(cin, r);
		} while (r.length() >= MAXLENGTH || r.empty());
		name = r;
	}
	void SetName(const string& name)
	{
		this->name = name;
	}
	string GetName()
	{
		return name;
	}
	void SetAuthor()
	{
		cout << "|   Enter book author (max 256 char, can NOT be empty) : ";
		string r;
		do
		{
			getline(cin, r);
		} while (r.length() >= MAXLENGTH || r.empty());
		author = r;
	}
	void SetAuthor(const string& author)
	{
		this->author = author;
	}
	string GetAuthor()
	{
		return author;
	}
	void Print() const
	{
		cout << "|     Book's id     : " << id << endl;
		cout << "|     Book's name   : " << name << endl;
		cout << "|     Book's author : " << author << endl;
	}
};

class BookRecord : public BookCard
{
protected:
	string borrower;
	string returnDate;
public:
	fstream& operator>>(fstream& f)
	{
		f << id << endl;
		f << name << endl;
		f << author << endl;
		f << borrower << endl;
		f << returnDate << endl;
		return f;
	}
	void SetBorrower()
	{
		cout << "|   Enter book borrower (max 256 char, can be empty) : ";
		string r;
		do
		{
			getline(cin, r);
		} while (r.length() >= MAXLENGTH);
		borrower = r;
	}
	void SetBorrower(string& borrower)
	{
		this->borrower = borrower;
	}
	void SetReturnDate()
	{
		auto verify = [](string& s) -> bool {
			int flag = 0; //flag = 0 : verify ok; = 1 : invalid format; = 2 : invalid value;
			if (s.length() != 10) return false;
			if (s[2] != '/' || s[5] != '/') return false;
			for (int i = 0; i < 10; i++)
			{
				if (i == 2 || i == 5) continue;
				if (!isdigit(s[i]))
				{
					return false;
				}
			}
			const auto dd = stoi(s.substr(0, 2));
			const auto mm = stoi(s.substr(3, 2));
			const auto yyyy = stoi(s.substr(6, 4));
			if (mm < 1 || mm > 12) return false;

#pragma region Validate date
			auto shortmonth = { 4,6,9,11 };
			auto longmonth = { 1,3,5,7,8,10,12 };
			if (find(shortmonth.begin(), shortmonth.end(), mm) != shortmonth.end())
			{
				if (dd < 1 || dd > 30) return false;
			}
			else if (find(longmonth.begin(), longmonth.end(), mm) != longmonth.end())
			{
				if (dd < 1 || dd > 31) return false;
			}
			else
			{
				if (yyyy % 4 == 0) //Leap year
				{
					if (dd < 1 || dd > 29) return false;
				}
				else //Normal year
				{
					if (dd < 1 || dd > 28) return false;
				}
			}
#pragma endregion return false when invalid

#pragma region Validate date to current time
			const auto now = chrono::system_clock::now();
			const time_t time = chrono::system_clock::to_time_t(now);
			char a[26];
			ctime_s(a, 26, &time);
			const string datetimestring(a); //Pattern: Wed Dec 15 20:45:44 2021
			const auto m = datetimestring.substr(4, 3);
			const auto month = m == "Jan" ? 1 : m == "Feb" ? 2 : m == "Mar" ? 3 : m == "Apr" ? 4 : m == "May" ? 5 : m == "Jun" ? 6 : m == "Jul" ? 7 : m == "Aug" ? 8 : m == "Sep" ? 9 : m == "Oct" ? 10 : m == "Nov" ? 11 : 12;
			const auto day = stoi(datetimestring.substr(8, 2));
			const auto year = stoi(datetimestring.substr(20, 4));
			if (year > yyyy) return false;
			if (month > mm) return false;
			if (day >= dd) return false;
#pragma endregion

			switch (flag)
			{
			case 0: //OK
				return true;
			case 1: //Invalid format
				cout << "|   " << "Invalid date format!\n" << endl;
				return false;
			case 2: //Invalid value
				cout << "|   " << "Invalid date value! (the date may conflict with current system time)\n" << endl;
				return false;
			}
		};
		cout << "|   Enter book return date (dd/mm/yyyy, can leave empty if you are adding new book) : ";
		string r;
		do
		{
			getline(cin, r);
			if (r.empty() || borrower.empty()) break; //if borrower empty and input empty -> skip input
		} while (!verify(r));
		returnDate = r;
	}
	void SetReturnDate(string& returnDate)
	{
		this->returnDate = returnDate;
	}
	void Print() const
	{
		__super::Print();
		cout << "|     Who borrow    : " << borrower << endl;
		cout << "|     Return date   : " << returnDate << endl;
	}
};

class CardHolder
{
protected:
	vector<BookCard> cards;
public:
	void Initialize()
	{
		cards.clear();
		cout << "|   Card holder initialized!\n";
	}
	bool operator << (fstream& f)
	{
		int size;
		if (f >> size)
		{
			f >> ws;
		}
		else
		{
			return false;
		}
		string id;
		string name;
		string author;
		string discarded;
		for (int i = 0; i < size; i++)
		{
			BookCard card;
			getline(f, id);
			getline(f, name);
			getline(f, author);
			getline(f, discarded);
			getline(f, discarded);
			card.SetId(id);
			card.SetName(name);
			card.SetAuthor(author);
			cards.push_back(card);
		}
		f.clear();
		f.seekg(0, ios::beg);
		return true;
	}
	BookCard AddCard()
	{
		BookCard card;
		card.SetId();
		card.SetName();
		card.SetAuthor();
		cards.push_back(card);
		return card;
	}
};

class BookKeeper : public CardHolder
{
protected:
	vector<BookRecord> books;
public:
	void operator>>(fstream& f) const
	{
		f << books.size() << endl;
		for (auto book : books)
		{
			book >> f;
		}
	}
	void operator<<(fstream& f)
	{
		auto res = __super::operator<<(f);
		int size;
		if (f >> size && res == true)
		{
			f >> ws;
		}
		else
		{
			cout << "|   Invalid file format! No change has been made.\n";
			return;
		}

		Initialize();
		cout << "|   Data size: " << size << endl;
		string id;
		string name;
		string author;
		string borrower;
		string returnDate;
		for (int i = 0; i < size; i++)
		{
			cout << "|   Begin parsing data: " << (i % 3 == 0 ? "\\" : i % 3 == 1 ? "|" : "/") << "\r";
			BookRecord record;
			getline(f, id);
			getline(f, name);
			getline(f, author);
			getline(f, borrower);
			getline(f, returnDate);
			record.SetId(id);
			record.SetName(name);
			record.SetAuthor(author);
			record.SetBorrower(borrower);
			record.SetReturnDate(returnDate);
			books.push_back(record);
		}
		cout << endl;
	}
	void Initialize()
	{
		__super::Initialize();
		books.clear();
		cout << "|   Book keeper initialized!\n";
	}
	void ListBook() const
	{
		int i = 0;
		cout << "\n  Listing " << books.size() << " books in library: \n";
		for (BookRecord book : books)
		{
			cout << "===========================================" << endl;
			cout << "|   Book #" << i << endl;
			book.Print();
			cout << "===========================================" << endl;
			i++;
		}
	}
	void AddBook()
	{
		auto card = AddCard();
		BookRecord record;
		record.SetId(card.GetId());
		record.SetName(card.GetName());
		record.SetAuthor(card.GetAuthor());
		record.SetBorrower();
		record.SetReturnDate();
		books.push_back(record);
	}
	void InvokeBorrow()
	{
		BookRecord book;
		const string prefix = "|   ";
		const string suffix = "\n";
		cout << prefix << "You want to borrow a book. But you have to provide information about YOU and which book you want to borrow." << suffix;
		cout << prefix << "What do you want to enter? Book's id or name? Enter \"id\" for id, otherwise for name. First book matches will be altered : ";
		string option;
		getline(cin, option);
		transform(option.begin(), option.end(), option.begin(), tolower);
		string bookiden;
		if (option == "id")
		{
			book.SetId();
			bookiden = book.GetId();
		}
		else
		{
			book.SetName();
			bookiden = book.GetName();
		}
		unsigned int i = 0;
		for (BookRecord record : books)
		{
			if (record.GetId() == bookiden || record.GetName() == bookiden)
			{
				break;
			}
			i++;
		}
		if (i >= books.size() - 1)
		{
			cout << prefix << "No book was found!" << suffix;
			return;
		}
		cout << prefix << "Book details : " << suffix;
		books[i].Print();
		cout << prefix << "Do you want to continue? \"y\" to confirm. Otherwise deny : " << suffix;
		string decide;
		getline(cin, decide);
		if (decide == "y" || decide == "Y")
		{

		}
		else
		{
			cout << prefix << "You denied. No change has been made" << suffix;
			return;
		}
		books[i].SetBorrower();
		books[i].SetReturnDate();
		cout << prefix << "Record saved!" << suffix;
	}
	void InvokeReturn()
	{
		BookRecord book;
		const string prefix = "|   ";
		const string suffix = "\n";
		cout << prefix << "You want to return a book. But you have to provide information about which book you want to return." << suffix;
		cout << prefix << "What do you want to enter? Book's id or name? Enter \"id\" for id, otherwise for name. First book matches will be altered : ";
		string option;
		getline(cin, option);
		transform(option.begin(), option.end(), option.begin(), tolower);
		string bookiden;
		if (option == "id")
		{
			book.SetId();
			bookiden = book.GetId();
		}
		else
		{
			book.SetName();
			bookiden = book.GetName();
		}
		unsigned int i = 0;
		for (BookRecord record : books)
		{
			if (record.GetId() == bookiden || record.GetName() == bookiden)
			{
				break;
			}
			i++;
		}
		if (i >= books.size() - 1)
		{
			cout << prefix << "No book was found!" << suffix;
			return;
		}
		cout << prefix << "Book details : " << suffix;
		books[i].Print();
		cout << prefix << "Do you want to continue? \"y\" to confirm. Otherwise deny : " << suffix;
		string decide;
		getline(cin, decide);
		if (decide != "y" && decide != "Y")
		{
			cout << prefix << "You denied. No change has been made" << suffix;
			return;
		}
		string empty;
		books[i].SetBorrower(empty);
		books[i].SetReturnDate(empty);
		cout << prefix << "Book returned!" << suffix;
	}
};

int main()
{
	constexpr unsigned int FILEMAX = 255;
	BookKeeper keeper;
	while (1)
	{
		cout << "\n__________________________________________\n";
		cout << "This program has these following commands:\n";
		cout << "  1. Initialize\n";
		cout << "  2. Save library record to file\n";
		cout << "  3. Read library record to file\n";
		cout << "  4. Add new book to library\n";
		cout << "  5. Borrow a book\n";
		cout << "  6. Return a book\n";
		cout << "  7. List all books\n";
		cout << "Enter your command here (1-7) : ";
		string commandstr;
		int command;
		getline(cin, commandstr);
		try
		{
			command = stoi(commandstr);
			if (command < 1 || command > 7)
			{
				throw invalid_argument("Option out of range.");
			}
		}
		catch (exception& e)
		{
			cout << "|   Invalid command : " << e.what() << endl;
			continue;
		}
		switch (command)
		{
		case 1: //Initialize
			keeper.Initialize();
			break;
		case 2: //Write data
		{
			fstream f;
			cout << "|   Please specify which file you want to write to (file name) : ";
			string filename;
			getline(cin, filename);
			if (filename.length() > FILEMAX)
			{
				cout << "File name too long. No change has been made\n";
				break;
			}
			f.open(filename, ios::in);
			if (f.is_open()) //if file already exist
			{
				//confirm overwrite
				cout << "|   " << filename << " already exist. Confirm overwrite? Type y for yes. no otherwise : ";
				string choice;
				getline(cin, choice);
				transform(choice.begin(), choice.end(), choice.begin(), ::tolower);
				//if user confirm overwrite then open it in truncated mode
				if (choice == "y")
				{
					f.close();
					f.open(filename, ios::in | ios::out | ios::trunc);
				}
				//if user doesn't confirm, ignore.
				else
				{
					cout << "|   You deny overwrite to " << filename << ". No change has been made\n";
					continue;
				}
			}
			else
			{
				//create new file if no file exist
				f.close();
				f.open(filename, ios::out | ios::trunc);
				f.close();
				//open it 
				f.open(filename, ios::in | ios::out | ios::trunc);
			}
			keeper >> f;
			f.close();
			break;
		}
		case 3: //read data
		{
			//ask for file name to read
			cout << "|   Please specify which file you want to read (file name) : ";
			string filename;
			getline(cin, filename);
			//check if file name is too long
			if (filename.length() > FILEMAX)
			{
				cout << "|   File name too long!\n";
				break;
			}
			fstream f;
			f.open(filename, ios::in);
			if (f.is_open())
			{
				keeper << f;
			}
			else
			{
				cout << "|   File \"" << filename << "\" does not exists.\n";
			}
			break;
		}
		case 4: //Add book to library
			keeper.AddBook();
			break;
		case 5: //Borrow a book
			keeper.InvokeBorrow();
			break;
		case 6: //Return a book
			//keeper.InvokeReturn();
			break;
		case 7:
			keeper.ListBook();
			break;
		}
	}
}