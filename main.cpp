#include <iostream>
#include <string.h>
using namespace std;

class FlashCard {
    private:
    string question;
    string answer;
    int score;

    public:
    FlashCard () { score=0; }

    void setQuestion () {
        cout << "Add a question: ";
        getline (cin, question);
    }

    void setAnswer () {
        cout << "Add the answer: ";
        getline (cin, answer);
    }

    string getQuestion () { return question; }

    string getAnswer () { return answer; }

    int increaseScore () {
        score++;
        return score;
    }

    int decreaseScore () {
        score--;
        return score;
    }
};

void print (string str) {
    cout << str << endl;
}

int main() {
    // sample flashcard object
    FlashCard c1;
    int choice=0;

    do {
        
        cout << endl;
        print ("---------------------------------");
        print ("      FLASHCARD CONSOLE          ");
        print ("---------------------------------");
        print ("1. Add FlashCard");
        print ("2. Review Flashcards");
        print ("3. View Progress");
        print ("4. Exit");
        cout << endl;

        cout << "Enter your choice (number): ";
        cin >> choice;
        cin.ignore();
        cout << endl;

        if (choice == 1) {
            c1.setQuestion();
            c1.setAnswer();
        }

        else if  (choice == 4) {
            return 0;
        }

    } while (true);

    return 0;
}