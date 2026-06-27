#include <iostream>
#include <string>
#include <vector>   
#include <fstream>  

using namespace std;


void print(string str) {
    cout << str << endl;
}

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

    // Helper function so FileManager can load data directly without using cin
    void loadCardDetails(string q, string a, int s) {
        question = q;
        answer = a;
        score = s;
    }

    string getQuestion() { return question; }
    string getAnswer() { return answer; }
    int getScore() { return score; }

    int increaseScore() {
        score++;
        return score;
    }

    int decreaseScore() {
        score--;
        return score;
    }
};

class UserProgress {
private:
    FlashCard *card;

public:
    UserProgress(FlashCard *c) { card = c; }

    void progress() {
        cout << "User Progress:" << endl;
        cout << "Question: " << card->getQuestion() << endl;
        cout << "Point: " << card->getScore() << endl;
        cout << endl;
    }
};

class StudySession {
private:
    FlashCard *card;  // Aggregation via pointer

public:
    StudySession(FlashCard *c) { card = c; }

    void showCard() {
        cout << "Question: " << card->getQuestion() << endl;
        cout << endl;
        cout << "Press ENTER to reveal answer...";
        cin.ignore(); // Pauses and waits for the user to hit Enter
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
        cin.ignore(); // Clean buffer after reading integer

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
        cout << "         REVIEW CARD             " << endl;
        cout << "---------------------------------" << endl;

        showCard();
        recordFeedback();
        selectNextCard();
    }
};

class Deck {
private:
    vector<FlashCard> cards; // Dynamic array to hold flashcards

public:
    void addNewCard() {
        FlashCard newCard;
        newCard.setQuestion();
        newCard.setAnswer();
        cards.push_back(newCard);
        print("Card added successfully!");
    }

    vector<FlashCard>& getCards() {
        return cards;
    }

    void clearDeck() {
        cards.clear();
    }

    // Traditional Quick Quiz
    void playQuiz() {
        if (cards.empty()) {
            print("The deck is empty! Add some cards first.");
            return;
        }

        string userAnswer;
        for (size_t i = 0; i < cards.size(); i++) {
            cout << "\nQuestion: " << cards[i].getQuestion() << endl;
            cout << "Your Answer: ";
            getline(cin, userAnswer);

            if (userAnswer == cards[i].getAnswer()) {
                print("Correct!");
                cards[i].increaseScore();
            } else {
                print("Wrong! The right answer was: " + cards[i].getAnswer());
                cards[i].decreaseScore();
            }

            UserProgress tracker(&cards[i]);
            tracker.progress();
        }
    }

    void playSpacedRepetition() {
        if (cards.empty()) {
            print("The deck is empty! Add some cards first.");
            return;
        }

        for (size_t i = 0; i < cards.size(); i++) {
            
            StudySession session(&cards[i]); 
            session.startSession();
        }
    }
};

class FileManager {
public:
    static void saveToFile(Deck& deck, string filename) {
        ofstream outFile(filename);

        if (!outFile) {
            print("Error: Could not open file for writing.");
            return;
        }

        vector<FlashCard>& cards = deck.getCards();
        for (size_t i = 0; i < cards.size(); i++) {
            outFile << cards[i].getQuestion() << endl;
            outFile << cards[i].getAnswer() << endl;
            outFile << cards[i].getScore() << endl;
        }

        outFile.close();
        print("Deck saved successfully to " + filename);
    }

    static void loadFromFile(Deck& deck, string filename) {
        ifstream inFile(filename);

        if (!inFile) {
            print("No saved file found. Starting with an empty deck.");
            return;
        }

        deck.clearDeck();

        string qLines, aLines, scoreLines;
        while (getline(inFile, qLines) && getline(inFile, aLines) && getline(inFile, scoreLines)) {
            int savedScore = stoi(scoreLines); 

            FlashCard loadedCard;
            loadedCard.loadCardDetails(qLines, aLines, savedScore);
            deck.getCards().push_back(loadedCard);
        }

        inFile.close();
        print("Deck loaded successfully from " + filename);
    }
};

int main() {
    Deck myDeck;
    string filename = "flashcards.txt";

    // Load saved cards at startup
    FileManager::loadFromFile(myDeck, filename);

    int choice = 0;

    
    while (choice != 4) {
        cout << "\n=========================" << endl;
        cout << "    FLASHCARD PROGRAM    " << endl;
        cout << "=========================" << endl;
        cout << "1. Add a New Flashcard" << endl;
        cout << "2. Play Quick Quiz" << endl;
        cout << "3. Play Spaced Repetition Study" << endl;
        cout << "4. Save and Exit" << endl;
        cout << "Enter your choice (1-4): ";
        cin >> choice;

        cin.ignore();

        switch (choice) {
            case 1:
                cout << "\n--- Creating a Card ---" << endl;
                myDeck.addNewCard();
                break;

            case 2:
                myDeck.playQuiz();
                break;

            case 3:
                myDeck.playSpacedRepetition(); // SRS
                break;

            case 4:
                cout << "\nSaving your deck..." << endl;
                FileManager::saveToFile(myDeck, filename);
                cout << "Goodbye! Great job on your code expansion." << endl;
                break;

            default:
                cout << "Invalid selection. Please type 1, 2, 3, or 4." << endl;
                break;
        }
    }

    return 0;
}