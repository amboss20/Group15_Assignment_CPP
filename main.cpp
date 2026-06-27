#include <iostream>
#include <string>
using namespace std;

enum Difficulty { HARD = 0, MEDIUM = 1, EASY = 2 };

class FlashCard {
private:
    string question;
    string answer;
    Difficulty difficulty;

public:
    FlashCard() { difficulty = MEDIUM; }  

    void setQuestion() {
        cout << "Add a question: ";
        getline(cin, question);
    }

    void setAnswer() {
        cout << "Add the answer: ";
        getline(cin, answer);
    }

    string getQuestion()       { return question; }
    string getAnswer()         { return answer; }
    Difficulty getDifficulty() { return difficulty; }

    void setDifficulty(Difficulty d) { difficulty = d; }

    string getDifficultyLabel() {
        if (difficulty == HARD)   return "Hard";
        if (difficulty == MEDIUM) return "Medium";
        return "Easy";
    }
};

class UserProgress {
private:
    FlashCard *card;  // aggregation

public:
    UserProgress(FlashCard *c) { card = c; }

    void progress() {
        cout << "User Progress:"                          << endl;
        cout << "Question:   " << card->getQuestion()    << endl;
        cout << "Difficulty: " << card->getDifficultyLabel() << endl;
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
        cout << "How difficult was this card?" << endl;
        cout << "1. Hard   (will appear first next time)"  << endl;
        cout << "2. Medium (will appear in the middle)"    << endl;
        cout << "3. Easy   (will appear last next time)"   << endl;
        cout << "Enter choice: ";
        cin >> feedback;
        cin.ignore();

        if (feedback == 1) {
            card->setDifficulty(HARD);
            cout << "Marked as Hard. This card will appear first." << endl;
        } else if (feedback == 2) {
            card->setDifficulty(MEDIUM);
            cout << "Marked as Medium. This card will appear in the middle." << endl;
        } else if (feedback == 3) {
            card->setDifficulty(EASY);
            cout << "Marked as Easy. This card will appear last." << endl;
        } else {
            cout << "Invalid input. Difficulty unchanged." << endl;
        }
    }

    void selectNextCard() {
        cout << endl;
        Difficulty d = card->getDifficulty();
        if (d == HARD) {
            cout << "[Review Order] Marked HARD so this card appears at the front." << endl;
        } else if (d == MEDIUM) {
            cout << "[Review Order] Marked MEDIUM so this card appears in the middle." << endl;
        } else {
            cout << "[Review Order] Marked EASY so this card appears at the back." << endl;
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
    FlashCard     c1;
    UserProgress  track  (&c1);
    StudySession  session(&c1);
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