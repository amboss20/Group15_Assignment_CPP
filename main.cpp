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

    int getScore () { return score; }

    int increaseScore () {
        score++;
        return score;
    }

    int decreaseScore () {
        score--;
        return score;
    }
};

class UserProgress {
    private:
    FlashCard *card;

    public:
    UserProgress (FlashCard *c) { card=c; }

    void progress () {
        cout << "User Progress:" << endl;
        cout << "Question: " << card->getQuestion() << endl;
        cout << "Point: " << card->getScore() << endl;
        cout << endl;
    }


};

void print (string str) {
    cout << str << endl;
}

int main() {
    // sample flashcard object
    FlashCard c1;
    // sample object to track progress
    UserProgress track (&c1);
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
            // score increment test
            c1.increaseScore();
        }

        else if (choice == 2) {
            cout << "Still in development..." << endl;
            cout << "To be developed by Ambriss..." << endl;
            // score decrement test
            c1.decreaseScore();
        }

        else if (choice == 3) {
            track.progress();
        }

        else if  (choice == 4) {
            return 0;
        }

    } while (true);

    return 0;
}