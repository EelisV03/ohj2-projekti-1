#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <cstdlib>
#include <algorithm>

using namespace std;


// Parametri row tarkoittaa riviä ja separator annettua merkkiä,
// jolla pilkotaan rivi osiin.
// ignore_empty on totuusarvo, joka kertoo, halutaanko tyhjät osat ohittaa,
// oletusarvo on false.
// Paluuarvona saadaan vektori parts, joka sisältää separator:in pilkkomat kentät.
vector<string> split( const string& row, const char separator ,
                      bool ignore_empty = false )
{
   vector< string > parts;
   string current_part;

   for ( char c : row ) {

       if ( c == separator) {

           if ( !ignore_empty or !current_part.empty() ) {
               parts.push_back(current_part);
           }
           current_part = "";

       } else {
           current_part += c;
       }
   }

   if ( !ignore_empty or !current_part.empty() ) {
       parts.push_back(current_part);
   }

   return parts;
}

// Yhden elokuvan tiedot.
struct Movie {
    string name;
    int year;
    string director;
    vector<string> actors;
};

// Vertailufunktio elokuvien järjestämiseen ilman lambdaa.
bool compare_movies(const Movie& a, const Movie& b)
{
    if ( a.year != b.year ) {
        return a.year < b.year;
    }
    return a.name < b.name;
}


// read yrittää avata tiedoston ja lukea sitä rivi kerrallaan.
// Se tarkistaa myös virheet rivikohtaisesti ja tulostaa niiden ilmoitukset.
// Funktion parametrinä on tiedosto file_name ja se ei palauta mitään.
void read(const string& file_name, map<pair<string, int>,Movie>& movies) {

    ifstream file(file_name);
    if (!file) {
        cout << "Error: file cannot be opened." << endl;
        return;
    }

    int row_number = 1;
    string row;

    // Luetaan tiedostoa rivi kerrallaan.
    while (getline(file, row)) {
        int count = 0;

        for ( char c : row ) {
            if (c == ';') {
                count++;
            }
        }
        // Tarkistetaan onko puolipisteitä tasan 3 rivillä.
        if (count != 3) {
            cout << "Error: row " << row_number << " is erroneous." << endl;
        }

        vector<string> fields = split(row, ';', false);

            // Tarkistetaan rivin kenttien määrä.
            // Jos eri kuin 4, niin tulostetaan virheilmoitus.
            if (fields.size() != 4) {
                cout << "Error: row " << row_number << " is erroneous." << endl;
                continue;
            }

            // Tarkistetaan ettei mikään kentistä ole tyhjä.
            bool empty = false;
            for (auto &p : fields) {
                if (p.empty()) {
                    empty = true;
                    break;
                }
            }

            if (empty) {
                cout << "Error: row " << row_number << " is erroneous."
                     << endl;
                continue;
            }

            //Tietojen tallennus.
                Movie m;
                m.name = fields[0];
                m.year = stoi(fields[1]);
                m.director = fields[2];
                m.actors = split(fields[3],'|',true);

            pair<string, int> key = make_pair(m.name, m.year);
            movies[key] = m;

            row_number++;
    }
}



// Funktio tulostaa aputekstin, jossa selitetään ohjelman komennot
// ja niiden parametrit.
void help() {

    cout << "Commands:" << endl;
    cout << "  help: Print this command guide." << endl;
    cout << "  quit: Quit the application." << endl;
    cout << "  read <file_name>: Read in movie data"
            " from a file." << endl;
    cout << "  movies: List all movies." << endl;
    cout << "  * from <year>: List all movies "
            "released in the given year." << endl;
    cout << "  * by <first_name> <last_name>: List all"
            " movies directed by the given director." << endl;
    cout << "  actors: List all actors." << endl;
    cout << "  * prolific <n>: List the top <n> actors "
            "that have acted in the most movies." << endl;

}

// Funktio muuttaa ohjaajan etunimen ja sukunimen paikan keskenään
// ja poistaa pilkun.
// Funktion parametrinä on movies map-säiliön director arvot.
// Funktio palauttaa ohjaajan nimet käännettynä muodossa "etunimi sukunimi".
string format_director(const string& director) {
    vector<string> parts = split(director, ',', true);

    return parts[1] + " " + parts[0];
}


// Funktio listaa kaikki ohjelmaan luetut elokuvat ensisijaisesti
// julkaisuvuoden mukaan vanhemmasta uudempaan, ja sitten elokuvan
// nimen mukaan aakkosjärjestyksessä.
// Funktion parametrinä on map-säiliö movies, jonka avaimena on (nimi, vuosi)
// ja arvona Movie-struct. Funktio ei palauta mitään.
void print_movies(const map<pair<string, int>, Movie>& movies){

    // Tallennetaan elokuvien tiedot vektoriin list.
    vector<Movie> list;
    for (const auto& data : movies) {
        list.push_back(data.second);
    }

    // Lajitellaan elokuvat vuosiluvun ja aakkosjärjestyksen mukaan ilman lambdaa.
    sort(list.begin(), list.end(), compare_movies);

    // Suoritetaan tulostus.
    for (const Movie& m : list) {
        cout << "  " << m.name << " (" << m.year << ", dir. " << format_director(m.director)
             << ")" << endl;
    }

}

// Listaa ne elokuvat, jotka on julkaistu parametrin määrittämänä vuotena
// elokuvan nimen mukaan aakkosjärjestyksessä.
// Funktion parametrina ovat map-säiliö movies, jonka avaimena on (nimi, vuosi)
// ja arvona Movie-struct ja toisena parametrinä vuosi year.
// Funktio ei palauta mitään.
void print_movies_from(const map<pair<string, int>, Movie>& movies, int year) {
    bool any = false;

    for ( const auto& data : movies ) {
        const Movie& m = data.second;
        if ( m.year == year ) {
            cout << "  " << m.name << " (dir. " <<
                    format_director(m.director) << ")" << endl;
            any = true;
        }
    }

    if ( !any ) {
        cout << "No movies released in " << year << "." << endl;
    }
}


// Listaa ne elokuvat, jotka ovat parametrina annetun henkilön ohjaamia.
// Elokuvat listataan samalla periaatteella, kuin komennossa movies.
// Funktion parametrina ovat map-säiliö movies, jonka avaimena on (nimi, vuosi)
// ja arvona Movie-struct ja toisena parametrina on ohjaaja director.
// Funktio ei palauta mitään.
void print_movies_by(const map<pair<string, int>, Movie>& movies,
                     const string& director) {
    bool any = false;

    // Tallennetaan elokuvien tiedot vektoriin list.
    vector<Movie> list;
    for (const auto& data : movies) {
        list.push_back(data.second);
    }

    // Lajitellaan elokuvat vuosiluvun ja aakkosjärjestyksen mukaan ilman lambdaa.
    sort(list.begin(), list.end(), compare_movies);

    for ( const Movie& m : list ) {
        if ( format_director(m.director) == director ) {
            cout << "  " << m.name << " (" << m.year << ")" << endl;
            any = true;
        }
    }

    if ( !any ) {
        cout << "No movies directed by " << director << "." << endl;
    }

}


// Funktio muuttaa näyttelijän etunimen ja sukunimen paikan keskenään
// ja poistaa pilkun.
// Funktion parametrinä on movies map-säiliön actor arvot.
// Funktio palauttaa näyttelijän nimet käännettynä muodossa "etunimi sukunimi".
string format_actor(const string& actor) {
    vector<string> parts = split(actor, ',', true);

    return parts[1] + " " + parts[0];
}

// Listaa kaikki ohjelman tuntemat näyttelijät ensisijaisesti sukunimen,
// ja sitten etunimen mukaisesti aakkosjärjestykseen, kahdella välilyönnillä
// sisennettynä.
// Funktion parametrinä on map-säiliö movies, jonka avaimena on (nimi, vuosi)
// ja arvona Movie-struct. Funktio ei palauta mitään.
void print_actors(const map<pair<string, int>, Movie>& movies) {

    // Luodaan set-säiliö näyttelijöiden tallentamista varten.
    set<string> actors_set;

    // Tallennetaan näyttelijät set -säiliöön.
    for ( const auto& item : movies ) {
        const Movie& m = item.second;
        for ( const string& actor : m.actors ) {
            actors_set.insert(actor);
        }
    }
    // Tulostukset.
        if ( actors_set.empty() ) {
            cout << "No actors." << endl;
        } else {
            for ( const string& actor : actors_set ) {
                cout << "  " << format_actor(actor) << endl;
            }
        }
    }


void print_actors_prolific(const map<pair<string, int>, Movie>& movies, const string& actors, int number ) {
        // Luodaan set-säiliö näyttelijöiden tallentamista varten.
        map<string, int> actor_count;

        // Tallennetaan näyttelijät set -säiliöön.
        for ( const auto& item : movies ) {
            const Movie& m = item.second;
            for ( const string& actor : m.actors ) {
                actor_count[actor]++;
            }
        }
    }


    int main()
    {

        map<pair<string, int>, Movie> movies;

        while (true) {
            cout << "> ";
            string input;
            getline(cin, input);

            vector<string> fields = split(input, ' ', true);
            string command = fields[0];

            // Kun käyttäjä käyttää "read" -komentoa.
            if ( command == "read" ) {
                if ( fields.size() != 2) {
                    cout << "Error: wrong amount of parameters." << endl;
                } else {
                    string file_name = fields[1];
                    read(file_name, movies);
                }


            } else if ( command == "movies" ) {
                if ( movies.empty()) {
                    cout << "No movies." << endl;
                } else if ( fields.size() == 1 ) {
                    print_movies(movies);
                } else if ( fields.size() == 3 && fields[1] == "from" ) {
                    string year = fields[2];
                    bool digits = true;
                    for ( char c : year) {
                        if ( !isdigit(c)) {
                            digits = false;
                            cout << "Error: invalid parameter." << endl;
                            break;
                        }
                    }
                    if (digits) {

                        int year = stoi(fields[2]);
                        print_movies_from(movies, year);
                    }
                } else if ( fields.size() == 4 && fields[1] == "by" ) {
                    string director = fields[2] + " " + fields[3];
                    print_movies_by(movies, director);
                }

            } else if ( command == "actors" ) {
                if ( fields.size() == 1) {
                    print_actors(movies);}
                else if ( fields.size() == 3 && fields[1] == "prolific" ) {
                            string number = fields[2];
                            bool digits = true;
                            for ( char c : number) {
                                if ( !isdigit(c)) {
                                    digits = false;
                                    cout << "Error: invalid parameter." << endl;
                                    break;
                                }
                            }
                            if (digits) {
                            int number = stoi(fields[2]);
                            print_actors_prolific(movies, actors, number);
                            }
                }
            } else if ( command == "quit" ) {
                return EXIT_SUCCESS;

            } else if ( command == "help" ) {
                help();

            } else {
                cout << "Error: unknown command." << endl;
            }


            cout << " " << endl;
        }
    }
