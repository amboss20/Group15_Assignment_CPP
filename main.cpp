#include <iostream>
#include <string>
using namespace std;

class FlashCard {
private:
    string question;
    string answer;
    int score;

public:
    FlashCard() { score = 0; }

    void setQuestion() {
        cout << "Add a question: ";
        getline(cin, question);
    }

    void setAnswer() {
        cout << "Add the answer: ";
        getline(cin, answer);
    }

    string getQuestion() { return question; }
    string getAnswer()   { return answer; }
    int    getScore()    { return score; }

    int increaseScore() { return ++score; }
    int decreaseScore() { return --score; }
};

class UserProgress {
private:
    FlashCard *card;  // aggregation

public:
    UserProgress(FlashCard *c) { card = c; }

    void progress() {
        cout << "User Progress:" << endl;
        cout << "Question: " << card->getQuestion() << endl;
        cout << "Score:    " << card->getScore()    << endl;
        cout << endl;
    }
};

class StudySession {
private:
    FlashCard *card;  // aggregation

public:
    StudySession(FlashCard *c) { card = c; }

    void showCard() {
        cout << "Question: " << card->getQuestion() << endl;
        cout << endl;
        cout << "Press ENTER to reveal answer...";
        cin.ignore();
        cout << "Answer: " << card->getAnswer() << endl;
        cout << endl;
    }

    void recordFeedback() {
        int feedback = 0;
        cout << "Did you get it right?" << endl;
        cout << "1. Yes (Easy)" << endl;
        cout << "2. No  (Hard)" << endl;
        cout << "Enter choice: ";
        cin >> feedback;
        cin.ignore();

        if (feedback == 1) {
            card->increaseScore();
            cout << "Great! Score increased to " << card->getScore() << "." << endl;
        } else {
            card->decreaseScore();
            cout << "No worries! Score decreased to " << card->getScore() << "." << endl;
        }
    }

    void selectNextCard() {
        cout << endl;
        if (card->getScore() <= 2) {
            cout << "[Spaced Repetition] Score is low (" << card->getScore()
                 << ") this card will appear again SOON." << endl;
        } else {
            cout << "[Spaced Repetition] Score is good (" << card->getScore()
                 << ") this card will appear LATER." << endl;
        }
    }

    void startSession() {
        cout << endl;
        cout << "---------------------------------" << endl;
        cout << "         REVIEW SESSION          " << endl;
        cout << "---------------------------------" << endl;

        showCard();
        recordFeedback();
        selectNextCard();
    }
};

void print(string str) {
    cout << str << endl;
}

int main() {
    FlashCard c1;
    UserProgress  track  (&c1);
    StudySession  session(&c1);   // aggregation
    int choice = 0;

    do {
        cout << endl;
        print("---------------------------------");
        print("      FLASHCARD CONSOLE          ");
        print("---------------------------------");
        print("1. Add FlashCard");
        print("2. Review Flashcards");
        print("3. View Progress");
        print("4. Exit");
        cout << endl;

        cout << "Enter your choice (number): ";
        cin >> choice;
        cin.ignore();
        cout << endl;

        if (choice == 1) {
            c1.setQuestion();
            c1.setAnswer();
        }
        else if (choice == 2) {
            session.startSession();
        }
        else if (choice == 3) {
            track.progress();
        }
        else if (choice == 4) {
            return 0;
        }

    } while (true);

    return 0;
}